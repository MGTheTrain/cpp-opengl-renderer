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

// Construction / destruction

Mgtt::Apps::OpenGlViewer::~OpenGlViewer() { Clear(); }

void Mgtt::Apps::OpenGlViewer::Clear() {
  gltfSceneImporter->Clear(mgttScene);
  textureManager->Clear(renderTextureContainer);
  ClearImGui();
}

Mgtt::Apps::OpenGlViewer::OpenGlViewer()
    : cameraPosition(0.0f, 0.0f, -3.0f),
      windowWidth(1000.0f),
      windowHeight(1000.0f),
      scaleIblAmbient(1.0f),
      showEnvMap(false),
      glmMatrices(std::make_unique<GlmMatrices>()),
      glmVectors(std::make_unique<GlmVectors>()),
      gltfSceneImporter(std::make_unique<Mgtt::Rendering::GltfSceneImporter>()),
      textureManager(std::make_unique<Mgtt::Rendering::TextureManager>()) {
  const std::string appName = "opengl-viewer";
  glfwWindow = std::make_unique<Mgtt::Window::GlfwWindow>(appName, windowWidth,
                                                          windowHeight);
  glfwWindow->SetFramebufferSizeCallback(
      Mgtt::Apps::OpenGlViewer::FramebufferSizeCallback);

#ifndef __EMSCRIPTEN__
  if (glewInit() != GLEW_OK) {
    throw std::runtime_error("GLEW could not be initialized");
  }

  const std::pair<std::string_view, std::string_view> pbrShaderPaths{
      "assets/shader/core/pbr.vert", "assets/shader/core/pbr.frag"};
  if (auto r = mgttScene.shader.Compile(pbrShaderPaths); r.err()) {
    throw std::runtime_error("PBR shader compile failed: " + r.error());
  }

  // RenderTexturesContainer owns OpenGlShader members which are move-only,
  // so construct in-place via the parameterised constructor and move-assign.
  renderTextureContainer = Mgtt::Rendering::RenderTexturesContainer(
      std::pair<std::string_view, std::string_view>{
          "assets/shader/core/eq2CubeMap.vert",
          "assets/shader/core/eq2CubeMap.frag"},
      std::pair<std::string_view, std::string_view>{
          "assets/shader/core/genBrdf.vert", "assets/shader/core/genBrdf.frag"},
      std::pair<std::string_view, std::string_view>{
          "assets/shader/core/envMap.vert", "assets/shader/core/envMap.frag"});
#else
  const std::pair<std::string_view, std::string_view> pbrShaderPaths{
      "assets/shader/es/pbr.vert", "assets/shader/es/pbr.frag"};
  if (auto r = mgttScene.shader.Compile(pbrShaderPaths); r.err()) {
    throw std::runtime_error("PBR shader compile failed: " + r.error());
  }

  renderTextureContainer = Mgtt::Rendering::RenderTexturesContainer(
      std::pair<std::string_view, std::string_view>{
          "assets/shader/es/eq2CubeMap.vert",
          "assets/shader/es/eq2CubeMap.frag"},
      std::pair<std::string_view, std::string_view>{
          "assets/shader/es/genBrdf.vert", "assets/shader/es/genBrdf.frag"},
      std::pair<std::string_view, std::string_view>{
          "assets/shader/es/envMap.vert", "assets/shader/es/envMap.frag"});
#endif
  glEnable(GL_DEPTH_TEST);

  if (auto r = gltfSceneImporter->Load(
          mgttScene, "assets/scenes/water-bottle/WaterBottle.gltf");
      r.err()) {
    throw std::runtime_error("Scene load failed: " + r.error());
  }

  if (auto r = textureManager->LoadFromHdr(renderTextureContainer,
                                           "assets/texture/surgery.jpg");
      r.err()) {
    throw std::runtime_error("HDR texture load failed: " + r.error());
  }

  if (auto r = textureManager->LoadBrdfLut(renderTextureContainer); r.err()) {
    throw std::runtime_error("BRDF LUT generation failed: " + r.error());
  }

  int fbWidth = 0;
  int fbHeight = 0;
  glfwGetFramebufferSize(glfwWindow->GetWindow(), &fbWidth, &fbHeight);
  glViewport(0, 0, fbWidth, fbHeight);
  InitializeImGui();
}

// Render loop

void Mgtt::Apps::OpenGlViewer::Render() {
#ifndef __EMSCRIPTEN__
  while (!glfwWindow->WindowShouldClose()) {
#else
  windowWidth = static_cast<float>(CanvasGetWidth());
  windowHeight = static_cast<float>(CanvasGetHeight());
  glfwWindow->SetWindowSize(windowWidth, windowHeight);
#endif

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto [width, height] = glfwWindow->GetWindowSize();
    glmMatrices->view = glm::mat4(1.0f);
    glmMatrices->projection = glm::perspective(
        glm::radians(45.0f),
        static_cast<float>(width) / static_cast<float>(height), 0.1f, 1000.0f);
    glmMatrices->view = glm::translate(glmMatrices->view, cameraPosition);

    glmMatrices->model =
        glm::scale(glm::mat4(1.0f), glm::vec3(1.0f / mgttScene.aabb.scale));
    glmMatrices->model = glm::scale(glmMatrices->model, glmVectors->scale);

    const glm::vec3 offset = -mgttScene.aabb.center + glmVectors->translation;
    glmMatrices->model = glm::translate(glmMatrices->model, offset);
    glmMatrices->model =
        glm::rotate(glmMatrices->model, glm::radians(glmVectors->rotation.x),
                    glm::vec3(1.0f, 0.0f, 0.0f));
    glmMatrices->model =
        glm::rotate(glmMatrices->model, glm::radians(glmVectors->rotation.y),
                    glm::vec3(0.0f, 1.0f, 0.0f));
    glmMatrices->model =
        glm::rotate(glmMatrices->model, glm::radians(glmVectors->rotation.z),
                    glm::vec3(0.0f, 0.0f, 1.0f));
    glmMatrices->model = glm::translate(glmMatrices->model, -offset);
    glmMatrices->model = glm::translate(glmMatrices->model, offset);

    mgttScene.mvp =
        glmMatrices->projection * glmMatrices->view * glmMatrices->model;

    mgttScene.shader.Use();
    mgttScene.shader.SetMat4("model", glmMatrices->model);
    mgttScene.shader.SetMat4("mvp", mgttScene.mvp);
    mgttScene.shader.SetVec3("lightPosition", cameraPosition);
    mgttScene.shader.SetVec3("cameraPosition", cameraPosition);

    mgttScene.shader.SetInt("samplerEnvMap", 7);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_CUBE_MAP, renderTextureContainer.cubeMapTextureId);

    mgttScene.shader.SetInt("samplerIrradianceMap", 8);
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_CUBE_MAP, renderTextureContainer.cubeMapTextureId);

    mgttScene.shader.SetInt("samplerBrdfLut", 9);
    glActiveTexture(GL_TEXTURE9);
    glBindTexture(GL_TEXTURE_2D, renderTextureContainer.brdfLutTextureId);

    mgttScene.shader.SetFloat("scaleIblAmbient", scaleIblAmbient);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    auto [scrW, scrH] = glfwWindow->GetWindowSize();
    ImGui::SetNextWindowSize(
        ImVec2(static_cast<float>(scrW) * 0.3f, static_cast<float>(scrH)));
    ImGui::SetNextWindowPos(ImVec2(static_cast<float>(scrW) * 0.7f, 0.0f));
    UpdateSettings();

    for (auto& node : mgttScene.nodes) {
      TraverseSceneNode(node);
    }

    if (showEnvMap) {
      glDepthFunc(GL_LEQUAL);
      renderTextureContainer.envMapShader.Use();
      renderTextureContainer.envMapShader.SetMat4("projection",
                                                  glmMatrices->projection);
      renderTextureContainer.envMapShader.SetMat4("view", glmMatrices->view);
      renderTextureContainer.envMapShader.SetInt("envMap", 0);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_CUBE_MAP,
                    renderTextureContainer.cubeMapTextureId);
      glBindVertexArray(renderTextureContainer.cubeVao);
      glDrawArrays(GL_TRIANGLES, 0, 36);
      glBindVertexArray(0);
      glDepthFunc(GL_LESS);
    }

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwWindow->SwapBuffersAndPollEvents();
#ifndef __EMSCRIPTEN__
  }  // end while
#endif
}

// Scene traversal

void Mgtt::Apps::OpenGlViewer::TraverseSceneNode(
    const std::shared_ptr<Mgtt::Rendering::Node>& node) const {
  RenderMesh(node);
  for (const auto& child : node->children) {
    TraverseSceneNode(child);
  }
}

void Mgtt::Apps::OpenGlViewer::RenderMesh(
    const std::shared_ptr<Mgtt::Rendering::Node>& node) const {
  if (node->mesh == nullptr) {
    return;
  }

  glUniformMatrix4fv(
      glGetUniformLocation(mgttScene.shader.GetProgramId(), "matrix"), 1,
      GL_FALSE, &node->mesh->matrix[0][0]);

  for (const auto& prim : node->mesh->meshPrimitives) {
    const auto& mat = prim.pbrMaterial;

    auto bindTex = [&](uint32_t id, int slot, std::string_view flag,
                       std::string_view sampler) {
      if (id > 0) {
        mgttScene.shader.SetBool(flag, true);
        mgttScene.shader.SetInt(sampler, slot);
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, id);
      } else {
        mgttScene.shader.SetBool(flag, false);
      }
    };

    bindTex(mat.baseColorTexture.id, 0, "baseColorTextureSet", "baseColorMap");
    bindTex(mat.metallicRoughnessTexture.id, 1, "physicalDescriptorTextureSet",
            "physicalDescriptorMap");
    bindTex(mat.normalTexture.id, 2, "normalTextureSet", "normalMap");
    bindTex(mat.emissiveTexture.id, 3, "emissiveTextureSet", "emissiveMap");
    bindTex(mat.occlusionTexture.id, 4, "occlusionTextureSet", "occlusionMap");

    mgttScene.shader.SetVec4("baseColorFactor", mat.baseColorTexture.color);
    mgttScene.shader.SetVec3("emissiveFactor", mat.emissiveTexture.color);
    mgttScene.shader.SetFloat("occlusionFactor", mat.occlusionTexture.strength);
    mgttScene.shader.SetFloat("metallicFactor",
                              mat.metallicRoughnessTexture.metallicFactor);
    mgttScene.shader.SetFloat("roughnessFactor",
                              mat.metallicRoughnessTexture.roughnessFactor);
    mgttScene.shader.SetBool("alphaMaskSet", true);
    mgttScene.shader.SetFloat("alphaMaskCutoff", mat.alphaCutoff);

    glBindVertexArray(node->mesh->vao);
    glDrawElements(
        GL_TRIANGLES, static_cast<GLsizei>(prim.indexCount), GL_UNSIGNED_INT,
        // NOLINTNEXTLINE(performance-no-int-to-ptr)
        reinterpret_cast<const void*>(prim.firstIndex * sizeof(uint32_t)));
    glBindVertexArray(0);
  }
}

// ImGui

void Mgtt::Apps::OpenGlViewer::FramebufferSizeCallback(GLFWwindow*,
                                                       int32_t width,
                                                       int32_t height) {
  glViewport(0, 0, width, height);
}

void Mgtt::Apps::OpenGlViewer::InitializeImGui() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  ImGui_ImplGlfw_InitForOpenGL(glfwWindow->GetWindow(), true);
#ifndef __EMSCRIPTEN__
  ImGui_ImplOpenGL3_Init("#version 330 core");
#else
  ImGui_ImplOpenGL3_Init("#version 300 es");
  ResizeCanvas();
#endif
}

void Mgtt::Apps::OpenGlViewer::UpdateSettings() {
  ImGui::Begin("Settings");
  if (ImGui::BeginTabBar("Settings")) {
    if (ImGui::BeginTabItem("Scene")) {
      if (ImGui::Button("Select glTF scene")) {
#ifndef __EMSCRIPTEN__
        NFD_Init();
        nfdu8filteritem_t filters[1] = {{"3D Models", "gltf"}};
        nfdopendialogu8args_t args{};
        args.filterList = filters;
        args.filterCount = 1;
        nfdu8char_t* selectedPath = nullptr;

        if (NFD_OpenDialogU8_With(&selectedPath, &args) == NFD_OKAY) {
          gltfSceneImporter->Clear(mgttScene);

          const std::pair<std::string_view, std::string_view> paths{
              "assets/shader/core/pbr.vert", "assets/shader/core/pbr.frag"};
          if (auto r = mgttScene.shader.Compile(paths); r.err()) {
            std::cerr << "Shader recompile failed: " << r.error() << '\n';
          } else {
            const std::string scenePath(selectedPath);
            if (auto r = gltfSceneImporter->Load(mgttScene, scenePath);
                r.err()) {
              std::cerr << "Scene load failed: " << r.error() << '\n';
            }
          }

          scaleIblAmbient = 1.0f;
          showEnvMap = false;
          glmVectors->translation = glm::vec3(0.0f);
          glmVectors->rotation = glm::vec3(0.0f);
          glmVectors->scale = glm::vec3(1.0f);
          NFD_FreePathU8(selectedPath);
        } else if (const char* err = NFD_GetError(); err) {
          std::cerr << err << '\n';
        }
        NFD_Quit();
#endif
      }
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Model space transformation")) {
      glmMatrices->model = glm::mat4(1.0f);
      ImGui::SliderFloat3("Translation",
                          reinterpret_cast<float*>(&glmVectors->translation),
                          -mgttScene.aabb.scale, mgttScene.aabb.scale);
      ImGui::Dummy(ImVec2(0.0f, 5.0f));
      ImGui::SliderFloat3("Rotation",
                          reinterpret_cast<float*>(&glmVectors->rotation), 0.0f,
                          360.0f);
      ImGui::Dummy(ImVec2(0.0f, 5.0f));
      ImGui::SliderFloat3("Scale", reinterpret_cast<float*>(&glmVectors->scale),
                          0.01f, 3.0f);
      ImGui::Dummy(ImVec2(0.0f, 5.0f));
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Light")) {
      ImGui::Text("Image based lighting");
      ImGui::SliderFloat("Scale ibl ambient", &scaleIblAmbient, 0.0f, 2.0f);
      ImGui::Dummy(ImVec2(0.0f, 5.0f));
      ImGui::Checkbox("Show env map", &showEnvMap);
      ImGui::Dummy(ImVec2(0.0f, 5.0f));
      ImGui::EndTabItem();
    }

    ImGui::EndTabBar();
  }
  ImGui::End();
  ImGui::Render();
}

void Mgtt::Apps::OpenGlViewer::ClearImGui() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

// Entry point

#ifdef __EMSCRIPTEN__
static Mgtt::Apps::OpenGlViewer* gViewer = nullptr;
void EmscriptenMainLoop() { gViewer->Render(); }
#endif

int main() {
  try {
#ifndef __EMSCRIPTEN__
    Mgtt::Apps::OpenGlViewer openGlViewer;
    openGlViewer.Render();
#else
    // Heap-allocate so the object survives main() returning.
    // Emscripten manages the process lifetime; the OS reclaims
    // memory when the page is closed.
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
