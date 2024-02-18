#ifdef MGTT_RENDERING_TEST
#include <gtest/gtest.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <opengl-shader.h>

namespace Mgtt::Rendering::Test {
    class OpenGlShaderTest : public ::testing::Test {
    public:
        static std::unique_ptr<Mgtt::Rendering::OpenGlShader> openGlShader;
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

    std::unique_ptr<Mgtt::Rendering::OpenGlShader> OpenGlShaderTest::openGlShader = std::make_unique<Mgtt::Rendering::OpenGlShader>();

    // Test case for the Compile method
    TEST_F(OpenGlShaderTest, Compile) {
        RecordProperty("Test Description", "Checks if the Compile method compiles the shader program");
        RecordProperty("Expected Result", "No exception is thrown.");

        try {
            std::pair<std::string, std::string> shaderPathes = { "assets/shader/core/coordinate.vert", "assets/shader/core/coordinate.frag" };
            OpenGlShaderTest::openGlShader->Compile(shaderPathes);
        }
        catch (...) {
            FAIL() << "An exception has been thrown" << std::endl;
        }
    }

    // Test case for the Use method
    TEST_F(OpenGlShaderTest, Use) {
        RecordProperty("Test Description", "Checks if the Use method activates the shader program");
        RecordProperty("Expected Result", "No exception is thrown.");

        try {
            OpenGlShaderTest::openGlShader->Use();
            OpenGlShaderTest::openGlShader->SetInt("textureMap", 0);
            OpenGlShaderTest::openGlShader->SetMat4("mvp", glm::mat4(1.0f));
        }
        catch (...) {
            FAIL() << "An exception has been thrown" << std::endl;
        }
    }

    // Test case for the Clear method
    TEST_F(OpenGlShaderTest, Clear) {
        RecordProperty("Test Description", "Checks if the Clear method deletes the shader program");
        RecordProperty("Expected Result", "No exception is thrown.");

        try {
            OpenGlShaderTest::openGlShader->Clear();
            EXPECT_EQ(openGlShader->GetProgramId(), 0) << "GetProgramId should return 0 after Clear.";
        }
        catch (...) {
            FAIL() << "An exception has been thrown" << std::endl;
        }
    }
}
#endif