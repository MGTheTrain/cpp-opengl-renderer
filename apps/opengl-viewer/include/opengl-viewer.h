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
#include <opengl-shader.h>
#include <texture-manager.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <string_view>

namespace Mgtt::Apps {

// Texture slots
enum class TextureSlot : int {
  BaseColor = 0,
  MetallicRoughness = 1,
  Normal = 2,
  Emissive = 3,
  Occlusion = 4,
  EnvMap = 7,
  IrradianceMap = 8,
  BrdfLut = 9,
};

// Cached uniform locations
struct PbrUniforms {
  GLint model{-1};
  GLint mvp{-1};
  GLint matrix{-1};
  GLint lightPosition{-1};
  GLint cameraPosition{-1};
  GLint scaleIblAmbient{-1};
  GLint samplerEnvMap{-1};
  GLint samplerIrradianceMap{-1};
  GLint samplerBrdfLut{-1};
  GLint baseColorTextureSet{-1};
  GLint physicalDescriptorTextureSet{-1};
  GLint normalTextureSet{-1};
  GLint emissiveTextureSet{-1};
  GLint occlusionTextureSet{-1};
  GLint baseColorMap{-1};
  GLint physicalDescriptorMap{-1};
  GLint normalMap{-1};
  GLint emissiveMap{-1};
  GLint occlusionMap{-1};
  GLint baseColorFactor{-1};
  GLint emissiveFactor{-1};
  GLint occlusionFactor{-1};
  GLint metallicFactor{-1};
  GLint roughnessFactor{-1};
  GLint alphaMaskSet{-1};
  GLint alphaMaskCutoff{-1};

  void Cache(uint32_t programId) noexcept;
};

// Transform state (plain data, stack allocated)
struct ViewMatrices {
  glm::mat4 model{1.0f};
  glm::mat4 view{1.0f};
  glm::mat4 projection{1.0f};
};

struct TransformVectors {
  glm::vec3 translation{0.0f};
  glm::vec3 rotation{0.0f};
  glm::vec3 scale{1.0f};
};

// Viewer
class OpenGlViewer {
 public:
  OpenGlViewer();
  ~OpenGlViewer();

  OpenGlViewer(const OpenGlViewer&) = delete;
  OpenGlViewer& operator=(const OpenGlViewer&) = delete;
  OpenGlViewer(OpenGlViewer&&) = delete;
  OpenGlViewer& operator=(OpenGlViewer&&) = delete;

  void Run();

 private:
  // Lifecycle
  void InitGl();
  void InitImGui();
  void LoadDefaultScene();
  void LoadDefaultIbl();

  // Per-frame pipeline
  void RenderFrame();
  void UpdateMatrices();
  void RenderScene();
  void RenderEnvMap();
  void RenderUi();
  void EndFrame();

  // Scene traversal
  void TraverseNode(const std::shared_ptr<Mgtt::Rendering::Node>& node) const;
  void RenderMesh(const std::shared_ptr<Mgtt::Rendering::Node>& node) const;
  void BindMeshTextures(const Mgtt::Rendering::PbrMaterial& mat) const;

  // ImGui panels
  void PanelScene();
  void PanelTransform();
  void PanelLight();

  // Helpers
  void ReloadScene(std::string_view path);
  static void FramebufferSizeCallback(GLFWwindow*, int32_t w, int32_t h);

  // Platform constants
  struct Platform {
    static std::pair<std::string_view, std::string_view>
    PbrShaderPaths() noexcept;
    static std::pair<std::string_view, std::string_view>
    Eq2CubeMapPaths() noexcept;
    static std::pair<std::string_view, std::string_view>
    BrdfLutPaths() noexcept;
    static std::pair<std::string_view, std::string_view> EnvMapPaths() noexcept;
    static const char* ImGuiGlslVersion() noexcept;
  };

  // State
  std::unique_ptr<Mgtt::Window::GlfwWindow> window_;
  std::unique_ptr<Mgtt::Rendering::GltfSceneImporter> sceneImporter_;
  std::unique_ptr<Mgtt::Rendering::TextureManager> textureManager_;

  Mgtt::Rendering::Scene scene_;
  Mgtt::Rendering::RenderTexturesContainer ibl_;

  PbrUniforms uniforms_{};
  ViewMatrices matrices_{};
  TransformVectors transform_{};

  glm::vec3 cameraPos_{0.0f, 0.0f, -3.0f};
  float scaleIblAmbient_{1.0f};
  bool showEnvMap_{false};
  float windowW_{1000.0f};
  float windowH_{1000.0f};
};

}  // namespace Mgtt::Apps
