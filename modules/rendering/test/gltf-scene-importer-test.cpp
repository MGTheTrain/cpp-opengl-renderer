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
    
    TEST_F(GltfSceneImporterTest, LoadInvalidGltfFile) {
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

    TEST_F(GltfSceneImporterTest, LoadValidGltfFile) {
        try {
            std::pair<std::string, std::string> shaderPathes = { "assets/shader/core/pbr.vert", "assets/shader/core/pbr.frag" };
            GltfSceneImporterTest::mgttScene.shader.Compile(shaderPathes);
            GltfSceneImporterTest::gltfSceneImporter->Load(GltfSceneImporterTest::mgttScene, "assets/scenes/water-bottle/WaterBottle.gltf");
            EXPECT_EQ("assets/scenes/water-bottle/WaterBottle.gltf", GltfSceneImporterTest::mgttScene.path);
        }
        catch (...) {
            FAIL();
        }
    }

    TEST_F(GltfSceneImporterTest, ClearScene) {
        try {
            GltfSceneImporterTest::gltfSceneImporter->Clear(this->mgttScene);
        }
        catch (...) {
            FAIL();
        }
    }
}