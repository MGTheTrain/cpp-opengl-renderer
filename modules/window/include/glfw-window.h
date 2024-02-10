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
#include <iwindow.h>
#include <GLFW/glfw3.h>

namespace Mgtt::Window {
    /**
     * @brief Implementation of the IWindow interface using GLFW.
     * 
     * This class provides concrete implementation details for initializing and clearing a window using GLFW.
     */
    class GlfwWindow : public IWindow {
    public:
        /**
         * @brief Default constructor for the GLFW window.
         */
        GlfwWindow();

        /**
         * @brief Destructor for the GLFW window.
         */
        ~GlfwWindow();

        /**
         * @brief Initialize the GLFW window.
         * 
         * This method overrides the corresponding method in the IWindow interface.
         * It is implemented to set up the initial state specific to a GLFW window.
         */
        void Init() override;

        /**
         * @brief Clear the GLFW window's state.
         * 
         * This method overrides the corresponding method in the IWindow interface.
         * It is implemented to reset the internal state of the GLFW window, freeing GLFW resources.
         */
        void Clear() override;
    };

}