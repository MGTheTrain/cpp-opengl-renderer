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

#ifdef MGTT_OPENGL_VIEWER
#include <opengl-viewer.h>

#include <iostream>
#include <stdexcept>
#include <string>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
EM_JS(int, CanvasGetWidth, (), { return Module.canvas.width; });
EM_JS(int, CanvasGetHeight, (), { return Module.canvas.height; });
EM_JS(void, ResizeCanvas, (), { resizeCanvas(); });
#endif

namespace Mgtt::Apps {

// PbrUniforms::Cache
void PbrUniforms::Cache(uint32_t id) noexcept {
  auto loc = [id](const char* n) { return glGetUniformLocation(id, n); };
  model = loc("model");
  mvp = loc("mvp");
  matrix = loc("matrix");
  lightPosition = loc("lightPosition");
  cameraPosition = loc("cameraPosition");
  scaleIblAmbient = loc("scaleIblAmbient");
  samplerEnvMap = loc("samplerEnvMap");
  samplerIrradianceMap = loc("samplerIrradianceMap");
  samplerBrdfLut = loc("samplerBrdfLut");
  baseColorTextureSet = loc("baseColorTextureSet");
  physicalDescriptorTextureSet = loc("physicalDescriptorTextureSet");
  normalTextureSet = loc("normalTextureSet");
  emissiveTextureSet = loc("emissiveTextureSet");
  occlusionTextureSet = loc("occlusionTextureSet");
  baseColorMap = loc("baseColorMap");
  physicalDescriptorMap = loc("physicalDescriptorMap");
  normalMap = loc("normalMap");
  emissiveMap = loc("emissiveMap");
  occlusionMap = loc("occlusionMap");
  baseColorFactor = loc("baseColorFactor");
  emissiveFactor = loc("emissiveFactor");
  occlusionFactor = loc("occlusionFactor");
  metallicFactor = loc("metallicFactor");
  roughnessFactor = loc("roughnessFactor");
  alphaMaskSet = loc("alphaMaskSet");
  alphaMaskCutoff = loc("alphaMaskCutoff");
}

// Platform constants
std::pair<std::string_view, std::string_view>
OpenGlViewer::Platform::PbrShaderPaths() noexcept {
#ifdef __EMSCRIPTEN__
  return {"assets/shader/es/pbr.vert", "assets/shader/es/pbr.frag"};
#else
  return {"assets/shader/core/pbr.vert", "assets/shader/core/pbr.frag"};
#endif
}

std::pair<std::string_view, std::string_view>
OpenGlViewer::Platform::Eq2CubeMapPaths() noexcept {
#ifdef __EMSCRIPTEN__
  return {"assets/shader/es/eq2CubeMap.vert",
          "assets/shader/es/eq2CubeMap.frag"};
#else
  return {"assets/shader/core/eq2CubeMap.vert",
          "assets/shader/core/eq2CubeMap.frag"};
#endif
}

std::pair<std::string_view, std::string_view>
OpenGlViewer::Platform::BrdfLutPaths() noexcept {
#ifdef __EMSCRIPTEN__
  return {"assets/shader/es/genBrdf.vert", "assets/shader/es/genBrdf.frag"};
#else
  return {"assets/shader/core/genBrdf.vert", "assets/shader/core/genBrdf.frag"};
#endif
}

std::pair<std::string_view, std::string_view>
OpenGlViewer::Platform::EnvMapPaths() noexcept {
#ifdef __EMSCRIPTEN__
  return {"assets/shader/es/envMap.vert", "assets/shader/es/envMap.frag"};
#else
  return {"assets/shader/core/envMap.vert", "assets/shader/core/envMap.frag"};
#endif
}

const char* OpenGlViewer::Platform::ImGuiGlslVersion() noexcept {
#ifdef __EMSCRIPTEN__
  return "#version 300 es";
#else
  return "#version 330 core";
#endif
}

// Construction / destruction
OpenGlViewer::OpenGlViewer()
    : sceneImporter_(std::make_unique<Mgtt::Rendering::GltfSceneImporter>()),
      sceneUploader_(std::make_unique<Mgtt::Rendering::SceneUploader>()),
      textureManager_(std::make_unique<Mgtt::Rendering::TextureManager>()) {
  glfwContext_ = std::make_unique<Mgtt::Window::GlfwContext>();
  window_ = std::make_unique<Mgtt::Window::GlfwWindow>("opengl-viewer",
                                                       windowW_, windowH_);
  window_->SetFramebufferSizeCallback(FramebufferSizeCallback);

  InitGl();
  InitImGui();
  LoadDefaultScene();
  LoadDefaultIbl();

  int fbW = 0;
  int fbH = 0;
  glfwGetFramebufferSize(window_->GetWindow(), &fbW, &fbH);
  glViewport(0, 0, fbW, fbH);
}

OpenGlViewer::~OpenGlViewer() {
  sceneImporter_->Clear(scene_);
  textureManager_->Clear(ibl_);
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

// Initialisation
void OpenGlViewer::InitGl() {
#ifndef __EMSCRIPTEN__
  if (glewInit() != GLEW_OK) {
    throw std::runtime_error("GLEW init failed");
  }
#endif
  glEnable(GL_DEPTH_TEST);

  if (auto r = scene_.shader.Compile(Platform::PbrShaderPaths()); r.err()) {
    throw std::runtime_error("PBR shader: " + r.error());
  }
  uniforms_.Cache(scene_.shader.GetProgramId());

  ibl_ = Mgtt::Rendering::RenderTexturesContainer(Platform::Eq2CubeMapPaths(),
                                                  Platform::BrdfLutPaths(),
                                                  Platform::EnvMapPaths());
}

void OpenGlViewer::InitImGui() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(window_->GetWindow(), true);
  ImGui_ImplOpenGL3_Init(Platform::ImGuiGlslVersion());
#ifdef __EMSCRIPTEN__
  ResizeCanvas();
#endif
}

void OpenGlViewer::LoadDefaultScene() {
  if (auto r = sceneImporter_->Load(
          scene_, "assets/scenes/water-bottle/WaterBottle.gltf");
      r.err()) {
    throw std::runtime_error("Scene load: " + r.error());
  }
  if (auto r = sceneUploader_->Upload(scene_); r.err()) {
    throw std::runtime_error("Scene upload: " + r.error());
  }
}

void OpenGlViewer::LoadDefaultIbl() {
  if (auto r = textureManager_->LoadFromHdr(ibl_, "assets/texture/surgery.jpg");
      r.err()) {
    throw std::runtime_error("HDR load: " + r.error());
  }
  if (auto r = textureManager_->LoadBrdfLut(ibl_); r.err()) {
    throw std::runtime_error("BRDF LUT: " + r.error());
  }
}

// Run loop
void OpenGlViewer::Run() {
#ifndef __EMSCRIPTEN__
  while (!window_->WindowShouldClose()) {
    RenderFrame();
  }
#else
  // Emscripten: single-frame entry; caller sets up the loop.
  RenderFrame();
#endif
}

void OpenGlViewer::RenderFrame() {
#ifdef __EMSCRIPTEN__
  windowW_ = static_cast<float>(CanvasGetWidth());
  windowH_ = static_cast<float>(CanvasGetHeight());
  window_->SetWindowSize(windowW_, windowH_);
#endif

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  UpdateMatrices();

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  auto [scrW, scrH] = window_->GetWindowSize();
  ImGui::SetNextWindowSize(
      ImVec2(static_cast<float>(scrW) * 0.3f, static_cast<float>(scrH)));
  ImGui::SetNextWindowPos(ImVec2(static_cast<float>(scrW) * 0.7f, 0.0f));

  RenderScene();
  RenderEnvMap();
  RenderUi();
  EndFrame();
}

// Per-frame pipeline steps
void OpenGlViewer::UpdateMatrices() {
  auto [w, h] = window_->GetWindowSize();

  matrices_.view = glm::translate(glm::mat4(1.0f), cameraPos_);
  matrices_.projection = glm::perspective(
      glm::radians(45.0f), static_cast<float>(w) / static_cast<float>(h), 0.1f,
      1000.0f);

  const glm::vec3 kOffset = -scene_.aabb.center + transform_.translation;

  matrices_.model = glm::mat4(1.0f);
  matrices_.model =
      glm::scale(matrices_.model, glm::vec3(1.0f / scene_.aabb.scale));
  matrices_.model = glm::scale(matrices_.model, transform_.scale);
  matrices_.model = glm::translate(matrices_.model, kOffset);
  matrices_.model = glm::rotate(
      matrices_.model, glm::radians(transform_.rotation.x), glm::vec3(1, 0, 0));
  matrices_.model = glm::rotate(
      matrices_.model, glm::radians(transform_.rotation.y), glm::vec3(0, 1, 0));
  matrices_.model = glm::rotate(
      matrices_.model, glm::radians(transform_.rotation.z), glm::vec3(0, 0, 1));
  matrices_.model = glm::translate(matrices_.model, -kOffset);
  matrices_.model = glm::translate(matrices_.model, kOffset);

  scene_.mvp = matrices_.projection * matrices_.view * matrices_.model;
}

void OpenGlViewer::RenderScene() {
  if (auto result = scene_.shader.Use(); result.err()) {
    std::cerr << "Shader not ready: " << result.error() << "\n";
    return;
  }

  glUniformMatrix4fv(uniforms_.model, 1, GL_FALSE, &matrices_.model[0][0]);
  glUniformMatrix4fv(uniforms_.mvp, 1, GL_FALSE, &scene_.mvp[0][0]);
  glUniform3fv(uniforms_.lightPosition, 1, &cameraPos_[0]);
  glUniform3fv(uniforms_.cameraPosition, 1, &cameraPos_[0]);
  glUniform1f(uniforms_.scaleIblAmbient, scaleIblAmbient_);

  glUniform1i(uniforms_.samplerEnvMap, static_cast<int>(TextureSlot::EnvMap));
  glUniform1i(uniforms_.samplerIrradianceMap,
              static_cast<int>(TextureSlot::IrradianceMap));
  glUniform1i(uniforms_.samplerBrdfLut, static_cast<int>(TextureSlot::BrdfLut));

  glActiveTexture(GL_TEXTURE0 + static_cast<int>(TextureSlot::EnvMap));
  glBindTexture(GL_TEXTURE_CUBE_MAP, ibl_.cubeMapTextureId);
  glActiveTexture(GL_TEXTURE0 + static_cast<int>(TextureSlot::IrradianceMap));
  glBindTexture(GL_TEXTURE_CUBE_MAP, ibl_.cubeMapTextureId);
  glActiveTexture(GL_TEXTURE0 + static_cast<int>(TextureSlot::BrdfLut));
  glBindTexture(GL_TEXTURE_2D, ibl_.brdfLutTextureId);

  for (const auto& node : scene_.nodes) {
    TraverseNode(node);
  }
}

void OpenGlViewer::RenderEnvMap() {
  if (!showEnvMap_) {
    return;
  }

  glDepthFunc(GL_LEQUAL);
  if (auto result = ibl_.envMapShader.Use(); result.err()) {
    std::cerr << "Shader not ready: " << result.error() << "\n";
    return;
  }
  glUniformMatrix4fv(
      glGetUniformLocation(ibl_.envMapShader.GetProgramId(), "projection"), 1,
      GL_FALSE, &matrices_.projection[0][0]);
  glUniformMatrix4fv(
      glGetUniformLocation(ibl_.envMapShader.GetProgramId(), "view"), 1,
      GL_FALSE, &matrices_.view[0][0]);
  glUniform1i(glGetUniformLocation(ibl_.envMapShader.GetProgramId(), "envMap"),
              0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, ibl_.cubeMapTextureId);
  glBindVertexArray(ibl_.cubeVao);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
  glDepthFunc(GL_LESS);
}

void OpenGlViewer::RenderUi() {
  ImGui::Begin("Settings");
  if (ImGui::BeginTabBar("Settings")) {
    PanelScene();
    PanelTransform();
    PanelLight();
    ImGui::EndTabBar();
  }
  ImGui::End();
  ImGui::Render();
}

void OpenGlViewer::EndFrame() {
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  window_->SwapBuffersAndPollEvents();
}

// Scene traversal
void OpenGlViewer::TraverseNode(
    const std::shared_ptr<Mgtt::Rendering::Node>& node) const {
  RenderMesh(node);
  for (const auto& child : node->children) {
    TraverseNode(child);
  }
}

void OpenGlViewer::RenderMesh(
    const std::shared_ptr<Mgtt::Rendering::Node>& node) const {
  if (node->mesh == nullptr) {
    return;
  }

  glUniformMatrix4fv(uniforms_.matrix, 1, GL_FALSE, &node->mesh->matrix[0][0]);

  for (const auto& prim : node->mesh->meshPrimitives) {
    BindMeshTextures(prim.pbrMaterial);

    glUniform4fv(uniforms_.baseColorFactor, 1,
                 &prim.pbrMaterial.baseColorTexture.color[0]);
    glUniform3fv(uniforms_.emissiveFactor, 1,
                 &prim.pbrMaterial.emissiveTexture.color[0]);
    glUniform1f(uniforms_.occlusionFactor,
                prim.pbrMaterial.occlusionTexture.strength);
    glUniform1f(uniforms_.metallicFactor,
                prim.pbrMaterial.metallicRoughnessTexture.metallicFactor);
    glUniform1f(uniforms_.roughnessFactor,
                prim.pbrMaterial.metallicRoughnessTexture.roughnessFactor);
    glUniform1i(uniforms_.alphaMaskSet, 1);
    glUniform1f(uniforms_.alphaMaskCutoff, prim.pbrMaterial.alphaCutoff);

    glBindVertexArray(node->mesh->vao);
    glDrawElements(
        GL_TRIANGLES, static_cast<GLsizei>(prim.indexCount), GL_UNSIGNED_INT,
        // NOLINTNEXTLINE(performance-no-int-to-ptr)
        reinterpret_cast<const void*>(prim.firstIndex * sizeof(uint32_t)));
    glBindVertexArray(0);
  }
}

void OpenGlViewer::BindMeshTextures(
    const Mgtt::Rendering::PbrMaterial& mat) const {
  auto bind = [&](uint32_t texId, TextureSlot slot, GLint flagLoc,
                  GLint samplerLoc) {
    const bool kHas = texId > 0;
    glUniform1i(flagLoc, kHas ? 1 : 0);
    if (!kHas) {
      return;
    }
    glUniform1i(samplerLoc, static_cast<int>(slot));
    glActiveTexture(GL_TEXTURE0 + static_cast<int>(slot));
    glBindTexture(GL_TEXTURE_2D, texId);
  };

  bind(mat.baseColorTexture.id, TextureSlot::BaseColor,
       uniforms_.baseColorTextureSet, uniforms_.baseColorMap);
  bind(mat.metallicRoughnessTexture.id, TextureSlot::MetallicRoughness,
       uniforms_.physicalDescriptorTextureSet, uniforms_.physicalDescriptorMap);
  bind(mat.normalTexture.id, TextureSlot::Normal, uniforms_.normalTextureSet,
       uniforms_.normalMap);
  bind(mat.emissiveTexture.id, TextureSlot::Emissive,
       uniforms_.emissiveTextureSet, uniforms_.emissiveMap);
  bind(mat.occlusionTexture.id, TextureSlot::Occlusion,
       uniforms_.occlusionTextureSet, uniforms_.occlusionMap);
}

// ImGui panels
void OpenGlViewer::PanelScene() {
  if (!ImGui::BeginTabItem("Scene")) {
    return;
  }
#ifndef __EMSCRIPTEN__
  if (ImGui::Button("Select glTF scene")) {
    NFD_Init();
    nfdu8filteritem_t filters[1] = {{"3D Models", "gltf"}};
    nfdopendialogu8args_t args{};
    args.filterList = filters;
    args.filterCount = 1;
    nfdu8char_t* selectedPath = nullptr;
    if (NFD_OpenDialogU8_With(&selectedPath, &args) == NFD_OKAY) {
      ReloadScene(selectedPath);
      NFD_FreePathU8(selectedPath);
    } else if (const char* e = NFD_GetError(); e) {
      std::cerr << e << '\n';
    }
    NFD_Quit();
  }
#endif
  ImGui::EndTabItem();
}

void OpenGlViewer::PanelTransform() {
  if (!ImGui::BeginTabItem("Model space transformation")) {
    return;
  }
  ImGui::SliderFloat3("Translation",
                      reinterpret_cast<float*>(&transform_.translation),
                      -scene_.aabb.scale, scene_.aabb.scale);
  ImGui::Dummy(ImVec2(0, 5));
  ImGui::SliderFloat3(
      "Rotation", reinterpret_cast<float*>(&transform_.rotation), 0.0f, 360.0f);
  ImGui::Dummy(ImVec2(0, 5));
  ImGui::SliderFloat3("Scale", reinterpret_cast<float*>(&transform_.scale),
                      0.01f, 3.0f);
  ImGui::EndTabItem();
}

void OpenGlViewer::PanelLight() {
  if (!ImGui::BeginTabItem("Light")) {
    return;
  }
  ImGui::Text("Image based lighting");
  ImGui::SliderFloat("Scale ibl ambient", &scaleIblAmbient_, 0.0f, 2.0f);
  ImGui::Dummy(ImVec2(0, 5));
  ImGui::Checkbox("Show env map", &showEnvMap_);
  ImGui::EndTabItem();
}

// Helpers
void OpenGlViewer::ReloadScene(std::string_view path) {
  sceneImporter_->Clear(scene_);

  if (auto r = scene_.shader.Compile(Platform::PbrShaderPaths()); r.err()) {
    std::cerr << "Shader recompile: " << r.error() << '\n';
    return;
  }
  uniforms_.Cache(scene_.shader.GetProgramId());

  if (auto r = sceneImporter_->Load(scene_, path); r.err()) {
    std::cerr << "Scene load: " << r.error() << '\n';
    return;
  }
  if (auto r = sceneUploader_->Upload(scene_); r.err()) {
    std::cerr << "Scene upload: " << r.error() << '\n';
    return;
  }

  scaleIblAmbient_ = 1.0f;
  showEnvMap_ = false;
  transform_ = {};
}

void OpenGlViewer::FramebufferSizeCallback(GLFWwindow*, int32_t w, int32_t h) {
  glViewport(0, 0, w, h);
}

}  // namespace Mgtt::Apps

// Entry point
#ifdef __EMSCRIPTEN__
static Mgtt::Apps::OpenGlViewer* gViewer = nullptr;
void EmscriptenMainLoop() { gViewer->Run(); }
#endif

int main() {
  try {
#ifndef __EMSCRIPTEN__
    Mgtt::Apps::OpenGlViewer viewer;
    viewer.Run();
#else
    gViewer = new Mgtt::Apps::OpenGlViewer();
    emscripten_set_main_loop(&EmscriptenMainLoop, 0, 1);
#endif
  } catch (const std::runtime_error& ex) {
    std::cerr << ex.what() << '\n';
    return 1;
  }
  return 0;
}
#endif
