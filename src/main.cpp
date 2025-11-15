#include "common/application.hpp"
#include "common/shader.hpp"
#include "common/mesh.hpp"
#include "common/texture.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>
#include <iostream>
#include <vector>
#include <memory>
#include <filesystem>

namespace fs = std::filesystem;

// Function to create a sphere mesh (you might need to implement this or get it from somewhere)
Mesh create_sphere_mesh(float radius, int sectors, int stacks);

class ParticleApp final : public Application {
public:
    ParticleApp() : Application("Particle System", 1280, 720) {}

private:
    void init() override {
        // Camera setup
        _camera = std::make_unique<ModelViewerCamera>();

        // Load shaders
        _particle_program = Program::create_from_files(
            fs::path("shaders/particle.vert"), 
            fs::path("shaders/particle.frag")
        );
        _planet_program = Program::create_from_files(
            fs::path("shaders/planet.vert"), 
            fs::path("shaders/planet.frag")
        );

        _planet_texture = std::make_unique<Texture2D>(fs::path("textures/2k_saturn.jpg"));
        // Particle data
        _particle_count = 50000;
        std::vector<float> seeds(_particle_count);
        srand(time(0));
        for (int i = 0; i < _particle_count; ++i) {
            seeds[i] = static_cast<float>(rand()) / RAND_MAX;
        }

        glGenVertexArrays(1, &_particle_vao);
        glGenBuffers(1, &_particle_vbo);

        glBindVertexArray(_particle_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _particle_vbo);
        glBufferData(GL_ARRAY_BUFFER, seeds.size() * sizeof(float), seeds.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);

        glVertexAttribDivisor(0, 1); // Instanced rendering

        // Planet model
        _planet_mesh = std::make_unique<Mesh>(create_sphere_mesh(10.0f, 64, 32));

        glBindVertexArray(0);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_PROGRAM_POINT_SIZE);
    }

    void draw_ui() {
        ImGui::Begin("Settings");
        ImGui::Text("Light Settings");
        ImGui::Checkbox("Directional Light", &_use_directional_light);
        if (_use_directional_light) {
            if (ImGui::SliderFloat3("Light Direction", glm::value_ptr(_light_dir), -1.0f, 1.0f)) {
                if (glm::length(_light_dir) < 0.001f) {
                    _light_dir = glm::vec3(0.0f, -1.0f, 0.0f);
                }
                _light_dir = glm::normalize(_light_dir);
            }
        } else {
            ImGui::SliderFloat3("Light Position", glm::value_ptr(_light_pos), -100.0f, 100.0f);
        }
        ImGui::Text("Particle Settings");
        int new_count = _particle_count;
        if (ImGui::SliderInt("Particle Count", &new_count, 10000, 1000000)) {
            if (new_count != _particle_count) {
                std::vector<float> seeds(new_count);
                for (float &s : seeds) { s = rand() / static_cast<float>(RAND_MAX); }
                glBindBuffer(GL_ARRAY_BUFFER, _particle_vbo);
                glBufferData(GL_ARRAY_BUFFER, seeds.size() * sizeof(float), seeds.data(), GL_STATIC_DRAW);
                _particle_count = new_count;
            }
        }
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
        if (_height == 0) aspect = 1.0f; // Avoid division by zero
        glm::mat4 projection = _camera->projection(aspect);
        glm::mat4 view = _camera->view();

        // Draw planet
        glUseProgram(_planet_program->get());
        glm::mat4 model = glm::mat4(1.0f);
        const int light_type = _use_directional_light ? 1 : 0;
        const glm::vec3 light_dir = glm::normalize(_light_dir);
        glUniformMatrix4fv(glGetUniformLocation(_planet_program->get(), "u_Model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(_planet_program->get(), "u_View"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(_planet_program->get(), "u_Projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(glGetUniformLocation(_planet_program->get(), "u_LightPos"), 1, glm::value_ptr(_light_pos));
        glUniform3fv(glGetUniformLocation(_planet_program->get(), "u_LightDir"), 1, glm::value_ptr(light_dir));
        glUniform3fv(glGetUniformLocation(_planet_program->get(), "u_ViewPos"), 1, glm::value_ptr(_camera->position()));
        glUniform3fv(glGetUniformLocation(_planet_program->get(), "u_Color"), 1, glm::value_ptr(glm::vec3(0.9f, 0.8f, 0.6f)));
        glUniform1i(glGetUniformLocation(_planet_program->get(), "u_LightType"), light_type);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _planet_texture->get());
        glUniform1i(glGetUniformLocation(_planet_program->get(), "u_Texture"), 0);
        _planet_mesh->draw();


        // Draw particles
        glUseProgram(_particle_program->get());
        glUniformMatrix4fv(glGetUniformLocation(_particle_program->get(), "u_Projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(_particle_program->get(), "u_View"), 1, GL_FALSE, glm::value_ptr(view));
        glUniform1f(glGetUniformLocation(_particle_program->get(), "u_Time"), (float)glfwGetTime());
        glUniform3fv(glGetUniformLocation(_particle_program->get(), "u_LightPos"), 1, glm::value_ptr(_light_pos));
        glUniform3fv(glGetUniformLocation(_particle_program->get(), "u_LightDir"), 1, glm::value_ptr(light_dir));
        glUniform3fv(glGetUniformLocation(_particle_program->get(), "u_ViewPos"), 1, glm::value_ptr(_camera->position()));
        glUniform1i(glGetUniformLocation(_particle_program->get(), "u_LightType"), light_type);

        glBindVertexArray(_particle_vao);
        glDrawArraysInstanced(GL_POINTS, 0, 1, _particle_count);
        glBindVertexArray(0);
    }

    void update() override {
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
            _camera->orbit(dx * _orbit_sensitivity, -dy * _orbit_sensitivity);
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
    std::unique_ptr<Program> _particle_program;
    std::unique_ptr<Program> _planet_program;
    std::unique_ptr<Mesh> _planet_mesh;
    std::unique_ptr<Texture2D> _planet_texture;
    GLuint _particle_vao = 0, _particle_vbo = 0;
    int _particle_count = 0;
    glm::vec3 _light_pos = glm::vec3(60.0f, 60.0f, 60.0f);
    glm::vec3 _light_dir = glm::normalize(glm::vec3(-0.5f, -1.0f, -0.25f));
    bool _use_directional_light = false;
    int _width = 1280, _height = 720;
    bool _is_orbiting = false;
    double _last_cursor_x = 0.0;
    double _last_cursor_y = 0.0;
    const float _orbit_sensitivity = 0.005f;
    const float _zoom_sensitivity = 1.0f;
};

Mesh create_sphere_mesh(float radius, int sectors, int stacks) {
    std::vector<Mesh::Vertex> vertices;
    std::vector<uint32_t> indices;

    float x, y, z, xy;                              // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
    float s, t;                                     // vertex texCoord

    float sectorStep = 2 * M_PI / sectors;
    float stackStep = M_PI / stacks;
    float sectorAngle, stackAngle;

    for(int i = 0; i <= stacks; ++i)
    {
        stackAngle = M_PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for(int j = 0; j <= sectors; ++j)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            
            // normalized vertex normal (nx, ny, nz)
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;

            // vertex tex coord (s, t) range between [0, 1]
            s = (float)j / sectors;
            t = (float)i / stacks;
            
            vertices.push_back({{x, y, z}, {nx, ny, nz}, {}, {s, t}, {}, {}});
        }
    }

    int k1, k2;
    for(int i = 0; i < stacks; ++i)
    {
        k1 = i * (sectors + 1);     // beginning of current stack
        k2 = k1 + sectors + 1;      // beginning of next stack

        for(int j = 0; j < sectors; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if(i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if(i != (stacks-1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    return Mesh(vertices.data(), vertices.size(), indices.data(), indices.size());
}


int main() {
    try {
        ParticleApp app{};
        app.run();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}