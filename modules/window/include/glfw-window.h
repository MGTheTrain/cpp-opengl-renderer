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

#pragma once
#include <GLFW/glfw3.h>
#include <iwindow.h>

#include <iostream>
#include <memory>
#include <string>
#include <tuple>

namespace Mgtt::Window {
/**
 * @brief Implementation of the IWindow interface using GLFW.
 *
 * This class provides concrete implementation details for initializing and
 * clearing a window using GLFW.
 */
class GlfwWindow : public IWindow {
 public:
  /**
   * @brief Default constructor for the GLFW window.
   *
   * @param name   The name of the window.
   * @param width  The width of the window.
   * @param height The height of the window.
   */
  GlfwWindow(const std::string& name, uint32_t width, uint32_t height);

  /**
   * @brief Destructor for the GLFW window.
   */
  ~GlfwWindow();

  /**
   * @brief Get the GLFW window
   *
   * @return The GLFW window
   */
  GLFWwindow* GetWindow();

  /**
   * @brief Set the framebuffer size callback function for the GLFW window.
   *
   * This method sets the callback function that will be invoked when the
   * framebuffer size of the GLFW window changes.
   *
   * @param callbackFunc The callback function to be set.
   */
  void SetFramebufferSizeCallback(GLFWframebuffersizefun callbackFunc);

  /**
   * @brief Set the scroll callback function for the GLFW window.
   *
   * This method sets the callback function that will be invoked when scrolling
   * events occur within the GLFW window.
   *
   * @param callbackFunc The callback function to be set.
   */
  void SetScrollCallback(GLFWscrollfun callbackFunc);

  /**
   * @brief Set the key callback function for the GLFW window.
   *
   * This method sets the callback function that will be invoked when key events
   * occur within the GLFW window.
   *
   * @param callbackFunc The callback function to be set.
   */
  void SetKeyCallback(GLFWkeyfun callbackFunc);

  /**
   * @brief Swap the front and back buffers of the GLFW window and poll for
   * events.
   *
   * This method swaps the front and back buffers of the GLFW window, updating
   * the display, and then polls for any pending events.
   */
  void SwapBuffersAndPollEvents();

  /**
   * @brief Get the width and height of a GLFW window.
   *
   * This method retrieves the width and height of the specified GLFW window.
   *
   * @return A tuple containing the width and height of the window.
   */
  std::tuple<int, int> GetWindowSize();

  /**
   * @brief Sets the width and height of a GLFW window.
   *
   * @param width The new window width
   * @param height The new window height
   */
  void SetWindowSize(uint32_t width, uint32_t height);

  /**
   * @brief Checks if the GLFW window should close.
   *
   * This method queries GLFW to determine if the associated window should be
   * closed.
   *
   * @return True if the window should close, false otherwise.
   */
  bool WindowShouldClose();

 private:
  GLFWwindow* window;
};

}  // namespace Mgtt::Window
