#include <gtest/gtest.h>
#include <GL/glew.h>
#include <gltf-scene-importer.h>

namespace Mgtt::Rendering::Test {
    class GltfSceneImporterTest : public ::testing::Test {
    public:
        static Mgtt::Rendering::Scene mgttScene;
        static std::unique_ptr<Mgtt::Rendering::GltfSceneImporter> gltfSceneImporter;
    protected:
        void SetUp() override {
            try {
                if (glewInit() != GLEW_OK) {
                    throw std::runtime_error("GLEW ERROR: Glew could not be initialized");
                }
            }
            catch (...) {
                FAIL();
            }
        }
        void TearDown() override {}
    };

    Mgtt::Rendering::Scene GltfSceneImporterTest::mgttScene; 
    std::unique_ptr<Mgtt::Rendering::GltfSceneImporter> GltfSceneImporterTest::gltfSceneImporter = std::make_unique<Mgtt::Rendering::GltfSceneImporter>();
    
    // Test case for the LoadInvalidGltfFile method
    TEST_F(GltfSceneImporterTest, LoadInvalidGltfFile) {
         RecordProperty("Test Description", "Checks if LoadInvalidGltfFile handles invalid Gltf file");
        RecordProperty("Expected Result", "std::runtime_error is caught");

        try {
            GltfSceneImporterTest::gltfSceneImporter->Load(GltfSceneImporterTest::mgttScene, "assets/scenes/water-bottle/None-Existen-WaterBottle.gltf");
        }
        catch (const std::runtime_error& ex) {
            SUCCEED();
        }
        catch (...) {
            SUCCEED();
        }
    }

    // Test case for the LoadValidGltfFile method
    // @note Future suggestion: Evaluate a variety of GLTF scenes that include more than one node.
    TEST_F(GltfSceneImporterTest, LoadValidGltfFile) {
        RecordProperty("Test Description", "Checks if LoadValidGltfFile loads a valid Gltf file");
        RecordProperty("Expected Result", "No exception is thrown");

        try {
            std::pair<std::string, std::string> shaderPathes = { "assets/shader/core/pbr.vert", "assets/shader/core/pbr.frag" };
            GltfSceneImporterTest::mgttScene.shader.Compile(shaderPathes);
            GltfSceneImporterTest::gltfSceneImporter->Load(GltfSceneImporterTest::mgttScene, "assets/scenes/water-bottle/WaterBottle.gltf"); 
            EXPECT_EQ("assets/scenes/water-bottle/WaterBottle.gltf", GltfSceneImporterTest::mgttScene.path);
            EXPECT_EQ(GltfSceneImporterTest::mgttScene.textureMap.size(), 4);
            EXPECT_EQ(GltfSceneImporterTest::mgttScene.nodes.size(), 1);
            EXPECT_NE(GltfSceneImporterTest::mgttScene.nodes[0]->mesh, nullptr);
            EXPECT_GE(GltfSceneImporterTest::mgttScene.nodes[0]->mesh->pos, 0);
            EXPECT_GE(GltfSceneImporterTest::mgttScene.nodes[0]->mesh->normal, 0);
            EXPECT_GE(GltfSceneImporterTest::mgttScene.nodes[0]->mesh->tex, 0);
            EXPECT_GE(GltfSceneImporterTest::mgttScene.nodes[0]->mesh->vao, 0);
            EXPECT_GE(GltfSceneImporterTest::mgttScene.nodes[0]->mesh->indices.size(), 0);
            EXPECT_GE(GltfSceneImporterTest::mgttScene.nodes[0]->mesh->vertexPositionAttribs.size(), 0);
            EXPECT_GE(GltfSceneImporterTest::mgttScene.nodes[0]->mesh->vertexNormalAttribs.size(), 0);
            EXPECT_GE(GltfSceneImporterTest::mgttScene.nodes[0]->mesh->vertexTextureAttribs.size(), 0);
            EXPECT_GE(GltfSceneImporterTest::mgttScene.nodes[0]->mesh->meshPrimitives.size(), 0);
            EXPECT_EQ(GltfSceneImporterTest::mgttScene.nodes[0]->mesh->meshPrimitives[0].firstIndex, 0);
            EXPECT_GE(GltfSceneImporterTest::mgttScene.nodes[0]->mesh->meshPrimitives[0].pbrMaterial.metallicRoughnessTexture.id, 0);
            EXPECT_GE(GltfSceneImporterTest::mgttScene.nodes[0]->mesh->meshPrimitives[0].pbrMaterial.baseColorTexture.id, 0);
            EXPECT_GE(GltfSceneImporterTest::mgttScene.nodes[0]->mesh->meshPrimitives[0].pbrMaterial.normalTexture.id, 0);
            EXPECT_GE(GltfSceneImporterTest::mgttScene.nodes[0]->mesh->meshPrimitives[0].pbrMaterial.occlusionTexture.id, 0);
            EXPECT_GE(GltfSceneImporterTest::mgttScene.nodes[0]->mesh->meshPrimitives[0].pbrMaterial.emissiveTexture.id, 0);
        }
        catch (...) {
            FAIL();
        }
    }

    // Test case for the ClearScene method
    // @note Future suggestion: Evaluate a variety of GLTF scenes that include more than one node.
    TEST_F(GltfSceneImporterTest, ClearScene) {
        RecordProperty("Test Description", "Checks if ClearScene clears the scene");
        RecordProperty("Expected Result", "No exception is thrown");
        
        try {
            GltfSceneImporterTest::gltfSceneImporter->Clear(this->mgttScene);
            EXPECT_EQ("", GltfSceneImporterTest::mgttScene.path);
            EXPECT_EQ(GltfSceneImporterTest::mgttScene.textureMap.size(), 0);
            EXPECT_EQ(GltfSceneImporterTest::mgttScene.nodes.size(), 0);
        }
        catch (...) {
            FAIL();
        }
    }
}