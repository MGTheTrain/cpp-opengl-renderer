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
#endif>
#include <glfw-window.h>
#include <mesh.h>
#include <opengl-shader.h>
#include <scene.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>
#include <vector>

// To prevent linking errors, refrain from relocating the #define
// STB_IMAGE_IMPLEMENTATION macro to this location. It is restricted to a single
// usage. Tinygltf internally utilizes functions from the stb_image header and
// requires macros to be set in order to use those. See:
// https://github.com/syoyo/tinygltf/blob/release/stb_image.h
#include <stb_image.h>

namespace Mgtt::Apps {
/**
 * @brief Represents glm matrices
 */
struct GlmMatrices {
  GlmMatrices() {
    this->model = glm::mat4(1.0f);
    this->view = glm::mat4(1.0f);
    this->projection = glm::mat4(1.0f);
    this->mvp = glm::mat4(1.0f);
  }
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 projection;
  glm::mat4 mvp;
};

/**
 * @brief The RotatingTexturedCube  class represents a simple OpenGL viewer.
 */
class RotatingTexturedCube {
 public:
  /**
   * @brief Constructs an RotatingTexturedCube  object.
   */
  RotatingTexturedCube();

  /**
   * @brief Destructs the RotatingTexturedCube  object.
   */
  ~RotatingTexturedCube();

  /**
   * @brief Renders the scene using OpenGL.
   */
  void Render();

  /**
   * @brief Process input for the GLFW window.
   */
  void ProcessInput();

 private:
  /**
   * @brief Callback function for framebuffer size changes.
   *
   * @param window A pointer to the GLFW window whose framebuffer size has
   * changed.
   * @param width  The new width of the framebuffer.
   * @param height The new height of the framebuffer.
   */
  static void FramebufferSizeCallback(GLFWwindow* window, int32_t width,
                                      int32_t height);

  std::unique_ptr<Mgtt::Window::GlfwWindow> glfwWindow;
  std::vector<Mgtt::Rendering::OpenGlShader> openGlShaders;
  std::unique_ptr<GlmMatrices> glmMatrices;
  Mgtt::Rendering::Mesh mesh;
  float windowWidth;
  float windowHeight;
};

}  // namespace Mgtt::Apps
