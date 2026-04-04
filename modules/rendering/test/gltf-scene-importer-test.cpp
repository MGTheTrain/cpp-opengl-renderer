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
#include <scene-uploader.h>

#include <iostream>
#include <memory>

namespace Mgtt::Rendering::Test {

class GltfSceneImporterTest : public ::testing::Test {
 public:
  static Mgtt::Rendering::Scene mgttScene;
  static GLFWwindow* window;
  static std::unique_ptr<Mgtt::Rendering::GltfSceneImporter> gltfSceneImporter;
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

GLFWwindow* GltfSceneImporterTest::window = nullptr;
Mgtt::Rendering::Scene GltfSceneImporterTest::mgttScene;
std::unique_ptr<Mgtt::Rendering::GltfSceneImporter>
    GltfSceneImporterTest::gltfSceneImporter =
        std::make_unique<Mgtt::Rendering::GltfSceneImporter>();
std::unique_ptr<Mgtt::Rendering::SceneUploader>
    GltfSceneImporterTest::sceneUploader =
        std::make_unique<Mgtt::Rendering::SceneUploader>();

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
                 "Load parses the scene on CPU, Upload transfers it to GPU");
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
  const auto loadResult = gltfSceneImporter->Load(
      mgttScene, "assets/scenes/water-bottle/WaterBottle.gltf");
  ASSERT_TRUE(loadResult.ok()) << "Load failed: " << loadResult.error();

  EXPECT_EQ(mgttScene.path, "assets/scenes/water-bottle/WaterBottle.gltf");
  EXPECT_EQ(mgttScene.textureMap.size(), 4u);
  ASSERT_EQ(mgttScene.nodes.size(), 1u);

  const auto& meshAfterLoad = mgttScene.nodes[0]->mesh;
  ASSERT_NE(meshAfterLoad, nullptr);
  // CPU-side data is populated
  EXPECT_FALSE(meshAfterLoad->indices.empty());
  EXPECT_FALSE(meshAfterLoad->vertexPositionAttribs.empty());
  EXPECT_FALSE(meshAfterLoad->vertexNormalAttribs.empty());
  EXPECT_FALSE(meshAfterLoad->vertexTextureAttribs.empty());
  ASSERT_FALSE(meshAfterLoad->meshPrimitives.empty());
  // GL handles are not yet allocated
  EXPECT_EQ(meshAfterLoad->vao, 0u);
  EXPECT_EQ(meshAfterLoad->pos, 0u);
  EXPECT_EQ(meshAfterLoad->normal, 0u);
  EXPECT_EQ(meshAfterLoad->tex, 0u);

  // GPU upload
  const auto uploadResult = sceneUploader->Upload(mgttScene);
  ASSERT_TRUE(uploadResult.ok()) << "Upload failed: " << uploadResult.error();

  const auto& meshAfterUpload = mgttScene.nodes[0]->mesh;
  EXPECT_GT(meshAfterUpload->pos, 0u);
  EXPECT_GT(meshAfterUpload->normal, 0u);
  EXPECT_GT(meshAfterUpload->tex, 0u);
  EXPECT_GT(meshAfterUpload->vao, 0u);

  const auto& prim = meshAfterUpload->meshPrimitives[0];
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

TEST_F(GltfSceneImporterTest, LoadUnsupportedSuffix) {
  RecordProperty("Test Description", "Load returns Err for unsupported suffix");
  RecordProperty("Expected Result", "Result::err() is true");

  const std::pair<std::string_view, std::string_view> shaderPaths{
      "assets/shader/core/pbr.vert", "assets/shader/core/pbr.frag"};
  ASSERT_TRUE(mgttScene.shader.Compile(shaderPaths).ok());

  const auto result = gltfSceneImporter->Load(mgttScene, "scene.obj");
  EXPECT_TRUE(result.err());
}

TEST_F(GltfSceneImporterTest, LoadWithoutShader) {
  RecordProperty("Test Description",
                 "Load returns Err when shader is not compiled");
  RecordProperty("Expected Result", "Result::err() is true");

  Mgtt::Rendering::Scene unshadedScene;
  const auto result = gltfSceneImporter->Load(
      unshadedScene, "assets/scenes/water-bottle/WaterBottle.gltf");
  EXPECT_TRUE(result.err());
}

TEST_F(GltfSceneImporterTest, UploadWithoutLoad) {
  RecordProperty("Test Description",
                 "Upload on an empty scene returns Ok without crashing");
  RecordProperty("Expected Result", "Result::ok() is true, no GL calls made");

  Mgtt::Rendering::Scene emptyScene;
  const auto result = sceneUploader->Upload(emptyScene);
  EXPECT_TRUE(result.ok());
}

TEST_F(GltfSceneImporterTest, SceneMoveConstruct) {
  RecordProperty("Test Description",
                 "Scene move constructor transfers ownership");
  RecordProperty("Expected Result",
                 "moved scene has nodes, source scene is empty");

  const std::pair<std::string_view, std::string_view> shaderPaths{
      "assets/shader/core/pbr.vert", "assets/shader/core/pbr.frag"};
  ASSERT_TRUE(mgttScene.shader.Compile(shaderPaths).ok());
  ASSERT_TRUE(
      gltfSceneImporter
          ->Load(mgttScene, "assets/scenes/water-bottle/WaterBottle.gltf")
          .ok());
  ASSERT_TRUE(sceneUploader->Upload(mgttScene).ok());

  Mgtt::Rendering::Scene moved(std::move(mgttScene));

  EXPECT_FALSE(moved.nodes.empty());
  EXPECT_TRUE(mgttScene.nodes.empty());

  gltfSceneImporter->Clear(moved);
}

TEST_F(GltfSceneImporterTest, SceneMoveAssign) {
  RecordProperty("Test Description",
                 "Scene move assignment transfers ownership");
  RecordProperty("Expected Result",
                 "assigned scene has nodes, source scene is empty");

  const std::pair<std::string_view, std::string_view> shaderPaths{
      "assets/shader/core/pbr.vert", "assets/shader/core/pbr.frag"};
  ASSERT_TRUE(mgttScene.shader.Compile(shaderPaths).ok());
  ASSERT_TRUE(
      gltfSceneImporter
          ->Load(mgttScene, "assets/scenes/water-bottle/WaterBottle.gltf")
          .ok());
  ASSERT_TRUE(sceneUploader->Upload(mgttScene).ok());

  Mgtt::Rendering::Scene other;
  other = std::move(mgttScene);

  EXPECT_FALSE(other.nodes.empty());
  EXPECT_TRUE(mgttScene.nodes.empty());

  gltfSceneImporter->Clear(other);
}

TEST_F(GltfSceneImporterTest, MeshMoveConstruct) {
  RecordProperty("Test Description",
                 "Mesh move constructor transfers GL handles");
  RecordProperty("Expected Result", "moved mesh vao > 0, source mesh vao == 0");

  const std::pair<std::string_view, std::string_view> shaderPaths{
      "assets/shader/core/pbr.vert", "assets/shader/core/pbr.frag"};
  ASSERT_TRUE(mgttScene.shader.Compile(shaderPaths).ok());
  ASSERT_TRUE(
      gltfSceneImporter
          ->Load(mgttScene, "assets/scenes/water-bottle/WaterBottle.gltf")
          .ok());
  ASSERT_TRUE(sceneUploader->Upload(mgttScene).ok());
  ASSERT_FALSE(mgttScene.nodes.empty());
  ASSERT_NE(mgttScene.nodes[0]->mesh, nullptr);

  auto& srcMesh = *mgttScene.nodes[0]->mesh;
  const uint32_t kOriginalVao = srcMesh.vao;

  Mgtt::Rendering::Mesh moved(std::move(srcMesh));

  EXPECT_EQ(moved.vao, kOriginalVao);
  EXPECT_EQ(srcMesh.vao, 0u);

  gltfSceneImporter->Clear(mgttScene);
}

}  // namespace Mgtt::Rendering::Test
#endif
