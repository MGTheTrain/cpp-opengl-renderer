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

#include <GL/glew.h>
#include <glfw-window.h>
#include <gltf-scene-importer.h>
#include <opengl-shader.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

// To prevent linking errors, refrain from relocating the #define
// STB_IMAGE_IMPLEMENTATION macro to this location. It is restricted to a single
// usage. Tinygltf internally utilizes functions from the stb_image header and
// requires macros to be set in order to use those. See:
// https://github.com/syoyo/tinygltf/blob/release/stb_image.h
// #include <stb_image.h>

namespace Mgtt::Apps {
/**
 * @brief Represents glm matrices
 */
struct GlmMatrices {
  GlmMatrices() {
    this->model = glm::mat4(1.0f);
    this->view = glm::mat4(1.0f);
    this->projection = glm::mat4(1.0f);
  }
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 projection;
};

/**
 * @brief The OpenGlViewer class represents a simple OpenGL viewer.
 *
 * This class provides basic functionality for initializing an OpenGL context,
 * rendering scenes, and clearing the rendering buffer.
 */
class OpenGlViewer {
 public:
  /**
   * @brief Constructs an OpenGlViewer object.
   */
  OpenGlViewer();

  /**
   * @brief Destructs the OpenGlViewer object.
   */
  ~OpenGlViewer();

  /**
   * @brief Clears OpenGl and RAM allocated resources
   *
   */
  void Clear();

  /**
   * @brief Renders the scene using OpenGL.
   *
   * This method is responsible for rendering the contents of the scene using
   * OpenGL.
   */
  void Render();

 private:
  std::unique_ptr<GlmMatrices> glmMatrices;
  Mgtt::Rendering::Scene mgttScene;
  Mgtt::Rendering::RenderTexturesContainer renderTextureContainer;
  std::unique_ptr<Mgtt::Rendering::GltfSceneImporter> gltfSceneImporter;
  std::unique_ptr<Mgtt::Rendering::TextureManager> textureManager;
  std::unique_ptr<Mgtt::Window::GlfwWindow> glfwWindow;
  glm::vec3 cameraPosition;

  /**
   * @brief Iterates recursively over all nodes in the scene
   *
   * This function is responsible for iteraing recursively over all nodes in the
   *scene
   *
   * @param node A shared pointer to the 3D node to be rendered.
   **/
  void TraverseSceneNode(std::shared_ptr<Mgtt::Rendering::Node> node);

  /**
   * @brief Renders the mesh using the specified rendering technique.
   *
   * This function is responsible for rendering the mesh using the current
   * rendering technique and associated settings. It should be called within the
   * rendering loop.
   */
  void RenderMesh(std::shared_ptr<Mgtt::Rendering::Node> node);

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
  static void FramebufferSizeCallback(GLFWwindow* window, int width,
                                      int height);
};

}  // namespace Mgtt::Apps
