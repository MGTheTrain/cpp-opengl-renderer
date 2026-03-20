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

#pragma once

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <glfw-window.h>
#include <mesh.h>
#include <opengl-shader.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <vector>

namespace Mgtt::Apps {

/**
 * @brief GLM transformation matrices for a single draw call.
 */
struct GlmMatrices {
  GlmMatrices() : model(1.0f), view(1.0f), projection(1.0f), mvp(1.0f) {}

  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 projection;
  glm::mat4 mvp;
};

/**
 * @brief Renders a rotating textured cube using OpenGL.
 */
class RotatingTexturedCube {
 public:
  RotatingTexturedCube();
  ~RotatingTexturedCube();

  RotatingTexturedCube(const RotatingTexturedCube&) = delete;
  RotatingTexturedCube& operator=(const RotatingTexturedCube&) = delete;
  RotatingTexturedCube(RotatingTexturedCube&&) = delete;
  RotatingTexturedCube& operator=(RotatingTexturedCube&&) = delete;

  void Render();
  void ProcessInput();

 private:
  static void FramebufferSizeCallback(GLFWwindow* window, int32_t width,
                                      int32_t height);

  std::unique_ptr<Mgtt::Window::GlfwWindow> glfwWindow;
  std::vector<Mgtt::Rendering::OpenGlShader> openGlShaders;
  std::unique_ptr<GlmMatrices> glmMatrices;
  Mgtt::Rendering::Mesh mesh;
  float windowWidth{1000.0f};
  float windowHeight{1000.0f};
};

}  // namespace Mgtt::Apps
