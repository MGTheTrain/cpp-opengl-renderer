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

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
EM_JS(int, CanvasGetWidth, (), { return Module.canvas.width; });

EM_JS(int, CanvasGetHeight, (), { return Module.canvas.height; });

EM_JS(void, ResizeCanvas, (), { resizeCanvas(); });
#endif

/**
 * @brief Destructs the OpenGlViewer object.
 */
Mgtt::Apps::OpenGlViewer::~OpenGlViewer() { this->Clear(); }

/**
 * @brief Clears OpenGl and RAM allocated resources
 *
 */
void Mgtt::Apps::OpenGlViewer::Clear() {
  this->gltfSceneImporter->Clear(this->mgttScene);
  this->textureManager->Clear(this->renderTextureContainer);
  this->ClearImGui();
}

/**
 * @brief Constructs an OpenGlViewer object.
 */
Mgtt::Apps::OpenGlViewer::OpenGlViewer() {
  this->cameraPosition = glm::vec3(0.0f, 0.0f, -3.0f);
  std::string appName = "opengl-viewer";
  this->windowWidth = 1000.0f;
  this->windowHeight = 1000.0f;

  this->glmMatrices = std::make_unique<GlmMatrices>();
  this->glmVectors = std::make_unique<GlmVectors>();
  this->glfwWindow = std::make_unique<Mgtt::Window::GlfwWindow>(
      appName, windowWidth, windowHeight);
  this->glfwWindow->SetFramebufferSizeCallback(
      Mgtt::Apps::OpenGlViewer::FramebufferSizeCallback);
#ifndef __EMSCRIPTEN__
  if (glewInit() != GLEW_OK) {
    throw std::runtime_error("Glew could not be initialized");
  }

  std::pair<std::string, std::string> pbrshaderPaths = {
      "assets/shader/core/pbr.vert", "assets/shader/core/pbr.frag"};
  this->mgttScene.shader.Compile(pbrshaderPaths);
  std::pair<std::string, std::string> eq2BrdfLutshaderPaths = {
      "assets/shader/core/eq2CubeMap.vert",
      "assets/shader/core/eq2CubeMap.frag"};
  std::pair<std::string, std::string> brdfLutshaderPaths = {
      "assets/shader/core/genBrdf.vert", "assets/shader/core/genBrdf.frag"};
  std::pair<std::string, std::string> envMapshaderPaths = {
      "assets/shader/core/envMap.vert", "assets/shader/core/envMap.frag"};
  this->renderTextureContainer = Mgtt::Rendering::RenderTexturesContainer(
      eq2BrdfLutshaderPaths, brdfLutshaderPaths, envMapshaderPaths);
#else

  std::pair<std::string, std::string> pbrshaderPaths = {
      "assets/shader/es/pbr.vert", "assets/shader/es/pbr.frag"};
  this->mgttScene.shader.Compile(pbrshaderPaths);
  std::pair<std::string, std::string> eq2BrdfLutshaderPaths = {
      "assets/shader/es/eq2CubeMap.vert", "assets/shader/es/eq2CubeMap.frag"};
  std::pair<std::string, std::string> brdfLutshaderPaths = {
      "assets/shader/es/genBrdf.vert", "assets/shader/es/genBrdf.frag"};
  std::pair<std::string, std::string> envMapshaderPaths = {
      "assets/shader/es/envMap.vert", "assets/shader/es/envMap.frag"};
  this->renderTextureContainer = Mgtt::Rendering::RenderTexturesContainer(
      eq2BrdfLutshaderPaths, brdfLutshaderPaths, envMapshaderPaths);
#endif
  glEnable(GL_DEPTH_TEST);

  this->gltfSceneImporter =
      std::make_unique<Mgtt::Rendering::GltfSceneImporter>();
  std::string mgttScenePath = "assets/scenes/water-bottle/WaterBottle.gltf";

  this->gltfSceneImporter->Load(this->mgttScene, mgttScenePath);

  std::string hdrTexturePath = "assets/texture/surgery.jpg";
  this->textureManager->LoadFromHdr(this->renderTextureContainer,
                                    hdrTexturePath);

  this->textureManager->LoadBrdfLut(this->renderTextureContainer);

  glViewport(0, 0, windowWidth, windowHeight);

  this->InitializeImGui();
  this->scaleIblAmbient = 1.0f;
  this->showEnvMap = false;
}

/**
 * @brief Renders the scene using OpenGL.
 */
void Mgtt::Apps::OpenGlViewer::Render() {
#ifndef __EMSCRIPTEN__
  while (!this->glfwWindow->WindowShouldClose()) {
#else
  this->windowWidth = CanvasGetWidth();
  this->windowHeight = CanvasGetHeight();
  this->glfwWindow->SetWindowSize(this->windowWidth, this->windowHeight);
#endif

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->glmMatrices->view = glm::mat4(1.0f);
    this->glmMatrices->projection = glm::mat4(1.0f);
    auto [width, height] = this->glfwWindow->GetWindowSize();
    this->glmMatrices->projection = glm::perspective(
        glm::radians(45.0f),
        static_cast<float>(width) / static_cast<float>(height), 0.1f, 1000.0f);
    this->glmMatrices->view =
        glm::translate(this->glmMatrices->view, this->cameraPosition);

    this->glmMatrices->model = glm::scale(
        glm::mat4(1.0f), glm::vec3(1.0f / this->mgttScene.aabb.scale));
    this->glmMatrices->model =
        glm::scale(this->glmMatrices->model, this->glmVectors->scale);
    glm::vec3 offset =
        -this->mgttScene.aabb.center + this->glmVectors->translation;
    this->glmMatrices->model = glm::translate(this->glmMatrices->model, offset);
    this->glmMatrices->model = glm::rotate(
        this->glmMatrices->model, glm::radians(this->glmVectors->rotation.x),
        glm::vec3(1.0f, 0.0f, 0.0f));
    this->glmMatrices->model = glm::rotate(
        this->glmMatrices->model, glm::radians(this->glmVectors->rotation.y),
        glm::vec3(0.0f, 1.0f, 0.0f));
    this->glmMatrices->model = glm::rotate(
        this->glmMatrices->model, glm::radians(this->glmVectors->rotation.z),
        glm::vec3(0.0f, 0.0f, 1.0f));
    this->glmMatrices->model =
        glm::translate(this->glmMatrices->model, -offset);
    this->glmMatrices->model = glm::translate(this->glmMatrices->model, offset);

    this->mgttScene.mvp = this->glmMatrices->projection *
                          this->glmMatrices->view * this->glmMatrices->model;

    this->mgttScene.shader.Use();
    this->mgttScene.shader.SetMat4("model", this->glmMatrices->model);
    this->mgttScene.shader.SetMat4("mvp", this->mgttScene.mvp);
    this->mgttScene.shader.SetVec3("lightPosition", this->cameraPosition);
    this->mgttScene.shader.SetVec3("cameraPosition", this->cameraPosition);

    this->mgttScene.shader.SetInt("samplerEnvMap", 7);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_CUBE_MAP,
                  this->renderTextureContainer.cubeMapTextureId);

    this->mgttScene.shader.SetInt("samplerIrradianceMap", 8);
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_CUBE_MAP,
                  this->renderTextureContainer.cubeMapTextureId);

    this->mgttScene.shader.SetInt("samplerBrdfLut", 9);
    glActiveTexture(GL_TEXTURE9);
    glBindTexture(GL_TEXTURE_2D, this->renderTextureContainer.brdfLutTextureId);

    this->mgttScene.shader.SetFloat("scaleIblAmbient", this->scaleIblAmbient);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    auto [scrWidth, scrHeight] = this->glfwWindow->GetWindowSize();
    ImGui::SetNextWindowSize(ImVec2((float)scrWidth * 0.3f, (float)scrHeight));
    ImGui::SetNextWindowPos(ImVec2((float)scrWidth * 0.7f, 0.0f));
    this->UpdateSettings();

    for (auto& node : this->mgttScene.nodes) {
      this->TraverseSceneNode(node);
    }

    if (this->showEnvMap) {
      glDepthFunc(GL_LEQUAL);
      this->renderTextureContainer.envMapShader.Use();
      this->renderTextureContainer.envMapShader.SetMat4(
          "projection", this->glmMatrices->projection);
      this->renderTextureContainer.envMapShader.SetMat4(
          "view", this->glmMatrices->view);
      this->renderTextureContainer.envMapShader.SetInt("envMap", 0);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_CUBE_MAP,
                    this->renderTextureContainer.cubeMapTextureId);
      glBindVertexArray(this->renderTextureContainer.cubeVao);
      glDrawArrays(GL_TRIANGLES, 0, 36);
      glBindVertexArray(0);
      glDepthFunc(GL_LESS);
    }

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    this->glfwWindow->SwapBuffersAndPollEvents();
#ifndef __EMSCRIPTEN__
  }
#endif
}

/**
 * @brief Iterates recursively over all nodes in the scene
 *
 * @param node A shared pointer to the node.
 **/
void Mgtt::Apps::OpenGlViewer::TraverseSceneNode(
    std::shared_ptr<Mgtt::Rendering::Node> node) {
  RenderMesh(node);
  for (auto& child : node->children) {
    this->TraverseSceneNode(child);
  }
}

/**
 * @brief Renders the mesh using the specified rendering technique.
 *
 * @param node A shared pointer to the node.
 */
void Mgtt::Apps::OpenGlViewer::RenderMesh(
    std::shared_ptr<Mgtt::Rendering::Node> node) {
  if (node->mesh) {
    glUniformMatrix4fv(
        glGetUniformLocation(this->mgttScene.shader.GetProgramId(), "matrix"),
        1, GL_FALSE, &node->mesh->matrix[0][0]);
    for (auto& meshPrimitve : node->mesh->meshPrimitives) {
      if (meshPrimitve.pbrMaterial.baseColorTexture.id > 0) {
        this->mgttScene.shader.SetBool("baseColorTextureSet", true);
        this->mgttScene.shader.SetInt("baseColorMap", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,
                      meshPrimitve.pbrMaterial.baseColorTexture.id);
      } else {
        this->mgttScene.shader.SetBool("baseColorTextureSet", false);
      }

      if (meshPrimitve.pbrMaterial.metallicRoughnessTexture.id > 0) {
        this->mgttScene.shader.SetBool("physicalDescriptorTextureSet", true);
        this->mgttScene.shader.SetInt("physicalDescriptorMap", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D,
                      meshPrimitve.pbrMaterial.metallicRoughnessTexture.id);
      } else {
        this->mgttScene.shader.SetBool("physicalDescriptorTextureSet", false);
      }

      if (meshPrimitve.pbrMaterial.normalTexture.id > 0) {
        this->mgttScene.shader.SetBool("normalTextureSet", true);
        this->mgttScene.shader.SetInt("normalMap", 2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, meshPrimitve.pbrMaterial.normalTexture.id);
      } else {
        this->mgttScene.shader.SetBool("normalTextureSet", false);
      }

      if (meshPrimitve.pbrMaterial.emissiveTexture.id > 0) {
        this->mgttScene.shader.SetBool("emissiveTextureSet", true);
        this->mgttScene.shader.SetInt("emissiveMap", 3);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D,
                      meshPrimitve.pbrMaterial.emissiveTexture.id);
      } else {
        this->mgttScene.shader.SetBool("emissiveTextureSet", false);
      }

      if (meshPrimitve.pbrMaterial.occlusionTexture.id > 0) {
        this->mgttScene.shader.SetBool("occlusionTextureSet", true);
        this->mgttScene.shader.SetInt("occlusionMap", 4);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D,
                      meshPrimitve.pbrMaterial.occlusionTexture.id);
      } else {
        this->mgttScene.shader.SetBool("occlusionTextureSet", false);
      }

      this->mgttScene.shader.SetVec4(
          "baseColorFactor", meshPrimitve.pbrMaterial.baseColorTexture.color);
      this->mgttScene.shader.SetVec3(
          "emissiveFactor", meshPrimitve.pbrMaterial.emissiveTexture.color);
      this->mgttScene.shader.SetFloat(
          "occlusionFactor",
          meshPrimitve.pbrMaterial.occlusionTexture.strength);
      this->mgttScene.shader.SetFloat(
          "metallicFactor",
          meshPrimitve.pbrMaterial.metallicRoughnessTexture.metallicFactor);
      this->mgttScene.shader.SetFloat(
          "roughnessFactor",
          meshPrimitve.pbrMaterial.metallicRoughnessTexture.roughnessFactor);
      this->mgttScene.shader.SetBool("alphaMaskSet", true);
      this->mgttScene.shader.SetFloat("alphaMaskCutoff",
                                      meshPrimitve.pbrMaterial.alphaCutoff);

      glBindVertexArray(node->mesh->vao);
      glDrawElements(GL_TRIANGLES, meshPrimitve.indexCount, GL_UNSIGNED_INT,
                     (void*)(meshPrimitve.firstIndex * sizeof(uint32_t)));
      glBindVertexArray(0);
    }
  }
}

/**
 * @brief Callback function for framebuffer size changes.
 *
 * @param window A pointer to the GLFW window whose framebuffer size has
 * changed.
 * @param width  The new width of the framebuffer.
 * @param height The new height of the framebuffer.
 */
void Mgtt::Apps::OpenGlViewer::FramebufferSizeCallback(GLFWwindow* window,
                                                       int32_t width,
                                                       int32_t height) {
  glViewport(0, 0, width, height);
}

/**
 * @brief Initializes ImGui for the OpenGL viewer.
 */
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

/**
 * @brief Update settings trough ImGui widgets.
 */
void Mgtt::Apps::OpenGlViewer::UpdateSettings() {
  ImGui::Begin("Settings");
  if (ImGui::BeginTabBar("Settings")) {
    if (ImGui::BeginTabItem("Scene")) {
      if (ImGui::Button("Select glTF scene")) {
#ifndef __EMSCRIPTEN__
        NFD_Init();

        nfdu8filteritem_t filters[1] = {{"3D Models", "gltf"}};

        nfdopendialogu8args_t args = {0};
        args.filterList = filters;
        args.filterCount = 1;

        nfdu8char_t* selectedPath = NULL;

        nfdresult_t result = NFD_OpenDialogU8_With(&selectedPath, &args);

        if (result == NFD_OKAY) {
          this->gltfSceneImporter->Clear(this->mgttScene);
          std::pair<std::string, std::string> pbrshaderPaths = {
              "assets/shader/core/pbr.vert", "assets/shader/core/pbr.frag"};
          this->mgttScene.shader.Compile(pbrshaderPaths);
          std::string mgttScenePath = std::string(selectedPath);
          this->gltfSceneImporter->Load(this->mgttScene, mgttScenePath);

          this->scaleIblAmbient = 1.0f;
          this->showEnvMap = false;

          this->glmVectors->translation = glm::vec3(0.0f);
          this->glmVectors->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
          this->glmVectors->scale = glm::vec3(1.0f);

          NFD_FreePathU8(selectedPath);
        } else if (result == NFD_CANCEL) {
        } else {
          std::string msg = std::string(NFD_GetError());
          std::cerr << msg;
        }

        NFD_Quit();
#endif
      }
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Model space transformation")) {
      this->glmMatrices->model = glm::mat4(1.0f);
      ImGui::SliderFloat3("Translation", (float*)&this->glmVectors->translation,
                          -this->mgttScene.aabb.scale,
                          this->mgttScene.aabb.scale);
      ImGui::Dummy(ImVec2(0.0f, 5.0f));
      ImGui::SliderFloat3("Rotation", (float*)&this->glmVectors->rotation, 0.0f,
                          360.0f);
      ImGui::Dummy(ImVec2(0.0f, 5.0f));
      ImGui::SliderFloat3("Scale", (float*)&this->glmVectors->scale, 0.01f,
                          3.0f);
      ImGui::Dummy(ImVec2(0.0f, 5.0f));
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Light")) {
      ImGui::Text("Image based lighting");
      ImGui::SliderFloat("Scale ibl ambient", &this->scaleIblAmbient, 0.0f,
                         2.0f);
      ImGui::Text("Scale ibl");
      ImGui::Dummy(ImVec2(0.0f, 5.0f));
      ImGui::Checkbox("Show env map", &this->showEnvMap);
      ImGui::Dummy(ImVec2(0.0f, 5.0f));
      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }
  ImGui::End();
  ImGui::Render();
}

/**
 * @brief Initializes ImGui for the OpenGL viewer.
 */
void Mgtt::Apps::OpenGlViewer::ClearImGui() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

Mgtt::Apps::OpenGlViewer openGlViewer;
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
/**
 * NOTE: See
 * https://stackoverflow.com/questions/55415179/unable-to-pass-a-proper-lambda-to-emscripten-set-main-loop
 */
void EmscriptenMainLoop() { openGlViewer.Render(); }
#endif

int main() {
  try {
#ifndef __EMSCRIPTEN__
    openGlViewer.Render();
#else
    emscripten_set_main_loop(&EmscriptenMainLoop, 0, 1);
#endif
  } catch (const std::runtime_error& ex) {
    std::cout << ex.what() << std::endl;
    openGlViewer.Clear();
    return 1;
  }
  return 0;
}
#endif
