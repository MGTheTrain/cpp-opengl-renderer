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
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <glfw-window.h>

/**
 * @brief Default constructor for the GLFW window.
 *
 * @param name   The name of the window.
 * @param width  The width of the window.
 * @param height The height of the window.
 */
Mgtt::Window::GlfwWindow::GlfwWindow(const std::string& name, uint32_t width,
                                     uint32_t height) {
  if (!glfwInit()) {
    throw std::runtime_error("GLFW ERROR: Failed to initialize GLFW");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);

#ifdef __APPLE__
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#endif
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);  // enable anti-aliasing

  this->window =
      glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
  if (!this->window) {
    glfwTerminate();
    throw std::runtime_error("GLFW ERROR: Failed to create GLFW window");
    return;
  }
  glfwMakeContextCurrent(this->window);
}

/**
 * @brief Destructor for the GLFW window.
 */
Mgtt::Window::GlfwWindow::~GlfwWindow() {
  if (this->window) {
    glfwDestroyWindow(this->window);
    glfwTerminate();
  }
}

/**
 * @brief Get the GLFW window
 *
 * @return The GLFW window
 */
GLFWwindow* Mgtt::Window::GlfwWindow::GetWindow() { return this->window; }

/**
 * @brief Set the framebuffer size callback function for the GLFW window.
 *
 * This method sets the callback function that will be invoked when the
 * framebuffer size of the GLFW window changes.
 *
 * @param callbackFunc The callback function to be set.
 */
void Mgtt::Window::GlfwWindow::SetFramebufferSizeCallback(
    GLFWframebuffersizefun callbackFunc) {
  glfwSetFramebufferSizeCallback(this->window, callbackFunc);
}

/**
 * @brief Set the scroll callback function for the GLFW window.
 *
 * This method sets the callback function that will be invoked when scrolling
 * events occur within the GLFW window.
 *
 * @param callbackFunc The callback function to be set.
 */
void Mgtt::Window::GlfwWindow::SetScrollCallback(GLFWscrollfun callbackFunc) {
  glfwSetScrollCallback(this->window, callbackFunc);
}

/**
 * @brief Set the key callback function for the GLFW window.
 *
 * This method sets the callback function that will be invoked when key events
 * occur within the GLFW window.
 *
 * @param callbackFunc The callback function to be set.
 */
void Mgtt::Window::GlfwWindow::SetKeyCallback(GLFWkeyfun callbackFunc) {
  glfwSetKeyCallback(this->window, callbackFunc);
}

/**
 * @brief Swap the front and back buffers of the GLFW window and poll for
 * events.
 *
 * This method swaps the front and back buffers of the GLFW window, updating the
 * display, and then polls for any pending events.
 */
void Mgtt::Window::GlfwWindow::SwapBuffersAndPollEvents() {
  glfwSwapBuffers(this->window);
  glfwPollEvents();
}

/**
 * @brief Get the width and height of a GLFW window.
 *
 * This method retrieves the width and height of the specified GLFW window.
 *
 * @return A tuple containing the width and height of the window.
 */
std::tuple<int, int> Mgtt::Window::GlfwWindow::GetWindowSize() {
  int32_t width, height;
  glfwGetWindowSize(this->window, &width, &height);
  return std::make_tuple(width, height);
}

/**
 * @brief Sets the width and height of a GLFW window.
 *
 * @param width The new window width
 * @param height The new window height
 */
void Mgtt::Window::GlfwWindow::SetWindowSize(uint32_t width, uint32_t height) {
  glfwSetWindowSize(window, width, height);
}

/**
 * @brief Checks if the GLFW window should close.
 *
 * This method queries GLFW to determine if the associated window should be
 * closed.
 *
 * @return True if the window should close, false otherwise.
 */
bool Mgtt::Window::GlfwWindow::WindowShouldClose() {
  return glfwWindowShouldClose(this->window);
}
