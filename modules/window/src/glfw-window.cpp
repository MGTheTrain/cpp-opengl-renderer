// The MIT License
//
// Copyright (c) 2026 MGTheTrain
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

#include <stdexcept>

namespace Mgtt::Window {

GlfwWindow::GlfwWindow(std::string_view name, uint32_t width, uint32_t height) {
  // NOTE: Assume GlfwContext already exists
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
#ifdef __APPLE__
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#endif
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);

  window_ = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height),
                             std::string(name).c_str(), nullptr, nullptr);

  if (window_ == nullptr) {
    throw std::runtime_error("Failed to create GLFW window");
  }

  glfwMakeContextCurrent(window_);
}

GlfwWindow::~GlfwWindow() noexcept {
  if (window_ != nullptr) {
    glfwDestroyWindow(window_);
  }
}

GlfwWindow::GlfwWindow(GlfwWindow&& other) noexcept : window_(other.window_) {
  other.window_ = nullptr;
}

GlfwWindow& GlfwWindow::operator=(GlfwWindow&& other) noexcept {
  if (this != &other) {
    if (window_ != nullptr) {
      glfwDestroyWindow(window_);
    }
    window_ = other.window_;
    other.window_ = nullptr;
  }
  return *this;
}

GLFWwindow* GlfwWindow::GetWindow() const noexcept { return window_; }

void GlfwWindow::SetFramebufferSizeCallback(
    GLFWframebuffersizefun callbackFunc) {
  glfwSetFramebufferSizeCallback(window_, callbackFunc);
}

void GlfwWindow::SetScrollCallback(GLFWscrollfun callbackFunc) {
  glfwSetScrollCallback(window_, callbackFunc);
}

void GlfwWindow::SetKeyCallback(GLFWkeyfun callbackFunc) {
  glfwSetKeyCallback(window_, callbackFunc);
}

void GlfwWindow::SwapBuffersAndPollEvents() {
  glfwSwapBuffers(window_);
  glfwPollEvents();
}

std::tuple<int, int> GlfwWindow::GetWindowSize() const {
  int width = 0;
  int height = 0;
  glfwGetWindowSize(window_, &width, &height);
  return {width, height};
}

void GlfwWindow::SetWindowSize(uint32_t width, uint32_t height) {
  glfwSetWindowSize(window_, static_cast<int>(width), static_cast<int>(height));
}

bool GlfwWindow::WindowShouldClose() const noexcept {
  return glfwWindowShouldClose(window_) != 0;
}

}  // namespace Mgtt::Window
