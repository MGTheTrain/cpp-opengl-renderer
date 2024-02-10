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

#include <GL/glew.h>
#include <textured-cube-with-imgui-integration.h>

/**
 * @brief Destructs the TexturedCubeWithImguiIntegration  object.
 */
Mgtt::Apps::TexturedCubeWithImguiIntegration ::~TexturedCubeWithImguiIntegration () {
    
}

/**
 * @brief Constructs an TexturedCubeWithImguiIntegration  object.
 */
Mgtt::Apps::TexturedCubeWithImguiIntegration ::TexturedCubeWithImguiIntegration () {
    this->glfwWindow = 
      std::make_unique<Mgtt::Window::GlfwWindow>("textured-cube-with-imgui-integration", 1000.0f, 1000.0f);
}


/**
 * @brief Renders the scene using OpenGL.
 *
 * This method is responsible for rendering the contents of the scene using OpenGL.
 */
void Mgtt::Apps::TexturedCubeWithImguiIntegration ::Render() {
    while(!this->glfwWindow->WindowShouldClose()) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        this->glfwWindow->SwapBuffersAndPollEvents();
    }
}

int main() {
    try {
        Mgtt::Apps::TexturedCubeWithImguiIntegration  TexturedCubeWithImguiIntegration ;
        if (glewInit() != GLEW_OK) { // reuqires glfw
            throw std::runtime_error("GLEW ERROR: Glew could not be initialized");
        }
        TexturedCubeWithImguiIntegration .Render();
    } catch(const std::exception& ex) {
        std::cout << ex.what() << std::endl;
        return 1;
    }
    return 0;
}