#include "common/application.hpp"
#include "common/shader.hpp"
#include "common/mesh.hpp"
#include "common/texture.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <imgui/imgui.h>
#include <iostream>
#include <vector>
#include <memory>
#include <filesystem>
#include <cmath>

namespace fs = std::filesystem;

Mesh create_cube_mesh(float size);

class CubeApp final : public Application {
public:
    CubeApp() : Application("NormalMap Cube", 1280, 720) {}

private:
    void init() override {
        _camera = std::make_unique<ModelViewerCamera>();
        _cube_program = Program::create_from_files(
            fs::path("shaders/cube.vert"),
            fs::path("shaders/cube.frag")
        );
        _cube_texture = std::make_unique<Texture2D>(fs::path("textures/StoneBricksSplitface/StoneBricksSplitface001_COL_2K.jpg"));
        _cube_normal = std::make_unique<Texture2D>(fs::path("textures/StoneBricksSplitface/StoneBricksSplitface001_NRM_2K.jpg"));
        _cube_mesh = std::make_unique<Mesh>(create_cube_mesh(10.0f));
        glEnable(GL_DEPTH_TEST);
        _last_frame_time = glfwGetTime();
    }

    void draw_ui() {
        ImGui::Begin("Settings");
        ImGui::Text("Light Settings");
        ImGui::Checkbox("Use Normal Map", &_use_normal_map);
        ImGui::SliderFloat3("Light Position", glm::value_ptr(_light_pos), -100.0f, 100.0f);
        if (ImGui::CollapsingHeader("Camera")) {
            _camera->draw_ui();
        }
        ImGui::End();
    }

    void draw() {
        glfwGetFramebufferSize(_window, &_width, &_height);
        glViewport(0, 0, _width, _height);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float aspect = (float)_width / (float)_height;
        if (_height == 0) aspect = 1.0f;
        glm::mat4 projection = _camera->projection(aspect);
        glm::mat4 view = _camera->view();

        glUseProgram(_cube_program->get());
        glm::mat4 model = glm::mat4(1.0f);
        const int light_type = 0;
        const glm::vec3 light_dir = glm::normalize(_light_dir);
        glUniformMatrix4fv(glGetUniformLocation(_cube_program->get(), "u_Model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(_cube_program->get(), "u_View"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(_cube_program->get(), "u_Projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(glGetUniformLocation(_cube_program->get(), "u_LightPos"), 1, glm::value_ptr(_light_pos));
        glUniform3fv(glGetUniformLocation(_cube_program->get(), "u_LightDir"), 1, glm::value_ptr(light_dir));
        glUniform3fv(glGetUniformLocation(_cube_program->get(), "u_ViewPos"), 1, glm::value_ptr(_camera->position()));
        glUniform3fv(glGetUniformLocation(_cube_program->get(), "u_Color"), 1, glm::value_ptr(glm::vec3(0.9f, 0.8f, 0.6f)));
        glUniform1i(glGetUniformLocation(_cube_program->get(), "u_LightType"), light_type);
        glUniform1i(glGetUniformLocation(_cube_program->get(), "u_UseNormalMap"), _use_normal_map);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _cube_texture->get());
        glUniform1i(glGetUniformLocation(_cube_program->get(), "u_Texture"), 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, _cube_normal->get());
        glUniform1i(glGetUniformLocation(_cube_program->get(), "u_NormalMap"), 1);
        _cube_mesh->draw();
    }

    void update() override {
        double current_time = glfwGetTime();
        float delta_seconds = static_cast<float>(current_time - _last_frame_time);
        _last_frame_time = current_time;

        draw_ui();
        draw();
    }

    void cursor_position_callback(double xpos, double ypos) override {
        ImGuiIO &io = ImGui::GetIO();
        if (io.WantCaptureMouse) {
            _last_cursor_x = xpos;
            _last_cursor_y = ypos;
            return;
        }

        if (_is_orbiting && _camera) {
            float dx = static_cast<float>(xpos - _last_cursor_x);
            float dy = static_cast<float>(ypos - _last_cursor_y);
            _camera->orbit(-dx * _orbit_sensitivity, -dy * _orbit_sensitivity);
        }

        _last_cursor_x = xpos;
        _last_cursor_y = ypos;
    }

    void mouse_button_callback(int button, int action, int mods) override {
        (void)mods;
        ImGuiIO &io = ImGui::GetIO();
        if (io.WantCaptureMouse) {
            return;
        }

        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (action == GLFW_PRESS) {
                _is_orbiting = true;
                glfwGetCursorPos(_window, &_last_cursor_x, &_last_cursor_y);
            } else if (action == GLFW_RELEASE) {
                _is_orbiting = false;
            }
        }
    }

    void scroll_callback(double xoffset, double yoffset) override {
        (void)xoffset;
        ImGuiIO &io = ImGui::GetIO();
        if (io.WantCaptureMouse || !_camera) {
            return;
        }

        _camera->zoom(static_cast<float>(-yoffset) * _zoom_sensitivity);
    }

    std::unique_ptr<ModelViewerCamera> _camera;
    std::unique_ptr<Program> _cube_program;
    std::unique_ptr<Mesh> _cube_mesh;
    std::unique_ptr<Texture2D> _cube_texture;
    std::unique_ptr<Texture2D> _cube_normal;
    glm::vec3 _light_pos = glm::vec3(60.0f, 60.0f, 30.0f);
    glm::vec3 _light_dir = glm::normalize(glm::vec3(-0.5f, -1.0f, -0.25f));
    bool _use_normal_map = true;
    int _width = 1280, _height = 720;
    bool _is_orbiting = false;
    double _last_cursor_x = 0.0;
    double _last_cursor_y = 0.0;
    const float _orbit_sensitivity = 0.005f;
    const float _zoom_sensitivity = 1.0f;
    double _last_frame_time = 0.0;
};


// 立方体网格生成，带法线和切线
Mesh create_cube_mesh(float size) {
    float h = size * 0.5f;
    std::vector<Mesh::Vertex> vertices;
    std::vector<uint32_t> indices;
    // 每个面4个顶点，6个面
    struct Face {
        glm::vec3 normal;
        glm::vec4 tangent;
        glm::vec3 v[4];
        glm::vec2 uv[4];
    } faces[6] = {
        // +X
        { {1,0,0}, {0,1,0,1}, { {h,-h,-h}, {h,-h,h}, {h,h,h}, {h,h,-h} }, { {0,0},{1,0},{1,1},{0,1} } },
        // -X
        { {-1,0,0}, {0,1,0,1}, { {-h,-h,h}, {-h,-h,-h}, {-h,h,-h}, {-h,h,h} }, { {0,0},{1,0},{1,1},{0,1} } },
        // +Y
        { {0,1,0}, {1,0,0,1}, { {-h,h,-h}, {h,h,-h}, {h,h,h}, {-h,h,h} }, { {0,0},{1,0},{1,1},{0,1} } },
        // -Y
        { {0,-1,0}, {1,0,0,1}, { {-h,-h,h}, {h,-h,h}, {h,-h,-h}, {-h,-h,-h} }, { {0,0},{1,0},{1,1},{0,1} } },
        // +Z
        { {0,0,1}, {1,0,0,1}, { {-h,-h,h}, {-h,h,h}, {h,h,h}, {h,-h,h} }, { {0,0},{1,0},{1,1},{0,1} } },
        // -Z
        { {0,0,-1}, {1,0,0,1}, { {h,-h,-h}, {h,h,-h}, {-h,h,-h}, {-h,-h,-h} }, { {0,0},{1,0},{1,1},{0,1} } },
    };
    for(int f=0;f<6;++f) {
        int base = vertices.size();
        for(int v=0;v<4;++v) {
            vertices.push_back({faces[f].v[v], faces[f].normal, faces[f].tangent, faces[f].uv[v], {}, {}});
        }
        indices.push_back(base+0); indices.push_back(base+1); indices.push_back(base+2);
        indices.push_back(base+0); indices.push_back(base+2); indices.push_back(base+3);
    }
    return Mesh(vertices.data(), vertices.size(), indices.data(), indices.size());
}


int main() {
    try {
        CubeApp app{};
        app.run();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}