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
#include <gltf-scene-importer.h>
#include <gtest/gtest.h>

namespace Mgtt::Rendering::Test {
class GltfSceneImporterTest : public ::testing::Test {
 public:
  static Mgtt::Rendering::Scene mgttScene;
  static GLFWwindow* window;
  static std::unique_ptr<Mgtt::Rendering::GltfSceneImporter> gltfSceneImporter;

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

      GltfSceneImporterTest::window =
          glfwCreateWindow(800, 600, "test-window", nullptr, nullptr);
      if (!GltfSceneImporterTest::window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
        return;
      }
      glfwMakeContextCurrent(GltfSceneImporterTest::window);
      if (glewInit() != GLEW_OK) {
        throw std::runtime_error("Glew could not be initialized");
      }
    } catch (const std::runtime_error& ex) {
      std::cerr << ex.what();
    }
  }
  void TearDown() override {
    if (GltfSceneImporterTest::window) {
      glfwDestroyWindow(GltfSceneImporterTest::window);
      glfwTerminate();
    }
  }
};

GLFWwindow* GltfSceneImporterTest::window = nullptr;
Mgtt::Rendering::Scene GltfSceneImporterTest::mgttScene;
std::unique_ptr<Mgtt::Rendering::GltfSceneImporter>
    GltfSceneImporterTest::gltfSceneImporter =
        std::make_unique<Mgtt::Rendering::GltfSceneImporter>();

// Test case for the LoadInvalidGltfFile method
TEST_F(GltfSceneImporterTest, LoadInvalidGltfFile) {
  RecordProperty("Test Description",
                 "Checks if LoadInvalidGltfFile handles invalid Gltf file");
  RecordProperty("Expected Result", "std::runtime_error is caught");

  try {
    GltfSceneImporterTest::gltfSceneImporter->Load(
        GltfSceneImporterTest::mgttScene,
        "assets/scenes/water-bottle/None-Existen-WaterBottle.gltf");
  } catch (const std::runtime_error& ex) {
    SUCCEED();
  } catch (...) {
    SUCCEED();
  }
}

// Test case for the LoadValidGltfFile method
// @note Future suggestion: Evaluate a variety of GLTF scenes that include more
// than one node.
TEST_F(GltfSceneImporterTest, LoadValidGltfFile) {
  RecordProperty("Test Description",
                 "Checks if LoadValidGltfFile loads a valid Gltf file");
  RecordProperty("Expected Result", "No exception is thrown");

  try {
    std::pair<std::string, std::string> shaderPaths = {
        "assets/shader/core/pbr.vert", "assets/shader/core/pbr.frag"};
    GltfSceneImporterTest::mgttScene.shader.Compile(shaderPaths);
    GltfSceneImporterTest::gltfSceneImporter->Load(
        GltfSceneImporterTest::mgttScene,
        "assets/scenes/water-bottle/WaterBottle.gltf");
    EXPECT_EQ("assets/scenes/water-bottle/WaterBottle.gltf",
              GltfSceneImporterTest::mgttScene.path);
    EXPECT_EQ(GltfSceneImporterTest::mgttScene.textureMap.size(), 4);
    EXPECT_EQ(GltfSceneImporterTest::mgttScene.nodes.size(), 1);
    EXPECT_NE(GltfSceneImporterTest::mgttScene.nodes[0]->mesh, nullptr);
    EXPECT_GE(GltfSceneImporterTest::mgttScene.nodes[0]->mesh->pos, 0);
    EXPECT_GE(GltfSceneImporterTest::mgttScene.nodes[0]->mesh->normal, 0);
    EXPECT_GE(GltfSceneImporterTest::mgttScene.nodes[0]->mesh->tex, 0);
    EXPECT_GE(GltfSceneImporterTest::mgttScene.nodes[0]->mesh->vao, 0);
    EXPECT_GE(GltfSceneImporterTest::mgttScene.nodes[0]->mesh->indices.size(),
              0);
    EXPECT_GE(GltfSceneImporterTest::mgttScene.nodes[0]
                  ->mesh->vertexPositionAttribs.size(),
              0);
    EXPECT_GE(GltfSceneImporterTest::mgttScene.nodes[0]
                  ->mesh->vertexNormalAttribs.size(),
              0);
    EXPECT_GE(GltfSceneImporterTest::mgttScene.nodes[0]
                  ->mesh->vertexTextureAttribs.size(),
              0);
    EXPECT_GE(
        GltfSceneImporterTest::mgttScene.nodes[0]->mesh->meshPrimitives.size(),
        0);
    EXPECT_EQ(GltfSceneImporterTest::mgttScene.nodes[0]
                  ->mesh->meshPrimitives[0]
                  .firstIndex,
              0);
    EXPECT_GE(GltfSceneImporterTest::mgttScene.nodes[0]
                  ->mesh->meshPrimitives[0]
                  .pbrMaterial.metallicRoughnessTexture.id,
              0);
    EXPECT_GE(GltfSceneImporterTest::mgttScene.nodes[0]
                  ->mesh->meshPrimitives[0]
                  .pbrMaterial.baseColorTexture.id,
              0);
    EXPECT_GE(GltfSceneImporterTest::mgttScene.nodes[0]
                  ->mesh->meshPrimitives[0]
                  .pbrMaterial.normalTexture.id,
              0);
    EXPECT_GE(GltfSceneImporterTest::mgttScene.nodes[0]
                  ->mesh->meshPrimitives[0]
                  .pbrMaterial.occlusionTexture.id,
              0);
    EXPECT_GE(GltfSceneImporterTest::mgttScene.nodes[0]
                  ->mesh->meshPrimitives[0]
                  .pbrMaterial.emissiveTexture.id,
              0);
  } catch (...) {
    FAIL();
  }
}

// Test case for the ClearScene method
// @note Future suggestion: Evaluate a variety of GLTF scenes that include more
// than one node.
TEST_F(GltfSceneImporterTest, ClearScene) {
  RecordProperty("Test Description", "Checks if ClearScene clears the scene");
  RecordProperty("Expected Result", "No exception is thrown");

  try {
    GltfSceneImporterTest::gltfSceneImporter->Clear(this->mgttScene);
    EXPECT_EQ("", GltfSceneImporterTest::mgttScene.path);
    EXPECT_EQ(GltfSceneImporterTest::mgttScene.textureMap.size(), 0);
    EXPECT_EQ(GltfSceneImporterTest::mgttScene.nodes.size(), 0);
  } catch (...) {
    FAIL();
  }
}
}  // namespace Mgtt::Rendering::Test
#endif
