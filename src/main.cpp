// Hand Example
// Author: Yi Kangrui <yikangrui@pku.edu.cn>

//#define DIFFUSE_TEXTURE_MAPPING

#include "gl_env.h"

#include <cstdlib>
#include <cstdio>
#include <config.h>

#ifndef M_PI
#define M_PI (3.1415926535897932)
#endif

#include <iostream>

#include "skeletal_mesh.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

namespace SkeletalAnimation {
    const char *vertex_shader_330 =
            "#version 330 core\n"
            "const int MAX_BONES = 100;\n"
            "uniform mat4 u_bone_transf[MAX_BONES];\n"
            "uniform mat4 u_mvp;\n"
            "layout(location = 0) in vec3 in_position;\n"
            "layout(location = 1) in vec2 in_texcoord;\n"
            "layout(location = 2) in vec3 in_normal;\n"
            "layout(location = 3) in ivec4 in_bone_index;\n"
            "layout(location = 4) in vec4 in_bone_weight;\n"
            "out vec2 pass_texcoord;\n"
            "void main() {\n"
            "    float adjust_factor = 0.0;\n"
            "    for (int i = 0; i < 4; i++) adjust_factor += in_bone_weight[i] * 0.25;\n"
            "    mat4 bone_transform = mat4(1.0);\n"
            "    if (adjust_factor > 1e-3) {\n"
            "        bone_transform -= bone_transform;\n"
            "        for (int i = 0; i < 4; i++)\n"
            "            bone_transform += u_bone_transf[in_bone_index[i]] * in_bone_weight[i] / adjust_factor;\n"
            "	 }\n"
            "    gl_Position = u_mvp * bone_transform * vec4(in_position, 1.0);\n"
            "    pass_texcoord = in_texcoord;\n"
            "}\n";

    const char *fragment_shader_330 =
            "#version 330 core\n"
            "uniform sampler2D u_diffuse;\n"
            "in vec2 pass_texcoord;\n"
            "out vec4 out_color;\n"
            "void main() {\n"
            #ifdef DIFFUSE_TEXTURE_MAPPING
            "    out_color = vec4(texture(u_diffuse, pass_texcoord).xyz, 1.0);\n"
            #else
            "    out_color = vec4(pass_texcoord, 0.0, 1.0);\n"
            #endif
            "}\n";
}

static int current_gesture = 0;
static int current_status = 1; // 0: pause, 1: playing

static bool is_dragging = false;
static double last_cursor_x = 0.0;
static double last_cursor_y = 0.0;
static float rotation_angle_x = 180.0f;
static float rotation_angle_y = 0.0f;
static float rotation_angle_z = 0.0f;
const float ROTATION_SPEED = 0.01f;

static void error_callback(int error, const char *description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_0) current_gesture = 0;
        if (key == GLFW_KEY_1) current_gesture = 1;
        if (key == GLFW_KEY_2) current_gesture = 2;
        if (key == GLFW_KEY_3) current_gesture = 3;
        if (key == GLFW_KEY_4) current_gesture = 4;
        if (key == GLFW_KEY_5) current_gesture = 5;
        if (key == GLFW_KEY_6) current_gesture = 6;
        if (key == GLFW_KEY_7) current_gesture = 7;
        if (key == GLFW_KEY_8) current_gesture = 8;
        if (key == GLFW_KEY_9) current_gesture = 9;
        if (key == GLFW_KEY_P) current_gesture = 10; // paper
        if (key == GLFW_KEY_R) current_gesture = 11; // rock
        if (key == GLFW_KEY_S) current_gesture = 12; // scissors
        if (key == GLFW_KEY_SPACE) current_status = 1 - current_status; // optional keyboard toggle
    }
}

// mouse button state
static bool mouseButtons[8] = {false};

// mouse/camera interaction sensitivities
const float MOUSE_ROTATE_SENS = 0.2f; // degrees per pixel
const float MOUSE_PAN_SENS = 0.01f; // world units per pixel
const float MOUSE_ZOOM_SENS = 0.5f; // world units per scroll

// Interactive single camera state
static glm::fvec3 cam_pos = glm::fvec3(0.0f, 7.6f, 25.0f);
// Euler angles in degrees for UI; convert to radians when computing quaternion
static glm::fvec3 cam_euler_deg = glm::fvec3(0.0f, 0.0f, 0.0f);
static glm::quat cam_quat = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
// optional parameter slot (unused FOV or similar)
static float cam_param = 1.0f;
// Perspective camera parameter: field of view (degrees)
static float cam_fov_deg = 60.0f;

// Helper: convert Euler degrees (pitch=x, yaw=y, roll=z) to quaternion
static glm::quat eulerDegToQuat(const glm::fvec3 &deg) {
    glm::fvec3 rad = glm::radians(deg);
    // Order: yaw (Y), pitch (X), roll (Z)
    glm::fmat4 rot = glm::rotate(glm::identity<glm::fmat4>(), rad.y, glm::fvec3(0.0f, 1.0f, 0.0f));
    rot = glm::rotate(rot, rad.x, glm::fvec3(1.0f, 0.0f, 0.0f));
    rot = glm::rotate(rot, rad.z, glm::fvec3(0.0f, 0.0f, 1.0f));
    return glm::quat_cast(rot);
}

// GLFW mouse callbacks for interactive camera control
static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    ImGuiIO &io = ImGui::GetIO();
    // let ImGui capture mouse when it wants
    if (io.WantCaptureMouse) return;
    if (button >= 0 && button < 8) {
        if (action == GLFW_PRESS) {
            mouseButtons[button] = true;
            glfwGetCursorPos(window, &last_cursor_x, &last_cursor_y);
        } else if (action == GLFW_RELEASE) {
            mouseButtons[button] = false;
        }
    }
}

static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    ImGuiIO &io = ImGui::GetIO();
    if (io.WantCaptureMouse) {
        // still update last cursor so when leaving ImGui control we don't jump
        last_cursor_x = xpos; last_cursor_y = ypos; return;
    }

    float dx = (float)(xpos - last_cursor_x);
    float dy = (float)(ypos - last_cursor_y);
    last_cursor_x = xpos; last_cursor_y = ypos;

    // Rotate: LEFT button drag -> change yaw/pitch
    if (mouseButtons[GLFW_MOUSE_BUTTON_LEFT]) {
        cam_euler_deg.y += dx * MOUSE_ROTATE_SENS; // yaw
        cam_euler_deg.x += dy * MOUSE_ROTATE_SENS; // pitch (invert Y)
        // clamp pitch to avoid flipping
        if (cam_euler_deg.x > 89.0f) cam_euler_deg.x = 89.0f;
        if (cam_euler_deg.x < -89.0f) cam_euler_deg.x = -89.0f;
        return;
    }

    // Pan: RIGHT button drag -> translate camera in local right/up
    if (mouseButtons[GLFW_MOUSE_BUTTON_RIGHT]) {
        // move opposite to mouse delta to match common UX
        cam_pos -= glm::fvec3(1.0f, 0.0f, 0.0f) * dx * MOUSE_PAN_SENS;
        cam_pos += glm::fvec3(0.0f, 1.0f, 0.0f) * dy * MOUSE_PAN_SENS;
        return;
    }
}

static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    ImGuiIO &io = ImGui::GetIO();
    if (io.WantCaptureMouse) return;
    // move camera along its forward direction
    glm::fvec3 forward = glm::rotate(cam_quat, glm::fvec3(0.0f, 0.0f, -1.0f));
    cam_pos += forward * (float)yoffset * MOUSE_ZOOM_SENS;
}


int main(int argc, char *argv[]) {
    GLFWwindow *window;
    GLuint vertex_shader, fragment_shader, program;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__ // for macos
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(800, 800, "OpenGL output", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);
    // register mouse callbacks for interactive camera control
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    if (glewInit() != GLEW_OK)
        exit(EXIT_FAILURE);

    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &SkeletalAnimation::vertex_shader_330, NULL);
    glCompileShader(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &SkeletalAnimation::fragment_shader_330, NULL);
    glCompileShader(fragment_shader);

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    int linkStatus;
    if (glGetProgramiv(program, GL_LINK_STATUS, &linkStatus), linkStatus == GL_FALSE)
        std::cout << "Error occured in glLinkProgram()" << std::endl;

    SkeletalMesh::Scene &sr = SkeletalMesh::Scene::loadScene("Hand", DATA_DIR"/Hand.fbx");
    if (&sr == &SkeletalMesh::Scene::error)
        std::cout << "Error occured in loadMesh()" << std::endl;

    sr.setShaderInput(program, "in_position", "in_texcoord", "in_normal", "in_bone_index", "in_bone_weight");

    float passed_time;
    // initialize passed_time to avoid first-frame large delta
    passed_time = (float)glfwGetTime();
    SkeletalMesh::SkeletonModifier modifier;

    glEnable(GL_DEPTH_TEST);

    const char *finger_prefixes[] = {"thumb", "index", "middle", "ring", "pinky"};
    const char *phalange_suffixes[] = {"_proximal_phalange", "_intermediate_phalange", "_distal_phalange"};
    float current_angles[5][3] = {0};
    float current_swing[5] = {0};
    float animation_speed = 4.0f; // Controls how fast the hand transitions between gestures

    while (!glfwWindowShouldClose(window)) {
    float last_time = passed_time;
    passed_time = (float) glfwGetTime();
    float delta_time = passed_time - last_time;

    if (current_status == 1)
        {
        float target_angles[5][3] = {0};
        float target_swing[5] = {0};

        if (current_gesture == 0) { // OK
            target_angles[0][0] = M_PI * 0.1f; target_angles[0][1] = M_PI * 0.2f; target_angles[0][2] = M_PI * 0.2f;
            target_angles[1][0] = M_PI * 0.3f; target_angles[1][1] = M_PI * 0.3f; target_angles[1][2] = M_PI * 0.3f;
            target_swing[0] = -M_PI * 0.08f; target_swing[1] = M_PI * 0.08f;
        }

        
        else if (current_gesture == 1) {
            target_angles[0][0] = M_PI * 0.12f; target_angles[0][1] = M_PI * 0.2f; target_angles[0][2] = M_PI * 0.4f;
            target_angles[2][0] = M_PI * 0.48f; target_angles[2][1] = M_PI * 0.4f; target_angles[2][2] = M_PI * 0.5f;
            target_angles[3][0] = M_PI * 0.49f; target_angles[3][1] = M_PI * 0.4f; target_angles[3][2] = M_PI * 0.4f;
            target_angles[4][0] = M_PI * 0.48f; target_angles[4][1] = M_PI * 0.39f; target_angles[4][2] = M_PI * 0.5f;
            target_swing[0] = -M_PI * 0.1f; target_swing[2] = -M_PI * 0.02f; target_swing[3] = -M_PI * 0.06f; target_swing[4] = -M_PI * 0.09f;
        }
        else if (current_gesture == 2) {
            target_angles[0][0] = M_PI * 0.12f; target_angles[0][1] = M_PI * 0.2f; target_angles[0][2] = M_PI * 0.4f;
            target_angles[3][0] = M_PI * 0.49f; target_angles[3][1] = M_PI * 0.35f; target_angles[3][2] = M_PI * 0.2f;
            target_angles[4][0] = M_PI * 0.48f; target_angles[4][1] = M_PI * 0.35f; target_angles[4][2] = M_PI * 0.2f;
            target_swing[0] = -M_PI * 0.1f; target_swing[3] = -M_PI * 0; target_swing[4] = M_PI * 0.01f;
        }
        else if (current_gesture == 3) {
            target_angles[0][0] = M_PI * 0.12f; target_angles[0][1] = M_PI * 0.2f; target_angles[0][2] = M_PI * 0.4f;
            target_angles[4][0] = M_PI * 0.48f; target_angles[4][1] = M_PI * 0.35f; target_angles[4][2] = M_PI * 0.2f;
            target_swing[0] = -M_PI * 0.1f; target_swing[4] = -M_PI * 0.09f;
        }
        else if (current_gesture == 4) {
            target_angles[0][0] = M_PI * 0.12f; target_angles[0][1] = M_PI * 0.2f; target_angles[0][2] = M_PI * 0.4f;
            target_swing[0] = -M_PI * 0.1f;
        }
        else if (current_gesture == 6) {
            target_angles[1][0] = M_PI * 0.47f; target_angles[1][1] = M_PI * 0.35f; target_angles[1][2] = M_PI * 0.2f;
            target_angles[2][0] = M_PI * 0.45f; target_angles[2][1] = M_PI * 0.35f; target_angles[2][2] = M_PI * 0.2f;
            target_angles[3][0] = M_PI * 0.47f; target_angles[3][1] = M_PI * 0.35f; target_angles[3][2] = M_PI * 0.2f;
            target_swing[1] = M_PI * 0.057f; target_swing[2] = -M_PI * 0.02f; target_swing[3] = -M_PI * 0.06f;
        }
        else if (current_gesture == 7) {
            target_angles[0][0] = M_PI * 0.2f; target_angles[0][1] = M_PI * 0.0f; target_angles[0][2] = M_PI * 0.0f;
            target_angles[1][0] = M_PI * 0.45f; target_angles[1][1] = M_PI * 0.05f; target_angles[1][2] = M_PI * 0.05f;
            target_angles[2][0] = M_PI * 0.4f; target_angles[2][1] = M_PI * 0.05f; target_angles[2][2] = M_PI * 0.05f;
            target_angles[3][0] = M_PI * 0.4f; target_angles[3][1] = M_PI * 0.05f; target_angles[3][2] = M_PI * 0.05f;
            target_angles[4][0] = M_PI * 0.4f; target_angles[4][1] = M_PI * 0.05f; target_angles[4][2] = M_PI * 0.05f;
            target_swing[0] = -M_PI * 0.3f; target_swing[1] = M_PI * 0.0f; target_swing[2] = -M_PI * 0.02f; target_swing[3] = -M_PI * 0.06f; target_swing[4] = -M_PI * 0.09f;
        }
        else if (current_gesture == 8) {
            target_angles[2][0] = M_PI * 0.48f; target_angles[2][1] = M_PI * 0.35f; target_angles[2][2] = M_PI * 0.2f;
            target_angles[3][0] = M_PI * 0.49f; target_angles[3][1] = M_PI * 0.35f; target_angles[3][2] = M_PI * 0.2f;
            target_angles[4][0] = M_PI * 0.48f; target_angles[4][1] = M_PI * 0.35f; target_angles[4][2] = M_PI * 0.2f;
            target_swing[2] = -M_PI * 0.02f; target_swing[3] = -M_PI * 0.06f; target_swing[4] = -M_PI * 0.09f;
        }
        else if (current_gesture == 9) {
            target_angles[0][0] = M_PI * 0.12f; target_angles[0][1] = M_PI * 0.2f; target_angles[0][2] = M_PI * 0.4f;
            target_angles[1][0] = M_PI * 0.0f; target_angles[1][1] = M_PI * 0.4f; target_angles[1][2] = M_PI * 0.5f;
            target_angles[2][0] = M_PI * 0.48f; target_angles[2][1] = M_PI * 0.4f; target_angles[2][2] = M_PI * 0.5f;
            target_angles[3][0] = M_PI * 0.49f; target_angles[3][1] = M_PI * 0.4f; target_angles[3][2] = M_PI * 0.4f;
            target_angles[4][0] = M_PI * 0.48f; target_angles[4][1] = M_PI * 0.39f; target_angles[4][2] = M_PI * 0.5f;
            target_swing[0] = -M_PI * 0.1f; target_swing[2] = -M_PI * 0.02f; target_swing[3] = -M_PI * 0.06f; target_swing[4] = -M_PI * 0.09f;
        }
        else if (current_gesture == 11) { // Rock
            target_angles[0][0] = M_PI * 0.12f; target_angles[0][1] = M_PI * 0.2f; target_angles[0][2] = M_PI * 0.4f;
            target_angles[1][0] = M_PI * 0.5f; target_angles[1][1] = M_PI * 0.4f; target_angles[1][2] = M_PI * 0.5f;
            target_angles[2][0] = M_PI * 0.48f; target_angles[2][1] = M_PI * 0.4f; target_angles[2][2] = M_PI * 0.5f;
            target_angles[3][0] = M_PI * 0.49f; target_angles[3][1] = M_PI * 0.4f; target_angles[3][2] = M_PI * 0.4f;
            target_angles[4][0] = M_PI * 0.48f; target_angles[4][1] = M_PI * 0.39f; target_angles[4][2] = M_PI * 0.5f;
            target_swing[0] = -M_PI * 0.1f; target_swing[1] = M_PI * 0.054f; target_swing[2] = -M_PI * 0.02f; target_swing[3] = -M_PI * 0.06f; target_swing[4] = -M_PI * 0.09f;
        } else if (current_gesture == 12) { // Scissors
            target_angles[0][0] = M_PI * 0.12f; target_angles[0][1] = M_PI * 0.2f; target_angles[0][2] = M_PI * 0.4f;
            target_angles[3][0] = M_PI * 0.49f; target_angles[3][1] = M_PI * 0.35f; target_angles[3][2] = M_PI * 0.2f;
            target_angles[4][0] = M_PI * 0.48f; target_angles[4][1] = M_PI * 0.35f; target_angles[4][2] = M_PI * 0.2f;
            target_swing[0] = -M_PI * 0.1f; target_swing[3] = -M_PI * 0; target_swing[4] = M_PI * 0.01f;
        }

        // Smoothly interpolate from current angles to target angles
        for (int i = 0; i < 5; ++i) {
            for (int j = 0; j < 3; ++j) {
                float diff = target_angles[i][j] - current_angles[i][j];
                current_angles[i][j] += diff * animation_speed * delta_time;
                
            }
        }
        for (int i = 0; i < 5; ++i) {
            float diff = target_swing[i] - current_swing[i];
            current_swing[i] += diff * animation_speed * delta_time;
        }

        // Apply rotations to skeleton
        for (int i = 0; i < 5; ++i) {
            for (int j = 0; j < 3; ++j) {
                std::string bone_name = std::string(finger_prefixes[i]) + phalange_suffixes[j];
                modifier[bone_name] = glm::rotate(glm::identity<glm::mat4>(), current_angles[i][j],
                                                  glm::fvec3(0.0, 0.0, 1.0));
            }
        }
        for (int i = 0; i < 5; ++i) {
            std::string bone_name = std::string(finger_prefixes[i]) + phalange_suffixes[0];
            modifier[bone_name] = glm::rotate(glm::identity<glm::mat4>(), current_swing[i],
                                              glm::fvec3(0.0, 1.0, 0.0)) * modifier[bone_name];
        }
        glm::mat4 metacarpals_transform = glm::rotate(glm::identity<glm::mat4>(), glm::radians(rotation_angle_x), glm::fvec3(1.0, 0.0, 0.0));
        metacarpals_transform *= glm::rotate(glm::identity<glm::mat4>(), glm::radians(rotation_angle_y), glm::fvec3(0.0, 1.0, 0.0));
        metacarpals_transform *= glm::rotate(glm::identity<glm::mat4>(), glm::radians(rotation_angle_z), glm::fvec3(0.0, 0.0, 1.0));
        modifier["metacarpals"] = metacarpals_transform;
        }

    // --- Camera: interactive single camera ---
    // Update camera quaternion from Euler inputs (UI sets degrees)
    cam_quat = eulerDegToQuat(cam_euler_deg);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        float ratio;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;

        glClearColor(0.5, 0.5, 0.5, 1.0);

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program);
    // compute view from camera quaternion and position
    glm::fvec3 cam_forward = glm::rotate(cam_quat, glm::fvec3(0.0f, 0.0f, -1.0f));
    glm::fvec3 cam_up = glm::rotate(cam_quat, glm::fvec3(0.0f, 1.0f, 0.0f));
    glm::fvec3 cam_center = cam_pos + cam_forward;
    glm::fmat4 view = glm::lookAt(cam_pos, cam_center, cam_up);
    // Use perspective projection so camera distance affects apparent size
    float fov_rad = glm::radians(cam_fov_deg);
    float near_plane = 0.01f;
    float far_plane = 100.0f;
    glm::fmat4 proj = glm::perspective(fov_rad, ratio, near_plane, far_plane);
    glm::fmat4 mvp = proj * view;
        glUniformMatrix4fv(glGetUniformLocation(program, "u_mvp"), 1, GL_FALSE, (const GLfloat *) &mvp);
        glUniform1i(glGetUniformLocation(program, "u_diffuse"), SCENE_RESOURCE_SHADER_DIFFUSE_CHANNEL);
        SkeletalMesh::Scene::SkeletonTransf bonesTransf;
        sr.getSkeletonTransform(bonesTransf, modifier);
        if (!bonesTransf.empty())
            glUniformMatrix4fv(glGetUniformLocation(program, "u_bone_transf"), bonesTransf.size(), GL_FALSE,
                               (float *) bonesTransf.data());
        sr.render();

        {
            ImGui::Begin("Control Panel");
            ImGui::Text("Hand rotation");
            ImGui::SliderFloat("rotation_x (deg)", &rotation_angle_x, 0.0f, 360.0f);
            ImGui::SliderFloat("rotation_y (deg)", &rotation_angle_y, 0.0f, 360.0f);
            ImGui::SliderFloat("rotation_z (deg)", &rotation_angle_z, 0.0f, 360.0f);

            ImGui::Separator();
            ImGui::Text("Camera");

            // Position: 3 independent sliders with input boxes on the right
            ImGui::Text("Position");
            ImGui::PushID("pos");
            ImGui::SliderFloat("X", &cam_pos.x, -20.0f, 20.0f);
            ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputFloat("##X", &cam_pos.x);
            ImGui::SliderFloat("Y", &cam_pos.y, -20.0f, 20.0f);
            ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputFloat("##Y", &cam_pos.y);
            ImGui::SliderFloat("Z", &cam_pos.z, 0.0f, 50.0f);
            ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputFloat("##Z", &cam_pos.z);
            ImGui::PopID();

            // Euler: 3 independent sliders with input boxes on the right
            ImGui::Text("Euler (deg)");
            ImGui::Text("X: Pitch, Y: Yaw, Z: Roll");
            ImGui::PushID("euler");
            ImGui::SliderFloat("X", &cam_euler_deg.x, -180.0f, 180.0f);
            ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputFloat("##X", &cam_euler_deg.x);
            ImGui::SliderFloat("Y", &cam_euler_deg.y, -180.0f, 180.0f);
            ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputFloat("##Y", &cam_euler_deg.y);
            ImGui::SliderFloat("Z", &cam_euler_deg.z, -180.0f, 180.0f);
            ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputFloat("##Z", &cam_euler_deg.z);
            ImGui::PopID();

            ImGui::SliderFloat("FOV (deg)", &cam_fov_deg, 10.0f, 120.0f);
            if (ImGui::Button("Reset Camera")) { cam_pos = glm::fvec3(0.0f,7.6f,25.0f); cam_euler_deg = glm::fvec3(0.0f); cam_fov_deg = 60.0f; }

            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    SkeletalMesh::Scene::unloadScene("Hand");

    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}