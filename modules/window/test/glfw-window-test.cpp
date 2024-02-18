#include <gtest/gtest.h>
#include <memory>
#include <glfw-window.h>


namespace Mgtt::Window::Test {
    class GlfwWindowTest : public ::testing::Test {
    public:
        static std::unique_ptr<Mgtt::Window::GlfwWindow> glfwWindow;
    protected:
        void SetUp() override {}
        void TearDown() override {
            this->glfwWindow->~GlfwWindow();
        }

        static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
        static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
        static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    };

    std::unique_ptr<Mgtt::Window::GlfwWindow> GlfwWindowTest::glfwWindow =
        std::make_unique<Mgtt::Window::GlfwWindow>("test-window", 800, 600);

    void GlfwWindowTest::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {}
    void GlfwWindowTest::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {}
    void GlfwWindowTest::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {}

    TEST_F(GlfwWindowTest, SetFramebufferSizeCallback) {
        RecordProperty("Test Description", "Checks if SetFramebufferSizeCallback method works");
        RecordProperty("Expected Result", "No exception is thrown.");

        try {
            GlfwWindowTest::glfwWindow->SetFramebufferSizeCallback(GlfwWindowTest::FramebufferSizeCallback);
        }
        catch (...) {
            FAIL() << "An exception has been thrown" << std::endl;
        }
    }

    // Test case for the SetScrollCallback method
    TEST_F(GlfwWindowTest, SetScrollCallback) {
        RecordProperty("Test Description", "Checks if SetScrollCallback method works");
        RecordProperty("Expected Result", "No exception is thrown.");

        try {
            GlfwWindowTest::glfwWindow->SetScrollCallback(GlfwWindowTest::ScrollCallback);
        }
        catch (...) {
            FAIL() << "An exception has been thrown" << std::endl;
        }
    }

    // Test case for the SetKeyCallback method
    TEST_F(GlfwWindowTest, SetKeyCallback) {
        RecordProperty("Test Description", "Checks if SetKeyCallback method works");
        RecordProperty("Expected Result", "No exception is thrown.");

        try {
            GlfwWindowTest::glfwWindow->SetKeyCallback(GlfwWindowTest::KeyCallback);
        }
        catch (...) {
            FAIL() << "An exception has been thrown" << std::endl;
        }
    }

    // Test case for the SwapBuffersAndPollEvents method
    TEST_F(GlfwWindowTest, SwapBuffersAndPollEvents) {
        RecordProperty("Test Description", "Checks if SwapBuffersAndPollEvents method works");
        RecordProperty("Expected Result", "No exception is thrown.");

        try {
            GlfwWindowTest::glfwWindow->SwapBuffersAndPollEvents();
        }
        catch (...) {
            FAIL() << "An exception has been thrown" << std::endl;
        }
    }

    // Test case for the WindowShouldClose method
    TEST_F(GlfwWindowTest, WindowShouldClose) {
        RecordProperty("Test Description", "Checks if WindowShouldClose method works");
        RecordProperty("Expected Result", "No exception is thrown.");

        try {
            GlfwWindowTest::glfwWindow->SwapBuffersAndPollEvents();
        }
        catch (...) {
            FAIL() << "An exception has been thrown" << std::endl;
        }
    }
}