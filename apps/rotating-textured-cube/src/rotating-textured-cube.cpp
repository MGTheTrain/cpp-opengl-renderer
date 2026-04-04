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

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
EM_JS(int, CanvasGetWidth, (), { return Module.canvas.width; });
EM_JS(int, CanvasGetHeight, (), { return Module.canvas.height; });
EM_JS(void, ResizeCanvas, (), { resizeCanvas(); });
#endif

namespace Mgtt::Apps {

// Construction / destruction

RotatingTexturedCube::~RotatingTexturedCube() {
  mesh_.Clear();
  shader_.Clear();
}

RotatingTexturedCube::RotatingTexturedCube() {
  window_ = std::make_unique<Mgtt::Window::GlfwWindow>(
      "rotating-textured-cube", windowWidth_, windowHeight_);
  window_->SetFramebufferSizeCallback(FramebufferSizeCallback);

#ifndef __EMSCRIPTEN__
  if (glewInit() != GLEW_OK) {
    throw std::runtime_error("GLEW could not be initialized");
  }
  const std::pair<std::string_view, std::string_view> kShaderPaths{
      "assets/shader/core/coordinate.vert",
      "assets/shader/core/coordinate.frag"};
#else
  const std::pair<std::string_view, std::string_view> kShaderPaths{
      "assets/shader/es/coordinate.vert", "assets/shader/es/coordinate.frag"};
#endif

  glEnable(GL_DEPTH_TEST);

  shader_ = Mgtt::Rendering::OpenGlShader(kShaderPaths);
  if (shader_.GetProgramId() == 0) {
    throw std::runtime_error("Coordinate shader failed to compile");
  }

  // Vertex data
  mesh_.vertexPositionAttribs = {
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

  mesh_.vertexTextureAttribs = {
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

  // GPU upload
  const uint32_t kShaderId = shader_.GetProgramId();

  glGenVertexArrays(1, &mesh_.vao);
  glBindVertexArray(mesh_.vao);

  glGenBuffers(1, &mesh_.pos);
  glBindBuffer(GL_ARRAY_BUFFER, mesh_.pos);
  glBufferData(GL_ARRAY_BUFFER,
               static_cast<GLsizeiptr>(mesh_.vertexPositionAttribs.size() *
                                       sizeof(glm::vec3)),
               mesh_.vertexPositionAttribs.data(), GL_STATIC_DRAW);
  const uint32_t kPosLoc = glGetAttribLocation(kShaderId, "inVertexPosition");
  glEnableVertexAttribArray(kPosLoc);
  glVertexAttribPointer(kPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                        nullptr);

  glGenBuffers(1, &mesh_.tex);
  glBindBuffer(GL_ARRAY_BUFFER, mesh_.tex);
  glBufferData(GL_ARRAY_BUFFER,
               static_cast<GLsizeiptr>(mesh_.vertexTextureAttribs.size() *
                                       sizeof(glm::vec2)),
               mesh_.vertexTextureAttribs.data(), GL_STATIC_DRAW);
  const uint32_t kTexLoc =
      glGetAttribLocation(kShaderId, "inVertexTextureCoordinates");
  glEnableVertexAttribArray(kTexLoc);
  glVertexAttribPointer(kTexLoc, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2),
                        nullptr);

  glBindVertexArray(0);

  // Texture
  mesh_.meshPrimitives.emplace_back();
  auto& tex = mesh_.meshPrimitives[0].pbrMaterial.baseColorTexture;

  glGenTextures(1, &tex.id);
  glBindTexture(GL_TEXTURE_2D, tex.id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  const std::string kTexturePath = "assets/texture/surgery.jpg";
  tex.data = stbi_load(kTexturePath.c_str(), &tex.width, &tex.height,
                       &tex.nrComponents, 0);
  if (tex.data == nullptr) {
    throw std::runtime_error("Failed to load texture: " + kTexturePath);
  }

  const GLenum kFormat = (tex.nrComponents == 4) ? GL_RGBA : GL_RGB;
  glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(kFormat), tex.width,
               tex.height, 0, kFormat, GL_UNSIGNED_BYTE, tex.data);
  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(tex.data);
  tex.data = nullptr;

  if (auto result = shader_.Use(); result.err()) {
    std::cerr << "Shader not ready: " << result.error() << "\n";
    return;
  }
  shader_.SetInt("textureMap", 0);

  int fbWidth = 0;
  int fbHeight = 0;
  glfwGetFramebufferSize(window_->GetWindow(), &fbWidth, &fbHeight);
  glViewport(0, 0, fbWidth, fbHeight);
}

// Render loop

void RotatingTexturedCube::Render() {
#ifndef __EMSCRIPTEN__
  while (!window_->WindowShouldClose()) {
#else
  windowWidth_ = static_cast<float>(CanvasGetWidth());
  windowHeight_ = static_cast<float>(CanvasGetHeight());
  window_->SetWindowSize(windowWidth_, windowHeight_);
#endif
    ProcessInput();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,
                  mesh_.meshPrimitives[0].pbrMaterial.baseColorTexture.id);

    if (auto result = shader_.Use(); result.err()) {
      std::cerr << "Shader not ready: " << result.error() << "\n";
      return;
    }

    auto [width, height] = window_->GetWindowSize();
    matrices_.model =
        glm::rotate(glm::mat4(1.0f), static_cast<float>(glfwGetTime()),
                    glm::vec3(0.5f, 1.0f, 0.0f));
    matrices_.view =
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    matrices_.projection = glm::perspective(
        glm::radians(45.0f),
        static_cast<float>(width) / static_cast<float>(height), 0.1f, 1000.0f);
    matrices_.mvp = matrices_.projection * matrices_.view * matrices_.model;

    shader_.SetMat4("mvp", matrices_.mvp);
    glBindVertexArray(mesh_.vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    window_->SwapBuffersAndPollEvents();
#ifndef __EMSCRIPTEN__
  }
#endif
}

// Input / callbacks

void RotatingTexturedCube::ProcessInput() {
  if (glfwGetKey(window_->GetWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window_->GetWindow(), 1);
  }
}

void RotatingTexturedCube::FramebufferSizeCallback(GLFWwindow*, int32_t width,
                                                   int32_t height) {
  glViewport(0, 0, width, height);
}

}  // namespace Mgtt::Apps

// Entry point

int main() {
  try {
#ifndef __EMSCRIPTEN__
    Mgtt::Apps::RotatingTexturedCube cube;
    cube.Render();
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
