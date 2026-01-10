module;
#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <stb_image.h>
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
            shader_program = std::make_shared<Shader>("./res/shader/first_vert.glsl", "./res/shader/first_frag.glsl");
            shader_program->use();

            // Set up vertex data and buffers and configure vertex attributes
            glGenVertexArrays(1, &vertex_array_object);
            glBindVertexArray(vertex_array_object);

            glGenBuffers(1, &vertex_buffer_object);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

            // Position attribute
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void *>(0));
            glEnableVertexAttribArray(0);

            // Texture coord attribute
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                                  reinterpret_cast<void *>(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            stbi_set_flip_vertically_on_load(true);

            // Texture 1
            glActiveTexture(GL_TEXTURE0);
            glGenTextures(1, &texture_1);
            glBindTexture(GL_TEXTURE_2D, texture_1);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            if (const auto img_data = load_image("./res/image/oak_planks.png"); img_data.data != nullptr) {
                const auto format = get_image_format(img_data);
                glTexImage2D(GL_TEXTURE_2D, 0, format, img_data.width, img_data.height, 0, format, GL_UNSIGNED_BYTE,
                             img_data.data);
                glGenerateMipmap(GL_TEXTURE_2D);
                stbi_image_free(img_data.data);
            }
            else {
                stbi_image_free(img_data.data);
            }

            shader_program->set_int("texture1", 0);
            shader_program->set_int("texture2", 0); // Using the same texture for both slots to avoid artifacts

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        void on_update(double delta_time) override {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_1);
            // Bind the same texture to unit 1 just in case the shader mixes them
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture_1);

            shader_program->use();

            // Projection setup
            glm::mat4 projection = glm::perspective(
                    glm::radians(45.0f),
                    static_cast<float>(window.get_width()) / static_cast<float>(window.get_height()), 0.1f, 100.0f);
            shader_program->set_mat4("projection", projection);

            // View setup
            glm::mat4 view = glm::lookAt(camera_pos, camera_pos + camera_front, camera_up);
            auto view_loc = glGetUniformLocation(shader_program->get_id(), "view");
            glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));

            auto model_loc = glGetUniformLocation(shader_program->get_id(), "model");

            glBindVertexArray(vertex_array_object);

            // Draw one rotating cube at the center
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::rotate(model, static_cast<float>(SDL_GetTicks()) / 1000.0f * glm::radians(50.0f),
                                glm::vec3(0.5f, 1.0f, 0.0f));
            glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        auto on_event(const SDL_Event &event) -> SDL_AppResult override {
            // Camera controls - kept for navigating around the single cube
            const auto handle_key_down = [&]() {
                if (event.key.scancode == SDL_SCANCODE_ESCAPE) {
                    return SDL_APP_SUCCESS;
                }
                const float camera_speed = 0.1f;
                // Note: Delta time not passed to event, simplified here
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
            glDeleteTextures(1, &texture_1);
        }

    private:
        Window &window;
        std::shared_ptr<Shader> shader_program = nullptr;
        unsigned int vertex_array_object{};
        unsigned int vertex_buffer_object{};
        unsigned int texture_1{};

        glm::vec3 camera_pos{0.0f, 0.0f, 3.0f};
        glm::vec3 camera_front{0.0f, 0.0f, -1.0f};
        glm::vec3 camera_up{0.0f, 1.0f, 0.0f};

        float yaw = -90.0f;
        float pitch = 0.0f;
        float sensitivity = 0.1f;

        const std::vector<float> vertices = {
                -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
                0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

                -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
                0.5f,  0.5f,  0.5f,  1.0f, 1.0f, -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,

                -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
                -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  1.0f, 0.0f,

                0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f,
                0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

                -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 1.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
                0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

                -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f};
    };
} // namespace opengl_sandbox
