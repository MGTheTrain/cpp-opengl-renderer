#ifdef MGTT_RENDERING_TEST
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <gtest/gtest.h>
#include <opengl-shader.h>

#include <glm/glm.hpp>

namespace Mgtt::Rendering::Test {
class OpenGlShaderTest : public ::testing::Test {
 public:
  static GLFWwindow* window;
  static std::unique_ptr<Mgtt::Rendering::OpenGlShader> openGlShader;

 protected:
  void SetUp() override {
    try {
      if (!glfwInit()) {
        throw std::runtime_error("GLFW ERROR: Failed to initialize GLFW");
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

      OpenGlShaderTest::window =
          glfwCreateWindow(800, 600, "test-window", nullptr, nullptr);
      if (!OpenGlShaderTest::window) {
        glfwTerminate();
        throw std::runtime_error("GLFW ERROR: Failed to create GLFW window");
        return;
      }
      glfwMakeContextCurrent(OpenGlShaderTest::window);
      if (glewInit() != GLEW_OK) {
        throw std::runtime_error("GLEW ERROR: Glew could not be initialized");
      }
    } catch (const std::runtime_error& ex) {
      std::cerr << ex.what();
    }
  }
  void TearDown() override {
    if (OpenGlShaderTest::window) {
      glfwDestroyWindow(OpenGlShaderTest::window);
      glfwTerminate();
    }
  }
};

GLFWwindow* OpenGlShaderTest::window = nullptr;
std::unique_ptr<Mgtt::Rendering::OpenGlShader> OpenGlShaderTest::openGlShader =
    std::make_unique<Mgtt::Rendering::OpenGlShader>();

// Test case for the Compile method
TEST_F(OpenGlShaderTest, Compile) {
  RecordProperty("Test Description",
                 "Checks if the Compile method compiles the shader program");
  RecordProperty("Expected Result", "No exception is thrown.");

  try {
    std::pair<std::string, std::string> shaderPathes = {
        "assets/shader/core/coordinate.vert",
        "assets/shader/core/coordinate.frag"};
    OpenGlShaderTest::openGlShader->Compile(shaderPathes);
  } catch (...) {
    FAIL() << "An exception has been thrown" << std::endl;
  }
}

// Test case for the Use method
TEST_F(OpenGlShaderTest, Use) {
  RecordProperty("Test Description",
                 "Checks if the Use method activates the shader program");
  RecordProperty("Expected Result", "No exception is thrown.");

  try {
    OpenGlShaderTest::openGlShader->Use();
    OpenGlShaderTest::openGlShader->SetInt("textureMap", 0);
    OpenGlShaderTest::openGlShader->SetMat4("mvp", glm::mat4(1.0f));
  } catch (...) {
    FAIL() << "An exception has been thrown" << std::endl;
  }
}

// Test case for the Clear method
TEST_F(OpenGlShaderTest, Clear) {
  RecordProperty("Test Description",
                 "Checks if the Clear method deletes the shader program");
  RecordProperty("Expected Result", "No exception is thrown.");

  try {
    OpenGlShaderTest::openGlShader->Clear();
    EXPECT_EQ(openGlShader->GetProgramId(), 0)
        << "GetProgramId should return 0 after Clear.";
  } catch (...) {
    FAIL() << "An exception has been thrown" << std::endl;
  }
}
}  // namespace Mgtt::Rendering::Test
#endif