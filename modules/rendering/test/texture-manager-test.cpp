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
#include <texture-manager.h>

#include <memory>

namespace Mgtt::Rendering::Test {

class TextureManagerTest : public ::testing::Test {
 public:
  static GLFWwindow* window;
  static std::unique_ptr<Mgtt::Rendering::TextureManager> textureManager;

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
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

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

GLFWwindow* TextureManagerTest::window = nullptr;
std::unique_ptr<Mgtt::Rendering::TextureManager>
    TextureManagerTest::textureManager =
        std::make_unique<Mgtt::Rendering::TextureManager>();

TEST_F(TextureManagerTest, LoadFromHdrInvalidPath) {
  RecordProperty("Test Description",
                 "LoadFromHdr returns Err for non-existent HDR file");
  RecordProperty("Expected Result", "Result::err() is true");

  Mgtt::Rendering::RenderTexturesContainer container;
  const auto result = textureManager->LoadFromHdr(
      container, "assets/texture/does-not-exist.hdr");
  EXPECT_TRUE(result.err());
}

TEST_F(TextureManagerTest, LoadFromHdrValidPath) {
  RecordProperty("Test Description",
                 "LoadFromHdr loads HDR and populates cube map texture");
  RecordProperty("Expected Result", "Result::ok() and cubeMapTextureId > 0");

  const std::pair<std::string_view, std::string_view> kEq2CubeMapPaths{
      "assets/shader/core/eq2CubeMap.vert",
      "assets/shader/core/eq2CubeMap.frag"};
  const std::pair<std::string_view, std::string_view> kBrdfLutPaths{
      "assets/shader/core/genBrdf.vert", "assets/shader/core/genBrdf.frag"};
  const std::pair<std::string_view, std::string_view> kEnvMapPaths{
      "assets/shader/core/envMap.vert", "assets/shader/core/envMap.frag"};

  Mgtt::Rendering::RenderTexturesContainer container(
      kEq2CubeMapPaths, kBrdfLutPaths, kEnvMapPaths);

  const auto result =
      textureManager->LoadFromHdr(container, "assets/texture/surgery.jpg");
  ASSERT_TRUE(result.ok()) << result.error();
  EXPECT_GT(container.cubeMapTextureId, 0u);
}

TEST_F(TextureManagerTest, LoadBrdfLutMissingShader) {
  RecordProperty("Test Description",
                 "LoadBrdfLut returns Err when brdfLutShader is not compiled");
  RecordProperty("Expected Result", "Result::err() is true");

  Mgtt::Rendering::RenderTexturesContainer container;
  const auto result = textureManager->LoadBrdfLut(container);
  EXPECT_TRUE(result.err());
}

TEST_F(TextureManagerTest, LoadBrdfLutValid) {
  RecordProperty("Test Description", "LoadBrdfLut populates brdfLutTextureId");
  RecordProperty("Expected Result", "Result::ok() and brdfLutTextureId > 0");

  const std::pair<std::string_view, std::string_view> kEq2CubeMapPaths{
      "assets/shader/core/eq2CubeMap.vert",
      "assets/shader/core/eq2CubeMap.frag"};
  const std::pair<std::string_view, std::string_view> kBrdfLutPaths{
      "assets/shader/core/genBrdf.vert", "assets/shader/core/genBrdf.frag"};
  const std::pair<std::string_view, std::string_view> kEnvMapPaths{
      "assets/shader/core/envMap.vert", "assets/shader/core/envMap.frag"};

  Mgtt::Rendering::RenderTexturesContainer container(
      kEq2CubeMapPaths, kBrdfLutPaths, kEnvMapPaths);

  glGenFramebuffers(1, &container.fboId);
  glGenRenderbuffers(1, &container.rboId);

  const auto result = textureManager->LoadBrdfLut(container);
  ASSERT_TRUE(result.ok()) << result.error();
  EXPECT_GT(container.brdfLutTextureId, 0u);
}

TEST_F(TextureManagerTest, ClearContainer) {
  RecordProperty("Test Description",
                 "Clear resets all GL ids in the container to 0");
  RecordProperty("Expected Result", "All texture/FBO ids are 0 after Clear");

  Mgtt::Rendering::RenderTexturesContainer container;
  textureManager->Clear(container);

  EXPECT_EQ(container.cubeMapTextureId, 0u);
  EXPECT_EQ(container.brdfLutTextureId, 0u);
  EXPECT_EQ(container.hdrTextureId, 0u);
  EXPECT_EQ(container.fboId, 0u);
  EXPECT_EQ(container.rboId, 0u);
}

TEST_F(TextureManagerTest, LoadFromEnvMap) {
  RecordProperty("Test Description",
                 "LoadFromEnvMap returns Ok for valid paths");
  RecordProperty("Expected Result", "Result::ok() is true");

  Mgtt::Rendering::RenderTexturesContainer container;
  const std::vector<std::string> paths{
      "assets/texture/surgery.jpg", "assets/texture/surgery.jpg",
      "assets/texture/surgery.jpg", "assets/texture/surgery.jpg",
      "assets/texture/surgery.jpg", "assets/texture/surgery.jpg"};

  const auto result = textureManager->LoadFromEnvMap(container, paths);
  EXPECT_TRUE(result.ok());
}

TEST_F(TextureManagerTest, RenderTexturesContainerMoveConstruct) {
  RecordProperty("Test Description",
                 "RenderTexturesContainer move constructor transfers GL ids");
  RecordProperty("Expected Result",
                 "moved container has fboId > 0, source has fboId == 0");

  const std::pair<std::string_view, std::string_view> kEq2CubeMapPaths{
      "assets/shader/core/eq2CubeMap.vert",
      "assets/shader/core/eq2CubeMap.frag"};
  const std::pair<std::string_view, std::string_view> kBrdfLutPaths{
      "assets/shader/core/genBrdf.vert", "assets/shader/core/genBrdf.frag"};
  const std::pair<std::string_view, std::string_view> kEnvMapPaths{
      "assets/shader/core/envMap.vert", "assets/shader/core/envMap.frag"};

  Mgtt::Rendering::RenderTexturesContainer container(
      kEq2CubeMapPaths, kBrdfLutPaths, kEnvMapPaths);
  glGenFramebuffers(1, &container.fboId);
  const uint32_t kOriginalFbo = container.fboId;

  Mgtt::Rendering::RenderTexturesContainer moved(std::move(container));

  EXPECT_EQ(moved.fboId, kOriginalFbo);
  EXPECT_EQ(container.fboId, 0u);
}

TEST_F(TextureManagerTest, RenderTexturesContainerMoveAssign) {
  RecordProperty("Test Description",
                 "RenderTexturesContainer move assignment transfers GL ids");
  RecordProperty("Expected Result",
                 "assigned container has fboId > 0, source has fboId == 0");

  const std::pair<std::string_view, std::string_view> kEq2CubeMapPaths{
      "assets/shader/core/eq2CubeMap.vert",
      "assets/shader/core/eq2CubeMap.frag"};
  const std::pair<std::string_view, std::string_view> kBrdfLutPaths{
      "assets/shader/core/genBrdf.vert", "assets/shader/core/genBrdf.frag"};
  const std::pair<std::string_view, std::string_view> kEnvMapPaths{
      "assets/shader/core/envMap.vert", "assets/shader/core/envMap.frag"};

  Mgtt::Rendering::RenderTexturesContainer container(
      kEq2CubeMapPaths, kBrdfLutPaths, kEnvMapPaths);
  glGenFramebuffers(1, &container.fboId);
  const uint32_t kOriginalFbo = container.fboId;

  Mgtt::Rendering::RenderTexturesContainer other;
  other = std::move(container);

  EXPECT_EQ(other.fboId, kOriginalFbo);
  EXPECT_EQ(container.fboId, 0u);
}

}  // namespace Mgtt::Rendering::Test
#endif
