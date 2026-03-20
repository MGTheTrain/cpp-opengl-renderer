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

#ifdef MGTT_WINDOW_TEST
#include <glfw-window.h>
#include <gtest/gtest.h>

#include <memory>

namespace Mgtt::Window::Test {

class GlfwWindowTest : public ::testing::Test {
 public:
  static std::unique_ptr<Mgtt::Window::GlfwWindow> glfwWindow;

 protected:
  void SetUp() override {
    if (!glfwWindow) {
      GTEST_SKIP() << "GlfwWindow failed to initialise — skipping";
    }
  }

  static void FramebufferSizeCallback(GLFWwindow*, int32_t, int32_t) {}
  static void ScrollCallback(GLFWwindow*, double, double) {}
  static void KeyCallback(GLFWwindow*, int32_t, int32_t, int32_t, int32_t) {}
};

std::unique_ptr<Mgtt::Window::GlfwWindow> GlfwWindowTest::glfwWindow =
    std::make_unique<Mgtt::Window::GlfwWindow>("test-window", 800, 600);

TEST_F(GlfwWindowTest, GetWindow) {
  RecordProperty("Test Description", "GetWindow returns a non-null pointer");
  RecordProperty("Expected Result", "Pointer is non-null");
  EXPECT_NE(glfwWindow->GetWindow(), nullptr);
}

TEST_F(GlfwWindowTest, SetFramebufferSizeCallback) {
  RecordProperty("Test Description",
                 "SetFramebufferSizeCallback does not throw");
  RecordProperty("Expected Result", "No exception");
  glfwWindow->SetFramebufferSizeCallback(FramebufferSizeCallback);
}

TEST_F(GlfwWindowTest, SetScrollCallback) {
  RecordProperty("Test Description", "SetScrollCallback does not throw");
  RecordProperty("Expected Result", "No exception");
  glfwWindow->SetScrollCallback(ScrollCallback);
}

TEST_F(GlfwWindowTest, SetKeyCallback) {
  RecordProperty("Test Description", "SetKeyCallback does not throw");
  RecordProperty("Expected Result", "No exception");
  glfwWindow->SetKeyCallback(KeyCallback);
}

TEST_F(GlfwWindowTest, SwapBuffersAndPollEvents) {
  RecordProperty("Test Description", "SwapBuffersAndPollEvents does not throw");
  RecordProperty("Expected Result", "No exception");
  glfwWindow->SwapBuffersAndPollEvents();
}

TEST_F(GlfwWindowTest, GetWindowSize) {
  RecordProperty("Test Description", "GetWindowSize returns valid dimensions");
  RecordProperty("Expected Result", "width >= 800, height >= 600");
  auto [width, height] = glfwWindow->GetWindowSize();
  EXPECT_GE(width, 800);
  EXPECT_GE(height, 600);
}

TEST_F(GlfwWindowTest, SetWindowSize) {
  RecordProperty("Test Description", "SetWindowSize does not throw");
  RecordProperty("Expected Result", "No exception");
  glfwWindow->SetWindowSize(1000, 1000);
}

TEST_F(GlfwWindowTest, WindowShouldClose) {
  RecordProperty("Test Description",
                 "WindowShouldClose returns false on a fresh window");
  RecordProperty("Expected Result", "false");
  EXPECT_FALSE(glfwWindow->WindowShouldClose());
}

}  // namespace Mgtt::Window::Test
#endif
