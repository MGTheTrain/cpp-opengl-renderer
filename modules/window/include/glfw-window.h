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

#pragma once
#include <GLFW/glfw3.h>
#include <iwindow.h>

#include <string>
#include <string_view>
#include <tuple>

namespace Mgtt::Window {

/**
 * @brief GLFW-backed IWindow implementation.
 */
class GlfwWindow : public IWindow {
 public:
  GlfwWindow(std::string_view name, uint32_t width, uint32_t height);
  ~GlfwWindow() noexcept override;

  GlfwWindow(const GlfwWindow&) = delete;
  GlfwWindow& operator=(const GlfwWindow&) = delete;
  GlfwWindow(GlfwWindow&&) noexcept;
  GlfwWindow& operator=(GlfwWindow&&) noexcept;

  [[nodiscard]] GLFWwindow* GetWindow() const noexcept;

  void SetFramebufferSizeCallback(GLFWframebuffersizefun callbackFunc);
  void SetScrollCallback(GLFWscrollfun callbackFunc);
  void SetKeyCallback(GLFWkeyfun callbackFunc);

  void SwapBuffersAndPollEvents();

  [[nodiscard]] std::tuple<int, int> GetWindowSize() const;
  void SetWindowSize(uint32_t width, uint32_t height);

  [[nodiscard]] bool WindowShouldClose() const noexcept;

 private:
  GLFWwindow* window_{nullptr};
};

}  // namespace Mgtt::Window
