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
  this->cameraPosition = glm::vec3(0.0f, 0.0f, -1.0f);
  std::string appName = "opengl-viewer";
  float windowWidth = 1000.0f;
  float windowHeight = 1000.0f;

  this->glmMatrices = std::make_unique<GlmMatrices>();
  this->glmVectors = std::make_unique<GlmVectors>();
  this->glfwWindow = std::make_unique<Mgtt::Window::GlfwWindow>(
      appName, windowWidth, windowHeight);
  this->glfwWindow->SetFramebufferSizeCallback(
      Mgtt::Apps::OpenGlViewer::FramebufferSizeCallback);
  if (glewInit() != GLEW_OK) {
    throw std::runtime_error("GLEW ERROR: Glew could not be initialized");
  }
  glEnable(GL_DEPTH_TEST);

  // Compile shaders and link to OpenGl program
  std::pair<std::string, std::string> pbrShaderPathes = {
      "assets/shader/core/pbr.vert", "assets/shader/core/pbr.frag"};
  this->mgttScene.shader.Compile(pbrShaderPathes);
  std::pair<std::string, std::string> eq2BrdfLutShaderPathes = {
      "assets/shader/core/eq2CubeMap.vert",
      "assets/shader/core/eq2CubeMap.frag"};
  std::pair<std::string, std::string> brdfLutShaderPathes = {
      "assets/shader/core/genBrdf.vert", "assets/shader/core/genBrdf.frag"};
  std::pair<std::string, std::string> envMapShaderPathes = {
      "assets/shader/core/envMap.vert", "assets/shader/core/envMap.frag"};
  this->renderTextureContainer = Mgtt::Rendering::RenderTexturesContainer(
      eq2BrdfLutShaderPathes, brdfLutShaderPathes, envMapShaderPathes);

  // scene
  this->gltfSceneImporter =
      std::make_unique<Mgtt::Rendering::GltfSceneImporter>();
  std::string mgttScenePath = "assets/scenes/water-bottle/WaterBottle.gltf";
  // std::string mgttScenePath =
  // "assets/scenes/glTF-Sample-Models/2.0/DamagedHelmet/glTF/DamagedHelmet.gltf";
  this->gltfSceneImporter->Load(this->mgttScene, mgttScenePath);

  // equirectangular to env map
  std::string hdrTexturePath = "assets/texture/surgery.jpg";
  this->textureManager->LoadFromHdr(this->renderTextureContainer,
                                    hdrTexturePath);

  // brdf lut
  this->textureManager->LoadBrdfLut(this->renderTextureContainer);

  glViewport(0, 0, windowWidth, windowHeight);

  this->InitializeImGui();
  this->scaleIblAmbient = 1.0f;
  this->showEnvMap = false;
}

/**
 * @brief Renders the scene using OpenGL.
 *
 * This method is responsible for rendering the contents of the scene using
 * OpenGL.
 */
void Mgtt::Apps::OpenGlViewer::Render() {
  while (!this->glfwWindow->WindowShouldClose()) {
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

    //// Check brdf lut
    // this->renderTextureContainer.brdfLutShader.Use();

    // glBindVertexArray(this->renderTextureContainer.quadVao);
    // glDrawArrays(GL_TRIANGLES, 0, 6);
    // glBindVertexArray(0);

    // Check env map
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
  }
}

/**
 * @brief Iterates recursively over all nodes in the scene
 *
 * This function is responsible for iteraing recursively over all nodes in the
 *scene
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
 * This function is responsible for rendering the mesh using the current
 * rendering technique and associated settings. It should be called within the
 * rendering loop.
 */
void Mgtt::Apps::OpenGlViewer::RenderMesh(
    std::shared_ptr<Mgtt::Rendering::Node> node) {
  if (node->mesh) {
    glUniformMatrix4fv(
        glGetUniformLocation(this->mgttScene.shader.GetProgramId(), "matrix"),
        1, GL_FALSE, &node->mesh->matrix[0][0]);
    for (auto& meshPrimitve : node->mesh->meshPrimitives) {
      // base color
      if (meshPrimitve.pbrMaterial.baseColorTexture.id > 0) {
        this->mgttScene.shader.SetBool("baseColorTextureSet", true);
        this->mgttScene.shader.SetInt("baseColorMap", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,
                      meshPrimitve.pbrMaterial.baseColorTexture.id);
      } else {
        this->mgttScene.shader.SetBool("baseColorTextureSet", false);
      }

      // metallic roughness
      if (meshPrimitve.pbrMaterial.metallicRoughnessTexture.id > 0) {
        this->mgttScene.shader.SetBool("physicalDescriptorTextureSet", true);
        this->mgttScene.shader.SetInt("physicalDescriptorMap", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D,
                      meshPrimitve.pbrMaterial.metallicRoughnessTexture.id);
      } else {
        this->mgttScene.shader.SetBool("physicalDescriptorTextureSet", false);
      }

      // normal
      if (meshPrimitve.pbrMaterial.normalTexture.id > 0) {
        this->mgttScene.shader.SetBool("normalTextureSet", true);
        this->mgttScene.shader.SetInt("normalMap", 2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, meshPrimitve.pbrMaterial.normalTexture.id);
      } else {
        this->mgttScene.shader.SetBool("normalTextureSet", false);
      }

      // emissive
      if (meshPrimitve.pbrMaterial.emissiveTexture.id > 0) {
        this->mgttScene.shader.SetBool("emissiveTextureSet", true);
        this->mgttScene.shader.SetInt("emissiveMap", 3);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D,
                      meshPrimitve.pbrMaterial.emissiveTexture.id);
      } else {
        this->mgttScene.shader.SetBool("emissiveTextureSet", false);
      }

      // ambient occlusion
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
                     (void*)(meshPrimitve.firstIndex * sizeof(unsigned int)));
      glBindVertexArray(0);
    }
  }
}

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
void Mgtt::Apps::OpenGlViewer::FramebufferSizeCallback(GLFWwindow* window,
                                                       int width, int height) {
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
  ImGui_ImplOpenGL3_Init("#version 330 core");
}

/**
 * @brief Update settings trough ImGui widgets.
 */
void Mgtt::Apps::OpenGlViewer::UpdateSettings() {
  ImGui::Begin("opengl-viewer");
  if (ImGui::BeginTabBar("Settings")) {
    if (ImGui::BeginTabItem("Scene")) {
      if (ImGui::Button("Select glTF scene")) {
        nfdchar_t* selectedPath = NULL;
        nfdresult_t result = NFD_OpenDialog("gltf", NULL, &selectedPath);
        if (result == NFD_OKAY) {
          this->gltfSceneImporter->Clear(this->mgttScene);
          std::pair<std::string, std::string> pbrShaderPathes = {
              "assets/shader/core/pbr.vert", "assets/shader/core/pbr.frag"};
          this->mgttScene.shader.Compile(pbrShaderPathes);
          std::string mgttScenePath = std::string(selectedPath);
          this->gltfSceneImporter->Load(this->mgttScene, mgttScenePath);

          // Reset attributes
          this->scaleIblAmbient = 1.0f;
          this->showEnvMap = false;

          this->glmVectors->translation = glm::vec3(0.0f);
          this->glmVectors->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
          this->glmVectors->scale = glm::vec3(1.0f);

          this->glmMatrices->model = glm::mat4(1.0f);
        }
      }
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Model space transformation")) {
      this->glmMatrices->model = glm::mat4(1.0f);
      ImGui::SliderFloat3("Translation", (float*)&this->glmVectors->translation,
                          -1.0f, 1.0f);
      ImGui::Dummy(ImVec2(0.0f, 5.0f));
      ImGui::SliderFloat3("Rotation", (float*)&this->glmVectors->rotation, 0.0f,
                          360.0f);
      ImGui::Dummy(ImVec2(0.0f, 5.0f));
      ImGui::SliderFloat3("Scale", (float*)&this->glmVectors->scale, 0.1f,
                          3.0f);
      ImGui::Dummy(ImVec2(0.0f, 5.0f));
      glm::mat4 translationMatrix =
          glm::translate(glm::mat4(1.0f), this->glmVectors->translation);
      glm::mat4 rotationMatrix = glm::rotate(
          glm::mat4(1.0f), glm::radians(this->glmVectors->rotation.x),
          glm::vec3(1.0f, 0.0f, 0.0f));
      rotationMatrix = glm::rotate(rotationMatrix,
                                   glm::radians(this->glmVectors->rotation.y),
                                   glm::vec3(0.0f, 1.0f, 0.0f));
      rotationMatrix = glm::rotate(rotationMatrix,
                                   glm::radians(this->glmVectors->rotation.z),
                                   glm::vec3(0.0f, 0.0f, 1.0f));
      glm::mat4 scaleMatrix =
          glm::scale(glm::mat4(1.0f), this->glmVectors->scale);
      this->glmMatrices->model =
          translationMatrix * rotationMatrix * scaleMatrix;
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

int main() {
  Mgtt::Apps::OpenGlViewer openGlViewer;
  try {
    openGlViewer.Render();
  } catch (const std::runtime_error& ex) {
    std::cout << ex.what() << std::endl;
    openGlViewer.Clear();
    return 1;
  }
  return 0;
}
#endif
