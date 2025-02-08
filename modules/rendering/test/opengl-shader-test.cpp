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

#ifdef MGTT_RENDERING_TEST
#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <GL/glew.h>
#endif>
#include <GLFW/glfw3.h>
#include <gtest/gtest.h>
#include <opengl-shader.h>

#include <glm/glm.hpp>

namespace Mgtt::Rendering::Test {
class OpenGlShaderTest : public ::testing::Test {
 public:
  static GLFWwindow* window;
  static std::unique_ptr<Mgtt::Rendering::OpenGlShader> openGlShader;

 protected:
  void SetUp() override {
    try {
      if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
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

      OpenGlShaderTest::window =
          glfwCreateWindow(800, 600, "test-window", nullptr, nullptr);
      if (!OpenGlShaderTest::window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
        return;
      }
      glfwMakeContextCurrent(OpenGlShaderTest::window);
      if (glewInit() != GLEW_OK) {
        throw std::runtime_error("Glew could not be initialized");
      }
    } catch (const std::runtime_error& ex) {
      std::cerr << ex.what();
    }
  }
  void TearDown() override {
    if (OpenGlShaderTest::window) {
      glfwDestroyWindow(OpenGlShaderTest::window);
      glfwTerminate();
    }
  }
};

GLFWwindow* OpenGlShaderTest::window = nullptr;
std::unique_ptr<Mgtt::Rendering::OpenGlShader> OpenGlShaderTest::openGlShader =
    std::make_unique<Mgtt::Rendering::OpenGlShader>();

// Test case for the Compile method
TEST_F(OpenGlShaderTest, Compile) {
  RecordProperty("Test Description",
                 "Checks if the Compile method compiles the shader program");
  RecordProperty("Expected Result", "No exception is thrown.");

  try {
    std::pair<std::string, std::string> shaderPathes = {
        "assets/shader/core/coordinate.vert",
        "assets/shader/core/coordinate.frag"};
    OpenGlShaderTest::openGlShader->Compile(shaderPathes);
  } catch (...) {
    FAIL() << "An exception has been thrown" << std::endl;
  }
}

// Test case for the Use method
TEST_F(OpenGlShaderTest, Use) {
  RecordProperty("Test Description",
                 "Checks if the Use method activates the shader program");
  RecordProperty("Expected Result", "No exception is thrown.");

  try {
    OpenGlShaderTest::openGlShader->Use();
    OpenGlShaderTest::openGlShader->SetInt("textureMap", 0);
    OpenGlShaderTest::openGlShader->SetMat4("mvp", glm::mat4(1.0f));
  } catch (...) {
    FAIL() << "An exception has been thrown" << std::endl;
  }
}

// Test case for the Clear method
TEST_F(OpenGlShaderTest, Clear) {
  RecordProperty("Test Description",
                 "Checks if the Clear method deletes the shader program");
  RecordProperty("Expected Result", "No exception is thrown.");

  try {
    OpenGlShaderTest::openGlShader->Clear();
    EXPECT_EQ(openGlShader->GetProgramId(), 0)
        << "GetProgramId should return 0 after Clear.";
  } catch (...) {
    FAIL() << "An exception has been thrown" << std::endl;
  }
}
}  // namespace Mgtt::Rendering::Test
#endif
