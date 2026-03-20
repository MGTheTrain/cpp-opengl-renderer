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

#ifdef MGTT_RENDERING_TEST
#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include <gtest/gtest.h>
#include <opengl-shader.h>

#include <glm/glm.hpp>
#include <iostream>
#include <memory>

namespace Mgtt::Rendering::Test {

class OpenGlShaderTest : public ::testing::Test {
 public:
  static GLFWwindow* window;
  static std::unique_ptr<Mgtt::Rendering::OpenGlShader> openGlShader;

 protected:
  void SetUp() override {
    if (!glfwInit()) {
      GTEST_SKIP() << "glfwInit failed — skipping GL test";
      return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);  // headless

    window = glfwCreateWindow(800, 600, "test-window", nullptr, nullptr);
    if (!window) {
      glfwTerminate();
      GTEST_SKIP() << "glfwCreateWindow failed — skipping GL test";
      return;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
      glfwDestroyWindow(window);
      window = nullptr;
      glfwTerminate();
      GTEST_SKIP() << "glewInit failed — skipping GL test";
    }
  }

  void TearDown() override {
    if (window) {
      glfwDestroyWindow(window);
      window = nullptr;
      glfwTerminate();
    }
  }
};

GLFWwindow* OpenGlShaderTest::window = nullptr;
std::unique_ptr<Mgtt::Rendering::OpenGlShader> OpenGlShaderTest::openGlShader =
    std::make_unique<Mgtt::Rendering::OpenGlShader>();

TEST_F(OpenGlShaderTest, Compile) {
  RecordProperty("Test Description",
                 "Compile returns Ok for valid shader source files");
  RecordProperty("Expected Result", "Result::ok() is true");

  const std::pair<std::string_view, std::string_view> shaderPaths{
      "assets/shader/core/coordinate.vert",
      "assets/shader/core/coordinate.frag"};

  const auto result = openGlShader->Compile(shaderPaths);
  EXPECT_TRUE(result.ok()) << "Compile failed: " << result.error();
}

TEST_F(OpenGlShaderTest, CompileInvalidPaths) {
  RecordProperty("Test Description",
                 "Compile returns Err for non-existent shader files");
  RecordProperty("Expected Result", "Result::err() is true");

  const std::pair<std::string_view, std::string_view> badPaths{
      "assets/shader/core/does-not-exist.vert",
      "assets/shader/core/does-not-exist.frag"};

  const auto result = openGlShader->Compile(badPaths);
  EXPECT_TRUE(result.err())
      << "Expected an error for missing shader files but got Ok";
}

TEST_F(OpenGlShaderTest, Use) {
  RecordProperty("Test Description",
                 "Use binds the shader program without error");
  RecordProperty("Expected Result", "GetProgramId() > 0 and Use() succeeds");

  // Ensure the shader is compiled before Use() is meaningful.
  const std::pair<std::string_view, std::string_view> shaderPaths{
      "assets/shader/core/coordinate.vert",
      "assets/shader/core/coordinate.frag"};
  ASSERT_TRUE(openGlShader->Compile(shaderPaths).ok());

  openGlShader->Use();
  openGlShader->SetInt("textureMap", 0);
  openGlShader->SetMat4("mvp", glm::mat4(1.0f));

  EXPECT_GT(openGlShader->GetProgramId(), 0u);
}

TEST_F(OpenGlShaderTest, Clear) {
  RecordProperty("Test Description",
                 "Clear deletes the program and resets the id to 0");
  RecordProperty("Expected Result", "GetProgramId() == 0 after Clear()");

  openGlShader->Clear();
  EXPECT_EQ(openGlShader->GetProgramId(), 0u);
}

}  // namespace Mgtt::Rendering::Test
#endif
