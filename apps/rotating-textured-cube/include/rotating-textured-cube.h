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

#if defined(__EMSCRIPTEN__)
#include <GLES3/gl3.h>
#else
#if defined(__ANDROID__)
#include <GLES2/gl2.h>
#else
#include <GL/glew.h>
#endif
#endif
#ifndef __ANDROID__
#include <glfw-window.h>
#else
#include <android/log.h>
#include <jni.h>
#endif
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
 *
 * This class provides basic functionality for initializing an OpenGL context,
 * rendering scenes, and clearing the rendering buffer.
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
   * @brief Initializes objects required for scene rendering using OpenGL.
   *
   * This method is responsible for initializing objects required for scene
   * rendering using OpenGL.
   */
  void Init();

  /**
   * @brief Renders the scene using OpenGL.
   *
   * This method is responsible for rendering the contents of the scene using
   * OpenGL.
   */
  void Render();

  /**
   * @brief Process input for the GLFW window.
   *
   * This method processes input for the specified GLFW window. It can handle
   * keyboard and mouse input events and update the application state
   * accordingly.
   */
  void ProcessInput();

#ifdef __ANDROID__
  /**
   * @brief Updates the OpenGL viewport dimensions.
   *
   * This function is responsible for updating the dimensions of the OpenGL
   * viewport to match the specified width and height.
   *
   * @param width The new width of the viewport.
   * @param height The new height of the viewport.
   */
  void UpdateOpenGlViewPort(int width, int height);
#endif

 private:
#ifndef __ANDROID__
  /**
   * @brief Callback function for framebuffer size changes.
   *
   * This static callback function is invoked when the framebuffer size of the
   * GLFW window changes. It is typically registered using
   * `glfwSetFramebufferSizeCallback`. The function updates the viewport size
   * based on the new width and height.
   *
   * @param window A pointer to the GLFW window whose framebuffer size has
   * changed.
   * @param width  The new width of the framebuffer.
   * @param height The new height of the framebuffer.
   */
  static void FramebufferSizeCallback(GLFWwindow* window, int32_t width,
                                      int32_t height);

  std::unique_ptr<Mgtt::Window::GlfwWindow> glfwWindow;
#else
  float timeInSeconds;
#endif

  std::vector<Mgtt::Rendering::OpenGlShader> openGlShaders;
  std::unique_ptr<GlmMatrices> glmMatrices;
  Mgtt::Rendering::Mesh mesh;
  float windowWidth;
  float windowHeight;
};

}  // namespace Mgtt::Apps
