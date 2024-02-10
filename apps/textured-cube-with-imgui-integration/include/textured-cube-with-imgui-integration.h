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

namespace Mgtt::Apps {
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
    };

} // namespace Mgtt::Apps
