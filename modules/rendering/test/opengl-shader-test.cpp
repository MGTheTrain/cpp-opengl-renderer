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

  auto result = openGlShader->Use();
  EXPECT_TRUE(result.ok()) << "Use() failed: " << result.error();
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

TEST_F(OpenGlShaderTest, MoveConstruct) {
  RecordProperty("Test Description", "Move constructor transfers program id");
  RecordProperty("Expected Result", "source id is 0, destination id is > 0");

  const std::pair<std::string_view, std::string_view> shaderPaths{
      "assets/shader/core/coordinate.vert",
      "assets/shader/core/coordinate.frag"};
  ASSERT_TRUE(openGlShader->Compile(shaderPaths).ok());

  const uint32_t kOriginalId = openGlShader->GetProgramId();
  Mgtt::Rendering::OpenGlShader moved(std::move(*openGlShader));

  EXPECT_EQ(openGlShader->GetProgramId(), 0u);
  EXPECT_EQ(moved.GetProgramId(), kOriginalId);
}

TEST_F(OpenGlShaderTest, MoveAssign) {
  RecordProperty("Test Description", "Move assignment transfers program id");
  RecordProperty("Expected Result", "source id is 0, destination id is > 0");

  const std::pair<std::string_view, std::string_view> shaderPaths{
      "assets/shader/core/coordinate.vert",
      "assets/shader/core/coordinate.frag"};
  ASSERT_TRUE(openGlShader->Compile(shaderPaths).ok());

  const uint32_t kOriginalId = openGlShader->GetProgramId();
  Mgtt::Rendering::OpenGlShader other;
  other = std::move(*openGlShader);

  EXPECT_EQ(openGlShader->GetProgramId(), 0u);
  EXPECT_EQ(other.GetProgramId(), kOriginalId);
}

TEST_F(OpenGlShaderTest, SetUniforms) {
  RecordProperty("Test Description", "Set* methods execute without GL errors");
  RecordProperty("Expected Result", "No assertion failures");

  const std::pair<std::string_view, std::string_view> shaderPaths{
      "assets/shader/core/coordinate.vert",
      "assets/shader/core/coordinate.frag"};
  ASSERT_TRUE(openGlShader->Compile(shaderPaths).ok());
  auto result = openGlShader->Use();
  EXPECT_TRUE(result.ok()) << "Use() failed: " << result.error();

  openGlShader->SetBool("unused", true);
  openGlShader->SetInt("textureMap", 0);
  openGlShader->SetFloat("unused", 1.0f);
  openGlShader->SetVec2("unused", glm::vec2(1.0f));
  openGlShader->SetVec2("unused", 1.0f, 2.0f);
  openGlShader->SetVec3("unused", glm::vec3(1.0f));
  openGlShader->SetVec3("unused", 1.0f, 2.0f, 3.0f);
  openGlShader->SetVec4("unused", glm::vec4(1.0f));
  openGlShader->SetVec4("unused", 1.0f, 2.0f, 3.0f, 4.0f);
  openGlShader->SetMat2("unused", glm::mat2(1.0f));
  openGlShader->SetMat3("unused", glm::mat3(1.0f));
  openGlShader->SetMat4("mvp", glm::mat4(1.0f));

  EXPECT_GT(openGlShader->GetProgramId(), 0u);
}

TEST_F(OpenGlShaderTest, CompileEmptyPaths) {
  RecordProperty("Test Description", "Compile returns Err for empty paths");
  RecordProperty("Expected Result", "Result::err() is true");

  const std::pair<std::string_view, std::string_view> emptyPaths{"", ""};
  const auto result = openGlShader->Compile(emptyPaths);
  EXPECT_TRUE(result.err());
}

TEST_F(OpenGlShaderTest, ConstructWithValidPaths) {
  RecordProperty("Test Description",
                 "Constructor with valid paths compiles the shader");
  RecordProperty("Expected Result", "GetProgramId() > 0");

  const std::pair<std::string_view, std::string_view> shaderPaths{
      "assets/shader/core/coordinate.vert",
      "assets/shader/core/coordinate.frag"};

  Mgtt::Rendering::OpenGlShader shader(shaderPaths);
  EXPECT_GT(shader.GetProgramId(), 0u);
}

TEST_F(OpenGlShaderTest, ConstructWithInvalidPaths) {
  RecordProperty("Test Description",
                 "Constructor with invalid paths results in id == 0");
  RecordProperty("Expected Result", "GetProgramId() == 0 or exception thrown");

  const std::pair<std::string_view, std::string_view> badPaths{
      "assets/shader/core/does-not-exist.vert",
      "assets/shader/core/does-not-exist.frag"};

  try {
    Mgtt::Rendering::OpenGlShader shader(badPaths);
    // If we reach here, the constructor did not throw, so the program id should
    // be 0
    EXPECT_EQ(shader.GetProgramId(), 0u);
  } catch (const std::runtime_error& e) {
    // Constructor threw as expected; test passes
    SUCCEED() << "Constructor threw exception as expected: " << e.what();
  } catch (...) {
    // Unexpected exception type
    FAIL() << "Constructor threw unexpected exception type";
  }
}

}  // namespace Mgtt::Rendering::Test
#endif
