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
#include <stb_image.h>

#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
EM_JS(int, CanvasGetWidth, (), { return Module.canvas.width; });
EM_JS(int, CanvasGetHeight, (), { return Module.canvas.height; });
EM_JS(void, ResizeCanvas, (), { resizeCanvas(); });
#endif

// Construction / destruction

Mgtt::Apps::RotatingTexturedCube::~RotatingTexturedCube() {
  mesh.Clear();
  for (auto& shader : openGlShaders) {
    shader.Clear();
  }
}

Mgtt::Apps::RotatingTexturedCube::RotatingTexturedCube()
    : glmMatrices(std::make_unique<GlmMatrices>()) {
  const std::string appName = "rotating-textured-cube";
  glfwWindow = std::make_unique<Mgtt::Window::GlfwWindow>(appName, windowWidth,
                                                          windowHeight);
  glfwWindow->SetFramebufferSizeCallback(
      Mgtt::Apps::RotatingTexturedCube::FramebufferSizeCallback);

#ifndef __EMSCRIPTEN__
  if (glewInit() != GLEW_OK) {
    throw std::runtime_error("GLEW could not be initialized");
  }

  const std::pair<std::string_view, std::string_view> shaderPaths{
      "assets/shader/core/coordinate.vert",
      "assets/shader/core/coordinate.frag"};
#else
  const std::pair<std::string_view, std::string_view> shaderPaths{
      "assets/shader/es/coordinate.vert", "assets/shader/es/coordinate.frag"};
#endif

  glEnable(GL_DEPTH_TEST);

  // OpenGlShader is move-only — emplace_back constructs in-place
  openGlShaders.emplace_back(shaderPaths);
  if (openGlShaders[0].GetProgramId() == 0) {
    throw std::runtime_error("Coordinate shader failed to compile");
  }

  // --- vertex data ---
  mesh.vertexPositionAttribs = {
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
      glm::vec3(-0.5f, 0.5f, 0.5f),   glm::vec3(-0.5f, 0.5f, -0.5f),
  };

  mesh.vertexTextureAttribs = {
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
      glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f),
  };

  // --- GPU upload ---
  const uint32_t shaderId = openGlShaders[0].GetProgramId();

  glGenVertexArrays(1, &mesh.vao);
  glBindVertexArray(mesh.vao);

  glGenBuffers(1, &mesh.pos);
  glBindBuffer(GL_ARRAY_BUFFER, mesh.pos);
  glBufferData(GL_ARRAY_BUFFER,
               static_cast<GLsizeiptr>(mesh.vertexPositionAttribs.size() *
                                       sizeof(glm::vec3)),
               mesh.vertexPositionAttribs.data(), GL_STATIC_DRAW);
  const uint32_t posLoc = glGetAttribLocation(shaderId, "inVertexPosition");
  glEnableVertexAttribArray(posLoc);
  glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                        nullptr);

  glGenBuffers(1, &mesh.tex);
  glBindBuffer(GL_ARRAY_BUFFER, mesh.tex);
  glBufferData(GL_ARRAY_BUFFER,
               static_cast<GLsizeiptr>(mesh.vertexTextureAttribs.size() *
                                       sizeof(glm::vec2)),
               mesh.vertexTextureAttribs.data(), GL_STATIC_DRAW);
  const uint32_t texLoc =
      glGetAttribLocation(shaderId, "inVertexTextureCoordinates");
  glEnableVertexAttribArray(texLoc);
  glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2),
                        nullptr);

  glBindVertexArray(0);

  // --- texture ---
  mesh.meshPrimitives.emplace_back();
  auto& tex = mesh.meshPrimitives[0].pbrMaterial.baseColorTexture;

  glGenTextures(1, &tex.id);
  glBindTexture(GL_TEXTURE_2D, tex.id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  const std::string texturePath = "assets/texture/surgery.jpg";
  tex.data = stbi_load(texturePath.c_str(), &tex.width, &tex.height,
                       &tex.nrComponents, 0);
  if (tex.data == nullptr) {
    throw std::runtime_error("Failed to load texture: " + texturePath);
  }

  const GLenum format = (tex.nrComponents == 4) ? GL_RGBA : GL_RGB;
  glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(format), tex.width,
               tex.height, 0, format, GL_UNSIGNED_BYTE, tex.data);
  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(tex.data);
  tex.data = nullptr;

  openGlShaders[0].Use();
  openGlShaders[0].SetInt("textureMap", 0);

  int fbWidth = 0;
  int fbHeight = 0;
  glfwGetFramebufferSize(glfwWindow->GetWindow(), &fbWidth, &fbHeight);
  glViewport(0, 0, fbWidth, fbHeight);
}

// Render loop

void Mgtt::Apps::RotatingTexturedCube::Render() {
#ifndef __EMSCRIPTEN__
  while (!glfwWindow->WindowShouldClose()) {
#else
  windowWidth = static_cast<float>(CanvasGetWidth());
  windowHeight = static_cast<float>(CanvasGetHeight());
  glfwWindow->SetWindowSize(windowWidth, windowHeight);
#endif
    ProcessInput();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,
                  mesh.meshPrimitives[0].pbrMaterial.baseColorTexture.id);

    openGlShaders[0].Use();

    auto [width, height] = glfwWindow->GetWindowSize();
    glmMatrices->model =
        glm::rotate(glm::mat4(1.0f), static_cast<float>(glfwGetTime()),
                    glm::vec3(0.5f, 1.0f, 0.0f));
    glmMatrices->view =
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    glmMatrices->projection = glm::perspective(
        glm::radians(45.0f),
        static_cast<float>(width) / static_cast<float>(height), 0.1f, 1000.0f);
    glmMatrices->mvp =
        glmMatrices->projection * glmMatrices->view * glmMatrices->model;

    openGlShaders[0].SetMat4("mvp", glmMatrices->mvp);
    glBindVertexArray(mesh.vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glfwWindow->SwapBuffersAndPollEvents();
#ifndef __EMSCRIPTEN__
  }  // end while
#endif
}

// Input / callbacks

void Mgtt::Apps::RotatingTexturedCube::ProcessInput() {
  if (glfwGetKey(glfwWindow->GetWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(glfwWindow->GetWindow(), 1);
  }
}

void Mgtt::Apps::RotatingTexturedCube::FramebufferSizeCallback(GLFWwindow*,
                                                               int32_t width,
                                                               int32_t height) {
  glViewport(0, 0, width, height);
}

// Entry point

int main() {
  try {
#ifndef __EMSCRIPTEN__
    Mgtt::Apps::RotatingTexturedCube rotatingTexturedCube;
    rotatingTexturedCube.Render();
#else
    static Mgtt::Apps::RotatingTexturedCube* instance =
        new Mgtt::Apps::RotatingTexturedCube();
    emscripten_set_main_loop([]() { instance->Render(); }, 0, 1);
#endif
  } catch (const std::exception& ex) {
    std::cerr << ex.what() << '\n';
    return 1;
  }
  return 0;
}
#endif
