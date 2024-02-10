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

#include <iostream>
#include <glfw-window.h>

namespace Mgtt::Apps {
    /**
     * @brief The OpenGlViewer class represents a simple OpenGL viewer.
     *
     * This class provides basic functionality for initializing an OpenGL context,
     * rendering scenes, and clearing the rendering buffer.
     */
    class OpenGlViewer {
    public:
        /**
         * @brief Constructs an OpenGlViewer object.
         */
        OpenGlViewer();

        /**
         * @brief Destructs the OpenGlViewer object.
         */
        ~OpenGlViewer();

        /**
         * @brief Initializes the OpenGL viewer, setting up the necessary resources.
         */
        void Init();

        /**
         * @brief Renders the scene using OpenGL.
         *
         * This method is responsible for rendering the contents of the scene using OpenGL.
         */
        void Render();

        /**
         * @brief Clears the rendering buffer and performs cleanup.
         *
         * This method is called to clear the rendering buffer and perform any necessary cleanup
         * before exiting or switching to a new scene.
         */
        void Clear();

    private:
        std::unique_ptr<Mgtt::Window::GlfwWindow> glfwWindow;
    };

} // namespace Mgtt::Apps
