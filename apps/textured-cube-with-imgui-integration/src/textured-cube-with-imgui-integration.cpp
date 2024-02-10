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

#include <textured-cube-with-imgui-integration.h>

/**
 * @brief Destructs the TexturedCubeWithImguiIntegration  object.
 */
Mgtt::Apps::TexturedCubeWithImguiIntegration::~TexturedCubeWithImguiIntegration () {
    
}

/**
 * @brief Constructs an TexturedCubeWithImguiIntegration  object.
 */
Mgtt::Apps::TexturedCubeWithImguiIntegration::TexturedCubeWithImguiIntegration () {
    this->glfwWindow = 
      std::make_unique<Mgtt::Window::GlfwWindow>("textured-cube-with-imgui-integration", 1000.0f, 1000.0f);
    this->glfwWindow->SetFramebufferSizeCallback(Mgtt::Apps::TexturedCubeWithImguiIntegration::FramebufferSizeCallback);
    if (glewInit() != GLEW_OK) {
        throw std::runtime_error("GLEW ERROR: Glew could not be initialized");
    }
}


/**
 * @brief Renders the scene using OpenGL.
 *
 * This method is responsible for rendering the contents of the scene using OpenGL.
 */
void Mgtt::Apps::TexturedCubeWithImguiIntegration::Render() {
    while(!this->glfwWindow->WindowShouldClose()) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        this->glfwWindow->SwapBuffersAndPollEvents();
    }
}

/**
* @brief Process input for the GLFW window.
*
* This function processes input for the specified GLFW window. It can handle keyboard
* and mouse input events and update the application state accordingly.
*
* @param window A pointer to the GLFW window for which input should be processed.
*/
void Mgtt::Apps::TexturedCubeWithImguiIntegration::ProcessInput()
{
    if (glfwGetKey(this->glfwWindow->GetWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(this->glfwWindow->GetWindow(), true);
    }
}

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
void Mgtt::Apps::TexturedCubeWithImguiIntegration::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    try {
        Mgtt::Apps::TexturedCubeWithImguiIntegration  TexturedCubeWithImguiIntegration;
        TexturedCubeWithImguiIntegration .Render();
    } catch(const std::exception& ex) {
        std::cout << ex.what() << std::endl;
        return 1;
    }
    return 0;
}