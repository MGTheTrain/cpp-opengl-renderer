// The MIT License
//
// Copyright (c) 2024 MGTheTrain
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// Maintainers:
// - MGTheTrain 
//
// Contributors:
// - TBD
#pragma once

#include <GL/glew.h>
#include <iostream>
#include <glfw-window.h>
#include <opengl-shader.h>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Mgtt::Apps {
    /**
     * @brief Represents window params 
     */
    struct WindowParams {
        WindowParams() {
            this->name = "";
            this->width = 0.0f;
            this->height = 0.0f;
        }
        std::string name;
        float width;
        float height;
    };

    /**
     * @brief Represents glm matrices
     */
    struct GlmMatrices {
        GlmMatrices() {
            this->model = glm::mat4(1.0f);
            this->view = glm::mat4(1.0f);
            this->projection = glm::mat4(1.0f);
            this->mvp = glm::mat4(1.0f);
        }
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
        glm::mat4 mvp;
    };

    /**
     * @brief Represents opengl objects
     */
    struct OpenGlObjects {
        OpenGlObjects() {}
        ~OpenGlObjects() {
            for (auto vao : this->vaos) {
                glDeleteVertexArrays(1, &vao);
            }
            for (auto vbo : this->vbos) {
                glDeleteBuffers(1, &vbo);
            }
        }
        std::vector<unsigned int> vaos;
        std::vector<unsigned int> vbos;
    };

    /**
     * @brief Represents texture params
     */
    struct TextureParams {
        TextureParams() {
            this->width = 0;
            this->height = 0;
            this->nrChannels = 0;
        }
        int width;
        int height;
        int nrChannels;
    };

    /**
     * @brief The TexturedCubeWithImguiIntegration  class represents a simple OpenGL viewer.
     *
     * This class provides basic functionality for initializing an OpenGL context,
     * rendering scenes, and clearing the rendering buffer.
     */
    class TexturedCubeWithImguiIntegration {
    public:
        /**
         * @brief Constructs an TexturedCubeWithImguiIntegration  object.
         */
        TexturedCubeWithImguiIntegration ();

        /**
         * @brief Destructs the TexturedCubeWithImguiIntegration  object.
         */
        ~TexturedCubeWithImguiIntegration ();

        /**
         * @brief Renders the scene using OpenGL.
         *
         * This method is responsible for rendering the contents of the scene using OpenGL.
         */
        void Render();

        /**
        * @brief Process input for the GLFW window.
        *
        * This function processes input for the specified GLFW window. It can handle keyboard
        * and mouse input events and update the application state accordingly.
        */
        void ProcessInput();
    private:
        /**
        * @brief Callback function for framebuffer size changes.
        *
        * This static callback function is invoked when the framebuffer size of the GLFW window changes.
        * It is typically registered using `glfwSetFramebufferSizeCallback`. The function updates the
        * viewport size based on the new width and height.
        *
        * @param window A pointer to the GLFW window whose framebuffer size has changed.
        * @param width  The new width of the framebuffer.
        * @param height The new height of the framebuffer.
        */
        static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
        std::unique_ptr<Mgtt::Window::GlfwWindow> glfwWindow;
        std::vector<Mgtt::Rendering::OpenGlShader> openGlShaders;
        std::vector<unsigned int> textureMaps;
        std::unique_ptr<WindowParams> windowParams;
        std::unique_ptr<GlmMatrices> glmMatrices;
        std::unique_ptr<OpenGlObjects> openGlObjects;
        std::unique_ptr<TextureParams> textureParams;
    };

} // namespace Mgtt::Apps
