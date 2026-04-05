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
#ifdef MGTT_USD_SUPPORT

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include <gtest/gtest.h>
#include <scene-uploader.h>
#include <usd-scene-importer.h>

#include <memory>

namespace Mgtt::Rendering::Test {

class UsdSceneImporterTest : public ::testing::Test {
 public:
  static Mgtt::Rendering::Scene mgttScene;
  static GLFWwindow* window;
  static std::unique_ptr<Mgtt::Rendering::UsdSceneImporter> usdSceneImporter;
  static std::unique_ptr<Mgtt::Rendering::SceneUploader> sceneUploader;

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

GLFWwindow* UsdSceneImporterTest::window = nullptr;
Mgtt::Rendering::Scene UsdSceneImporterTest::mgttScene;
std::unique_ptr<Mgtt::Rendering::UsdSceneImporter>
    UsdSceneImporterTest::usdSceneImporter =
        std::make_unique<Mgtt::Rendering::UsdSceneImporter>();
std::unique_ptr<Mgtt::Rendering::SceneUploader>
    UsdSceneImporterTest::sceneUploader =
        std::make_unique<Mgtt::Rendering::SceneUploader>();

TEST_F(UsdSceneImporterTest, LoadInvalidUsdFile) {
  RecordProperty("Test Description",
                 "Load returns Err for a non-existent USD file");
  RecordProperty("Expected Result", "Result::err() is true");

  const auto result =
      usdSceneImporter->Load(mgttScene, "assets/scenes/non-existent.usda");

  EXPECT_TRUE(result.err())
      << "Expected an error for a missing file but got Ok";
}

TEST_F(UsdSceneImporterTest, LoadUnsupportedSuffix) {
  RecordProperty("Test Description", "Load returns Err for unsupported suffix");
  RecordProperty("Expected Result", "Result::err() is true");

  const std::pair<std::string_view, std::string_view> shaderPaths{
      "assets/shader/core/pbr.vert", "assets/shader/core/pbr.frag"};
  ASSERT_TRUE(mgttScene.shader.Compile(shaderPaths).ok());

  const auto result = usdSceneImporter->Load(mgttScene, "scene.obj");
  EXPECT_TRUE(result.err());
}

TEST_F(UsdSceneImporterTest, LoadWithoutShader) {
  RecordProperty("Test Description",
                 "Load returns Err when shader is not compiled");
  RecordProperty("Expected Result", "Result::err() is true");

  Mgtt::Rendering::Scene unshadedScene;
  const auto result = usdSceneImporter->Load(
      unshadedScene, "assets/scenes/texture-cat/texture-cat-plane.usda");
  EXPECT_TRUE(result.err());
}

TEST_F(UsdSceneImporterTest, LoadValidUsdFile) {
  RecordProperty("Test Description",
                 "Load parses the USD scene on CPU, Upload transfers to GPU");
  RecordProperty("Expected Result",
                 "Result::ok() for both, scene data and GL handles are valid");

  const std::pair<std::string_view, std::string_view> shaderPaths{
      "assets/shader/core/pbr.vert", "assets/shader/core/pbr.frag"};

  {
    const auto compileResult = mgttScene.shader.Compile(shaderPaths);
    ASSERT_TRUE(compileResult.ok())
        << "Shader compile failed: " << compileResult.error();
  }

  // CPU parse — no GL ids allocated yet
  const auto loadResult = usdSceneImporter->Load(
      mgttScene, "assets/scenes/texture-cat/texture-cat-plane.usda");
  ASSERT_TRUE(loadResult.ok()) << "Load failed: " << loadResult.error();

  EXPECT_FALSE(mgttScene.path.empty());
  EXPECT_FALSE(mgttScene.nodes.empty());

  const auto& meshAfterLoad = mgttScene.nodes[0]->mesh;
  ASSERT_NE(meshAfterLoad, nullptr);
  // CPU-side data is populated
  EXPECT_FALSE(meshAfterLoad->vertexPositionAttribs.empty());
  EXPECT_FALSE(meshAfterLoad->vertexNormalAttribs.empty());
  ASSERT_FALSE(meshAfterLoad->meshPrimitives.empty());
  // GL handles not yet allocated
  EXPECT_EQ(meshAfterLoad->vao, 0u);
  EXPECT_EQ(meshAfterLoad->pos, 0u);
  EXPECT_EQ(meshAfterLoad->normal, 0u);
  EXPECT_EQ(meshAfterLoad->tex, 0u);

  // GPU upload
  const auto uploadResult = sceneUploader->Upload(mgttScene);
  ASSERT_TRUE(uploadResult.ok()) << "Upload failed: " << uploadResult.error();

  const auto& meshAfterUpload = mgttScene.nodes[0]->mesh;
  EXPECT_GT(meshAfterUpload->vao, 0u);
  EXPECT_GT(meshAfterUpload->pos, 0u);
  EXPECT_GT(meshAfterUpload->normal, 0u);
  EXPECT_GT(meshAfterUpload->tex, 0u);
}

TEST_F(UsdSceneImporterTest, ClearScene) {
  RecordProperty("Test Description", "Clear resets all scene fields");
  RecordProperty("Expected Result",
                 "path is empty, textureMap and nodes are empty");

  usdSceneImporter->Clear(mgttScene);

  EXPECT_TRUE(mgttScene.path.empty());
  EXPECT_TRUE(mgttScene.textureMap.empty());
  EXPECT_TRUE(mgttScene.nodes.empty());
}

}  // namespace Mgtt::Rendering::Test

#endif  // MGTT_USD_SUPPORT
#endif  // MGTT_RENDERING_TEST
