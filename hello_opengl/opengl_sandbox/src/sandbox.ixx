module;
#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
// #include <stb_image.h> // Removed texture dependency
#include <stdexcept>
#include <string>
#include <vector>

export module opengl_sandbox.sandbox;

import opengl_sandbox.app;
import opengl_sandbox.window;
import opengl_sandbox.shader;
import opengl_sandbox.file_operation;

export namespace opengl_sandbox {
    class Sandbox : public Application {
    public:
        explicit Sandbox(Window &win) : window(win) {}

        ~Sandbox() override = default;

        void on_init() override {
            light_cube_shader =
                    std::make_shared<Shader>("./res/shader/first_vert.glsl", "./res/shader/first_frag.glsl");
            light_cube_shader->use();

            light_shader = std::make_shared<Shader>("./res/shader/light_shader_cube_vert.glsl",
                                                    "./res/shader/light_shader_cube_frag.glsl");
            light_shader->use();

            // Set up vertex data and buffers and configure vertex attributes
            glGenVertexArrays(1, &vertex_array_object);
            glBindVertexArray(vertex_array_object);

            glGenBuffers(1, &vertex_buffer_object);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

            // Position attribute
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void *>(0));
            glEnableVertexAttribArray(0);

            // Normal attribute
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                                  reinterpret_cast<void *>(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            glGenVertexArrays(1, &light_vertex_array_object);
            glBindVertexArray(light_vertex_array_object);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);

            // Light VAO only needs position
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void *>(0));
            glEnableVertexAttribArray(0);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        void on_update(double delta_time) override {
            light_cube_shader->use();

            light_cube_shader->set_vec3("objectColor", 1.0f, 0.5f, 0.31f);
            light_cube_shader->set_vec3("lightColor", 1.0f, 1.0f, 1.0f);
            light_cube_shader->set_vec3("lightPos", light_pos);
            light_cube_shader->set_vec3("viewPos", camera_pos);

            // Projection setup
            glm::mat4 projection = glm::perspective(
                    glm::radians(45.0f),
                    static_cast<float>(window.get_width()) / static_cast<float>(window.get_height()), 0.1f, 100.0f);
            light_cube_shader->set_mat4("projection", projection);

            // View setup
            glm::mat4 view = glm::lookAt(camera_pos, camera_pos + camera_front, camera_up);
            light_cube_shader->set_mat4("view", view);

            // World transformation
            glm::mat4 model = glm::mat4(1.0f);
            light_cube_shader->set_mat4("model", model);

            // Render object
            glBindVertexArray(vertex_array_object);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // Render light source
            light_shader->use();
            light_shader->set_mat4("projection", projection);
            light_shader->set_mat4("view", view);

            model = glm::mat4(1.0f);
            model = glm::translate(model, light_pos);
            model = glm::scale(model, glm::vec3(0.2f));
            light_shader->set_mat4("model", model);

            glBindVertexArray(light_vertex_array_object);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        auto on_event(const SDL_Event &event) -> SDL_AppResult override {
            const auto handle_key_down = [&]() {
                if (event.key.scancode == SDL_SCANCODE_ESCAPE) {
                    return SDL_APP_SUCCESS;
                }
                const float camera_speed = 0.1f;
                if (event.key.scancode == SDL_SCANCODE_W) {
                    camera_pos += camera_front * camera_speed;
                }
                if (event.key.scancode == SDL_SCANCODE_A) {
                    camera_pos -= glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed;
                }
                if (event.key.scancode == SDL_SCANCODE_S) {
                    camera_pos -= camera_front * camera_speed;
                }
                if (event.key.scancode == SDL_SCANCODE_D) {
                    camera_pos += glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed;
                }
                return SDL_APP_CONTINUE;
            };

            const auto handle_mouse_motion = [&]() {
                float x_offset = event.motion.xrel * sensitivity;
                float y_offset = -event.motion.yrel * sensitivity;

                yaw += x_offset;
                pitch += y_offset;

                if (pitch > 89.0f)
                    pitch = 89.0f;
                if (pitch < -89.0f)
                    pitch = -89.0f;

                glm::vec3 front;
                front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                front.y = sin(glm::radians(pitch));
                front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                camera_front = glm::normalize(front);
                return SDL_APP_CONTINUE;
            };

            switch (event.type) {
                case SDL_EVENT_KEY_DOWN:
                    return handle_key_down();
                case SDL_EVENT_MOUSE_MOTION:
                    return handle_mouse_motion();
                default:
                    break;
            }
            return SDL_APP_CONTINUE;
        }

        void on_quit() override {
            glDeleteVertexArrays(1, &vertex_array_object);
            glDeleteBuffers(1, &vertex_buffer_object);
        }

    private:
        Window &window;
        std::shared_ptr<Shader> light_cube_shader = nullptr;
        std::shared_ptr<Shader> light_shader = nullptr;
        unsigned int vertex_array_object{};
        unsigned int vertex_buffer_object{};
        unsigned int light_vertex_array_object{};

        glm::vec3 camera_pos{0.0f, 0.0f, 3.0f};
        glm::vec3 camera_front{0.0f, 0.0f, -1.0f};
        glm::vec3 camera_up{0.0f, 1.0f, 0.0f};

        float yaw = -90.0f;
        float pitch = 0.0f;
        float sensitivity = 0.1f;

        glm::vec3 light_pos{1.2f, 1.0f, 2.0f};

        // Vertices with normals (6 floats per vertex)
        const std::vector<float> vertices = {
                -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.5f,  -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f,
                0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f,
                -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f,

                -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,
                0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
                -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,

                -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  -0.5f, 0.5f,  -0.5f, -1.0f, 0.0f,  0.0f,
                -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
                -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,

                0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 1.0f,  0.0f,  0.0f,
                0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
                0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

                -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.5f,  -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,
                0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,
                -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,

                -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,
                0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
                -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f};
    };
} // namespace opengl_sandbox
