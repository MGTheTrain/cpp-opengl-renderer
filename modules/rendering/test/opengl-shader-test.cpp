#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <open-gl-shader.h>

class OpenGlShaderTest : public ::testing::Test {
protected:
    Mgtt::Rendering::OpenGlShader* openGlShader;

    void SetUp() override {
        openGlShader = new Mgtt::Rendering::OpenGlShader("vsPath", "fsPath");
    }

    void TearDown() override {
        delete openGlShader;
    }
};

// Test case for the Compile method
TEST_F(OpenGlShaderTest, Compile) {
    EXPECT_CALL(*glMock, glCreateShader(testing::_)).WillOnce(testing::Return(1));
}

// Test case for the GetProgramId method
TEST_F(OpenGlShaderTest, GetProgramId) {
    unsigned int programId = openGlShader->GetProgramId();
}

// Test case for the Use method
TEST_F(OpenGlShaderTest, Use) {
    openGlShader->Use();
}

// Test cases for various Set* methods
TEST_F(OpenGlShaderTest, SetBool) {
    // Similar structure to the previous test
}

TEST_F(OpenGlShaderTest, SetInt) {
    // Similar structure to the previous test
}

TEST_F(OpenGlShaderTest, SetFloat) {
    // Similar structure to the previous test
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
