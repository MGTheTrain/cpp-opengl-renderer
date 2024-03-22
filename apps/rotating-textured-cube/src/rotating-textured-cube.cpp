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

#ifdef MGTT_ROTATING_TEXTURED_CUBE
#include <rotating-textured-cube.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
EM_JS(int, CanvasGetWidth, (), { return Module.canvas.width; });

EM_JS(int, CanvasGetHeight, (), { return Module.canvas.height; });

EM_JS(void, ResizeCanvas, (), { resizeCanvas(); });
#endif

/**
 * @brief Destructs the RotatingTexturedCube  object.
 */
Mgtt::Apps::RotatingTexturedCube::~RotatingTexturedCube() {
  this->mesh.Clear();
  for (auto& shader : this->openGlShaders) {
    shader.Clear();
  }
}

/**
 * @brief Constructs an RotatingTexturedCube  object.
 */
Mgtt::Apps::RotatingTexturedCube::RotatingTexturedCube() {}

/**
 * @brief Initializes objects required for scene rendering using OpenGL.
 *
 * This method is responsible for initializing objects required for scene
 * rendering using OpenGL.
 */
void Mgtt::Apps::RotatingTexturedCube::Init() {
  std::string appName = "rotating-textured-cube";
  this->windowWidth = 1000.0f;
  this->windowHeight = 1000.0f;

  this->glmMatrices = std::make_unique<GlmMatrices>();

#ifndef __ANDROID__
  this->glfwWindow = std::make_unique<Mgtt::Window::GlfwWindow>(
      appName, this->windowWidth, this->windowHeight);
  this->glfwWindow->SetFramebufferSizeCallback(
      Mgtt::Apps::RotatingTexturedCube::FramebufferSizeCallback);
#else
  this->timeInSeconds = 0.0f;
#endif
#if !defined(__EMSCRIPTEN__) && !defined(__ANDROID__)
  if (glewInit() != GLEW_OK) {
    throw std::runtime_error("GLEW ERROR: Glew could not be initialized");
  }
  std::string vsPath = "assets/shader/core/coordinate.vert";
  std::string fsPath = "assets/shader/core/coordinate.frag";
#else
#ifdef __ANDROID__
  // Copy to the emulator or an actual Android OS device files via adb cli tool:
  // adb push <assets folder path>/shader/es/ /sdcard/Download/assets/shader/es/
  std::string vsPath = "/sdcard/Download/assets/shader/es/coordinate.vert";
  std::string fsPath = "/sdcard/Download/assets/shader/es/coordinate.frag";
#else
  std::string vsPath = "assets/shader/es/coordinate.vert";
  std::string fsPath = "assets/shader/es/coordinate.frag";
#endif
#endif
  glEnable(GL_DEPTH_TEST);

  std::pair<std::string, std::string> shaderPathes =
      std::make_pair(vsPath, fsPath);
  auto shader = Mgtt::Rendering::OpenGlShader(shaderPathes);
  this->openGlShaders.push_back(shader);

  this->mesh.vertexPositionAttribs = {
      glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, -0.5f, -0.5f),
      glm::vec3(0.5f, 0.5f, -0.5f),   glm::vec3(0.5f, 0.5f, -0.5f),
      glm::vec3(-0.5f, 0.5f, -0.5f),  glm::vec3(-0.5f, -0.5f, -0.5f),

      glm::vec3(-0.5f, -0.5f, 0.5f),  glm::vec3(0.5f, -0.5f, 0.5f),
      glm::vec3(0.5f, 0.5f, 0.5f),    glm::vec3(0.5f, 0.5f, 0.5f),
      glm::vec3(-0.5f, 0.5f, 0.5f),   glm::vec3(-0.5f, -0.5f, 0.5f),

      glm::vec3(-0.5f, 0.5f, 0.5f),   glm::vec3(-0.5f, 0.5f, -0.5f),
      glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-0.5f, -0.5f, -0.5f),
      glm::vec3(-0.5f, -0.5f, 0.5f),  glm::vec3(-0.5f, 0.5f, 0.5f),

      glm::vec3(0.5f, 0.5f, 0.5f),    glm::vec3(0.5f, 0.5f, -0.5f),
      glm::vec3(0.5f, -0.5f, -0.5f),  glm::vec3(0.5f, -0.5f, -0.5f),
      glm::vec3(0.5f, -0.5f, 0.5f),   glm::vec3(0.5f, 0.5f, 0.5f),

      glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, -0.5f, -0.5f),
      glm::vec3(0.5f, -0.5f, 0.5f),   glm::vec3(0.5f, -0.5f, 0.5f),
      glm::vec3(-0.5f, -0.5f, 0.5f),  glm::vec3(-0.5f, -0.5f, -0.5f),

      glm::vec3(-0.5f, 0.5f, -0.5f),  glm::vec3(0.5f, 0.5f, -0.5f),
      glm::vec3(0.5f, 0.5f, 0.5f),    glm::vec3(0.5f, 0.5f, 0.5f),
      glm::vec3(-0.5f, 0.5f, 0.5f),   glm::vec3(-0.5f, 0.5f, -0.5f)};

  this->mesh.vertexTextureAttribs = {
      glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f),
      glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f),
      glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f),
      glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f),
      glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f),
      glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
      glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f),
      glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
      glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f),
      glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f),
      glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f),
      glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f)};

  glGenVertexArrays(1, &this->mesh.vao);
  glBindVertexArray(this->mesh.vao);

  glGenBuffers(1, &this->mesh.pos);
  glBindBuffer(GL_ARRAY_BUFFER, this->mesh.pos);
  glBufferData(GL_ARRAY_BUFFER,
               this->mesh.vertexPositionAttribs.size() * sizeof(glm::vec3),
               this->mesh.vertexPositionAttribs.data(), GL_STATIC_DRAW);
  uint32_t posLocation = glGetAttribLocation(
      this->openGlShaders[0].GetProgramId(), "inVertexPosition");
  glEnableVertexAttribArray(posLocation);
  glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                        static_cast<void*>(0));

  glGenBuffers(1, &this->mesh.tex);
  glBindBuffer(GL_ARRAY_BUFFER, this->mesh.tex);
  glBufferData(GL_ARRAY_BUFFER,
               this->mesh.vertexTextureAttribs.size() * sizeof(glm::vec2),
               this->mesh.vertexTextureAttribs.data(), GL_STATIC_DRAW);
  uint32_t texLocation = glGetAttribLocation(
      this->openGlShaders[0].GetProgramId(), "inVertexTextureCoordinates");
  glEnableVertexAttribArray(texLocation);
  glVertexAttribPointer(texLocation, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2),
                        static_cast<void*>(0));

  Mgtt::Rendering::MeshPrimitive meshPrimitive;
  this->mesh.meshPrimitives.push_back(meshPrimitive);
  glGenTextures(1,
                &this->mesh.meshPrimitives[0].pbrMaterial.baseColorTexture.id);
  glBindTexture(GL_TEXTURE_2D,
                this->mesh.meshPrimitives[0].pbrMaterial.baseColorTexture.id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

#ifdef __ANDROID__
  // Copy to the emulator or an actual Android OS device files via adb cli tool:
  // adb push <assets folder path>/texture/ /sdcard/Download/assets/texture/
  std::string texturePath = "/sdcard/Download/assets/texture/surgery.jpg";
#else
  std::string texturePath = "assets/texture/surgery.jpg";
#endif
  this->mesh.meshPrimitives[0].pbrMaterial.baseColorTexture.data = stbi_load(
      texturePath.c_str(),
      &this->mesh.meshPrimitives[0].pbrMaterial.baseColorTexture.width,
      &this->mesh.meshPrimitives[0].pbrMaterial.baseColorTexture.height,
      &this->mesh.meshPrimitives[0].pbrMaterial.baseColorTexture.nrComponents,
      0);
  if (this->mesh.meshPrimitives[0].pbrMaterial.baseColorTexture.data) {
    if (this->mesh.meshPrimitives[0]
            .pbrMaterial.baseColorTexture.nrComponents == 3) {
      glTexImage2D(
          GL_TEXTURE_2D, 0, GL_RGB,
          this->mesh.meshPrimitives[0].pbrMaterial.baseColorTexture.width,
          this->mesh.meshPrimitives[0].pbrMaterial.baseColorTexture.height, 0,
          GL_RGB, GL_UNSIGNED_BYTE,
          this->mesh.meshPrimitives[0].pbrMaterial.baseColorTexture.data);
    } else if (this->mesh.meshPrimitives[0]
                   .pbrMaterial.baseColorTexture.nrComponents == 4) {
      glTexImage2D(
          GL_TEXTURE_2D, 0, GL_RGBA,
          this->mesh.meshPrimitives[0].pbrMaterial.baseColorTexture.width,
          this->mesh.meshPrimitives[0].pbrMaterial.baseColorTexture.height, 0,
          GL_RGBA, GL_UNSIGNED_BYTE,
          this->mesh.meshPrimitives[0].pbrMaterial.baseColorTexture.data);
    }
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    throw std::runtime_error("TEXTURE ERROR: Failed to load texture " +
                             texturePath);
  }
  if (this->mesh.meshPrimitives[0].pbrMaterial.baseColorTexture.data) {
    stbi_image_free(
        this->mesh.meshPrimitives[0].pbrMaterial.baseColorTexture.data);
    this->mesh.meshPrimitives[0].pbrMaterial.baseColorTexture.data = nullptr;
  }

  this->openGlShaders[0].Use();
  this->openGlShaders[0].SetInt("textureMap", 0);
}

/**
 * @brief Renders the scene using OpenGL.
 *
 * This method is responsible for rendering the contents of the scene using
 * OpenGL.
 */
void Mgtt::Apps::RotatingTexturedCube::Render() {
#if !defined(__EMSCRIPTEN__) && !defined(__ANDROID__)
  while (!this->glfwWindow->WindowShouldClose()) {
#elif defined(__EMSCRIPTEN__)
  this->windowWidth = CanvasGetWidth();
  this->windowHeight = CanvasGetHeight();
  this->glfwWindow->SetWindowSize(this->windowWidth, this->windowHeight);
#endif
    this->ProcessInput();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,
                  this->mesh.meshPrimitives[0].pbrMaterial.baseColorTexture.id);

    this->openGlShaders[0].Use();

    this->glmMatrices->model = glm::mat4(1.0f);
    this->glmMatrices->view = glm::mat4(1.0f);
    this->glmMatrices->projection = glm::mat4(1.0f);
#ifndef __ANDROID__
    this->glmMatrices->model =
        glm::rotate(this->glmMatrices->model, static_cast<float>(glfwGetTime()),
                    glm::vec3(0.5f, 1.0f, 0.0f));
#else
  if (this->timeInSeconds > 0.0f) {
    this->timeInSeconds = this->timeInSeconds - 0.1f;
  }
  if (this->timeInSeconds < 5.0f) {
    this->timeInSeconds = this->timeInSeconds + 0.1f;
  }
  this->glmMatrices->model =
      glm::rotate(this->glmMatrices->model, this->timeInSeconds,
                  glm::vec3(0.5f, 1.0f, 0.0f));
#endif
    this->glmMatrices->view =
        glm::translate(this->glmMatrices->view, glm::vec3(0.0f, 0.0f, -3.0f));

#ifndef __ANDROID__
    auto [width, height] = glfwWindow->GetWindowSize();
    this->glmMatrices->projection = glm::perspective(
        glm::radians(45.0f),
        static_cast<float>(width) / static_cast<float>(height), 0.1f, 1000.0f);
#else
  this->glmMatrices->projection =
      glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 1000.0f);
#endif
    this->glmMatrices->mvp = this->glmMatrices->projection *
                             this->glmMatrices->view * this->glmMatrices->model;
    this->openGlShaders[0].SetMat4("mvp", this->glmMatrices->mvp);
    glBindVertexArray(this->mesh.vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);

#ifndef __ANDROID__
    this->glfwWindow->SwapBuffersAndPollEvents();
#endif

#if !defined(__EMSCRIPTEN__) && !defined(__ANDROID__)
  }
#endif
}

/**
 * @brief Process input for the GLFW window.
 *
 * This function processes input for the specified GLFW window. It can handle
 * keyboard and mouse input events and update the application state accordingly.
 *
 * @param window A pointer to the GLFW window for which input should be
 * processed.
 */
void Mgtt::Apps::RotatingTexturedCube::ProcessInput() {
#ifndef __ANDROID__
  if (glfwGetKey(this->glfwWindow->GetWindow(), GLFW_KEY_ESCAPE) ==
      GLFW_PRESS) {
    glfwSetWindowShouldClose(this->glfwWindow->GetWindow(), true);
  }
#endif
}

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
void Mgtt::Apps::RotatingTexturedCube::FramebufferSizeCallback(
    GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}
#else
/**
 * @brief Updates the OpenGL viewport dimensions.
 *
 * This function is responsible for updating the dimensions of the OpenGL
 * viewport to match the specified width and height.
 *
 * @param width The new width of the viewport.
 * @param height The new height of the viewport.
 */
void Mgtt::Apps::RotatingTexturedCube::UpdateOpenGlViewPort(int width,
                                                            int height) {
  glViewport(0, 0, width, height);
}
#endif

#ifndef __ANDROID__
Mgtt::Apps::RotatingTexturedCube rotatingTexturedCube;
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
// @ref
// https://stackoverflow.com/questions/55415179/unable-to-pass-a-proper-lambda-to-emscripten-set-main-loop
void EmscriptenMainLoop() { rotatingTexturedCube.Render(); }
#endif

int main() {
  try {
    rotatingTexturedCube.Init();
#ifndef __EMSCRIPTEN__
    rotatingTexturedCube.Render();
#else
    emscripten_set_main_loop(&EmscriptenMainLoop, 0, 1);
#endif
  } catch (const std::exception& ex) {
    std::cout << ex.what() << std::endl;
    return 1;
  }
  return 0;
}
#else
extern "C" {
Mgtt::Apps::RotatingTexturedCube rotatingTexturedCube;

JNIEXPORT void JNICALL Java_com_mgtt_rtc_GL3JNILib_Init(JNIEnv* env,
                                                        jobject obj) {
  rotatingTexturedCube.Init();
}

JNIEXPORT void JNICALL Java_com_mgtt_rtc_GL3JNILib_Render(JNIEnv* env,
                                                          jobject obj) {
  rotatingTexturedCube.Render();
}

JNIEXPORT void JNICALL Java_com_mgtt_rtc_GL3JNILib_UpdateGlViewPort(
    JNIEnv* env, jobject obj, jint width, jint height) {
  rotatingTexturedCube.UpdateOpenGlViewPort(width, height);
}
};
#endif
#endif