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
#endif
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
#include <memory>
#include <vector>

namespace Mgtt::Apps {

struct GlmMatrices {
  GlmMatrices() : model(1.0f), view(1.0f), projection(1.0f) {}
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 projection;
};

struct GlmVectors {
  GlmVectors() : translation(0.0f), rotation(0.0f), scale(1.0f) {}
  glm::vec3 translation;
  glm::vec3 rotation;
  glm::vec3 scale;
};

class OpenGlViewer {
 public:
  OpenGlViewer();
  ~OpenGlViewer();

  void Clear();
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

  void TraverseSceneNode(
      const std::shared_ptr<Mgtt::Rendering::Node>& node) const;
  void RenderMesh(const std::shared_ptr<Mgtt::Rendering::Node>& node) const;

  static void FramebufferSizeCallback(GLFWwindow* window, int32_t width,
                                      int32_t height);
  void InitializeImGui();
  void UpdateSettings();
  void ClearImGui();
};

}  // namespace Mgtt::Apps
