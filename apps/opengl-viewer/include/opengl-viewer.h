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
#include <nfd.h>
#endif>
#include <glfw-window.h>
#include <gltf-scene-importer.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <opengl-shader.h>
#include <texture-manager.h>

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
 * @brief Represents glm vectors
 */
struct GlmVectors {
  GlmVectors() {
    this->translation = glm::vec3(0.0f);
    this->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    this->scale = glm::vec3(1.0f);
  }
  glm::vec3 translation;
  glm::vec3 rotation;
  glm::vec3 scale;
};

/**
 * @brief The OpenGlViewer class represents a simple OpenGL viewer.
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
   */
  void Render();

 private:
  std::unique_ptr<GlmMatrices> glmMatrices;
  std::unique_ptr<GlmVectors> glmVectors;
  Mgtt::Rendering::Scene mgttScene;
  Mgtt::Rendering::RenderTexturesContainer renderTextureContainer;
  std::unique_ptr<Mgtt::Rendering::GltfSceneImporter> gltfSceneImporter;
  std::unique_ptr<Mgtt::Rendering::TextureManager> textureManager;
  std::unique_ptr<Mgtt::Window::GlfwWindow> glfwWindow;
  glm::vec3 cameraPosition;
  float scaleIblAmbient;
  bool showEnvMap;
  float windowWidth;
  float windowHeight;

  /**
   * @brief Iterates recursively over all nodes in the scene
   *
   * @param node A shared pointer to the node.
   **/
  void TraverseSceneNode(std::shared_ptr<Mgtt::Rendering::Node> node);

  /**
   * @brief Renders the mesh using the specified rendering technique.
   *
   * @param node A shared pointer to the node.
   */
  void RenderMesh(std::shared_ptr<Mgtt::Rendering::Node> node);

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

  /**
   * @brief Initializes ImGui for the OpenGL viewer.
   */
  void InitializeImGui();

  /**
   * @brief Update settings trough ImGui widgets.
   */
  void UpdateSettings();

  /**
   * @brief Initializes ImGui for the OpenGL viewer.
   */
  void ClearImGui();
};

}  // namespace Mgtt::Apps
