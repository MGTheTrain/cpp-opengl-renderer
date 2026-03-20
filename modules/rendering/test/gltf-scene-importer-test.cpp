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
#include <gltf-scene-importer.h>
#include <gtest/gtest.h>

#include <iostream>
#include <memory>

namespace Mgtt::Rendering::Test {

class GltfSceneImporterTest : public ::testing::Test {
 public:
  static Mgtt::Rendering::Scene mgttScene;
  static GLFWwindow* window;
  static std::unique_ptr<Mgtt::Rendering::GltfSceneImporter> gltfSceneImporter;

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
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);  // headless — no window on screen

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

GLFWwindow* GltfSceneImporterTest::window = nullptr;
Mgtt::Rendering::Scene GltfSceneImporterTest::mgttScene;
std::unique_ptr<Mgtt::Rendering::GltfSceneImporter>
    GltfSceneImporterTest::gltfSceneImporter =
        std::make_unique<Mgtt::Rendering::GltfSceneImporter>();

TEST_F(GltfSceneImporterTest, LoadInvalidGltfFile) {
  RecordProperty("Test Description",
                 "Load returns Err for a non-existent glTF file");
  RecordProperty("Expected Result", "Result::err() is true");

  const auto result = gltfSceneImporter->Load(
      mgttScene, "assets/scenes/water-bottle/None-Existent-WaterBottle.gltf");

  EXPECT_TRUE(result.err())
      << "Expected an error for a missing file but got Ok";
}

TEST_F(GltfSceneImporterTest, LoadValidGltfFile) {
  RecordProperty("Test Description",
                 "Load populates the scene from a valid glTF file");
  RecordProperty("Expected Result",
                 "Result::ok() is true and scene data is valid");

  const std::pair<std::string_view, std::string_view> shaderPaths{
      "assets/shader/core/pbr.vert", "assets/shader/core/pbr.frag"};

  {
    const auto compileResult = mgttScene.shader.Compile(shaderPaths);
    ASSERT_TRUE(compileResult.ok())
        << "Shader compile failed: " << compileResult.error();
  }

  const auto loadResult = gltfSceneImporter->Load(
      mgttScene, "assets/scenes/water-bottle/WaterBottle.gltf");
  ASSERT_TRUE(loadResult.ok()) << "Load failed: " << loadResult.error();

  EXPECT_EQ(mgttScene.path, "assets/scenes/water-bottle/WaterBottle.gltf");
  EXPECT_EQ(mgttScene.textureMap.size(), 4u);
  ASSERT_EQ(mgttScene.nodes.size(), 1u);

  const auto& mesh = mgttScene.nodes[0]->mesh;
  ASSERT_NE(mesh, nullptr);
  EXPECT_GT(mesh->pos, 0u);
  EXPECT_GT(mesh->normal, 0u);
  EXPECT_GT(mesh->tex, 0u);
  EXPECT_GT(mesh->vao, 0u);
  EXPECT_FALSE(mesh->indices.empty());
  EXPECT_FALSE(mesh->vertexPositionAttribs.empty());
  EXPECT_FALSE(mesh->vertexNormalAttribs.empty());
  EXPECT_FALSE(mesh->vertexTextureAttribs.empty());
  ASSERT_FALSE(mesh->meshPrimitives.empty());

  const auto& prim = mesh->meshPrimitives[0];
  EXPECT_EQ(prim.firstIndex, 0u);
  EXPECT_GT(prim.pbrMaterial.metallicRoughnessTexture.id, 0u);
  EXPECT_GT(prim.pbrMaterial.baseColorTexture.id, 0u);
  EXPECT_GT(prim.pbrMaterial.normalTexture.id, 0u);
  EXPECT_GT(prim.pbrMaterial.occlusionTexture.id, 0u);
  EXPECT_GT(prim.pbrMaterial.emissiveTexture.id, 0u);
}

TEST_F(GltfSceneImporterTest, ClearScene) {
  RecordProperty("Test Description", "Clear resets all scene fields");
  RecordProperty("Expected Result",
                 "path is empty, textureMap and nodes are empty");

  gltfSceneImporter->Clear(mgttScene);

  EXPECT_TRUE(mgttScene.path.empty());
  EXPECT_TRUE(mgttScene.textureMap.empty());
  EXPECT_TRUE(mgttScene.nodes.empty());
}

}  // namespace Mgtt::Rendering::Test
#endif
