// The MIT License
//
// Copyright (c) 2026 MGTheTrain
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

#include <texture-manager.h>

#include <iostream>
#include <string>
#include <vector>

namespace Mgtt::Rendering {

Mgtt::Common::Result<void> TextureManager::LoadFromEnvMap(
    Mgtt::Rendering::RenderTexturesContainer& container,
    const std::vector<std::string>& texturePaths) {
  // TBD: upload individual cube faces
  for (const auto& path : texturePaths) {
    std::cout << "Allocated env map " << path << '\n';
  }
  return Mgtt::Common::Result<void>::Ok();
}

Mgtt::Common::Result<void> TextureManager::LoadFromHdr(
    Mgtt::Rendering::RenderTexturesContainer& container,
    std::string_view texturePath) {
  const std::string pathStr(texturePath);
  Mgtt::Rendering::Texture texture;

  texture.data = stbi_load(pathStr.c_str(), &texture.width, &texture.height,
                           &texture.nrComponents, 0);
  if (!texture.data) {
    return Mgtt::Common::Result<void>::Err("Failed to load HDR texture: " +
                                           pathStr);
  }

  glGenTextures(1, &container.hdrTextureId);
  glBindTexture(GL_TEXTURE_2D, container.hdrTextureId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, texture.width, texture.height, 0,
               GL_RGB, GL_UNSIGNED_BYTE, texture.data);
  stbi_image_free(texture.data);
  texture.data = nullptr;

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glGenFramebuffers(1, &container.fboId);
  glGenRenderbuffers(1, &container.rboId);
  glBindFramebuffer(GL_FRAMEBUFFER, container.fboId);
  glBindRenderbuffer(GL_RENDERBUFFER, container.rboId);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 128, 128);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, container.rboId);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    container.Clear();
    return Mgtt::Common::Result<void>::Err(
        "Framebuffer incomplete during HDR load");
  }

  glGenTextures(1, &container.cubeMapTextureId);
  glBindTexture(GL_TEXTURE_CUBE_MAP, container.cubeMapTextureId);
  for (uint32_t i = 0; i < 6; ++i) {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB8, 128, 128, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, nullptr);
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  if (container.eq2CubeMapShader.GetProgramId() == 0) {
    container.Clear();
    return Mgtt::Common::Result<void>::Err(
        "eq2CubeMapShader program missing — compile it before LoadFromHdr");
  }

  const glm::mat4 captureProjection =
      glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

  const std::vector<glm::mat4> captureViews = {
      glm::lookAt(glm::vec3(0.f), glm::vec3(1.f, 0.f, 0.f),
                  glm::vec3(0.f, -1.f, 0.f)),
      glm::lookAt(glm::vec3(0.f), glm::vec3(-1.f, 0.f, 0.f),
                  glm::vec3(0.f, -1.f, 0.f)),
      glm::lookAt(glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f),
                  glm::vec3(0.f, 0.f, 1.f)),
      glm::lookAt(glm::vec3(0.f), glm::vec3(0.f, -1.f, 0.f),
                  glm::vec3(0.f, 0.f, -1.f)),
      glm::lookAt(glm::vec3(0.f), glm::vec3(0.f, 0.f, 1.f),
                  glm::vec3(0.f, -1.f, 0.f)),
      glm::lookAt(glm::vec3(0.f), glm::vec3(0.f, 0.f, -1.f),
                  glm::vec3(0.f, -1.f, 0.f)),
  };

  const uint32_t progId = container.eq2CubeMapShader.GetProgramId();
  glUseProgram(progId);
  glUniform1i(glGetUniformLocation(progId, "equirectangularMap"), 0);
  glUniformMatrix4fv(glGetUniformLocation(progId, "projection"), 1, GL_FALSE,
                     &captureProjection[0][0]);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, container.hdrTextureId);

  glViewport(0, 0, 128, 128);
  glBindFramebuffer(GL_FRAMEBUFFER, container.fboId);
  for (uint32_t i = 0; i < 6; ++i) {
    glUniformMatrix4fv(glGetUniformLocation(progId, "view"), 1, GL_FALSE,
                       &captureViews[i][0][0]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                           container.cubeMapTextureId, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    SetupCube(container);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glDeleteTextures(1, &container.hdrTextureId);
  container.hdrTextureId = 0;
  container.textures.push_back(texture);

  std::cout << "Allocated env map from HDR: " << pathStr << '\n';
  return Mgtt::Common::Result<void>::Ok();
}

Mgtt::Common::Result<void> TextureManager::LoadBrdfLut(
    Mgtt::Rendering::RenderTexturesContainer& container) {
  if (container.brdfLutShader.GetProgramId() == 0) {
    return Mgtt::Common::Result<void>::Err(
        "brdfLutShader program missing — compile it before LoadBrdfLut");
  }
  if (HasValuesGreaterThanZero({container.brdfLutTextureId})) {
    std::cout << "BRDF LUT already allocated, skipping\n";
    return Mgtt::Common::Result<void>::Ok();
  }

  glGenTextures(1, &container.brdfLutTextureId);
  glBindTexture(GL_TEXTURE_2D, container.brdfLutTextureId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, 128, 128, 0, GL_RG, GL_UNSIGNED_BYTE,
               nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glBindFramebuffer(GL_FRAMEBUFFER, container.fboId);
  glBindRenderbuffer(GL_RENDERBUFFER, container.rboId);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 128, 128);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         container.brdfLutTextureId, 0);

  glViewport(0, 0, 128, 128);
  glUseProgram(container.brdfLutShader.GetProgramId());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  SetupQuad(container);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  std::cout << "BRDF LUT allocated\n";
  return Mgtt::Common::Result<void>::Ok();
}

void TextureManager::Clear(
    Mgtt::Rendering::RenderTexturesContainer& container) {
  container.Clear();
}

void TextureManager::SetupCube(
    Mgtt::Rendering::RenderTexturesContainer& container) {
  if (!HasValuesGreaterThanZero({container.cubeVao, container.cubeVbo})) {
    glGenVertexArrays(1, &container.cubeVao);
    glGenBuffers(1, &container.cubeVbo);
  }

  static constexpr float kVertices[] = {
      -1.f, 1.f,  -1.f, -1.f, -1.f, -1.f, 1.f,  -1.f, -1.f,
      1.f,  -1.f, -1.f, 1.f,  1.f,  -1.f, -1.f, 1.f,  -1.f,

      -1.f, -1.f, 1.f,  -1.f, -1.f, -1.f, -1.f, 1.f,  -1.f,
      -1.f, 1.f,  -1.f, -1.f, 1.f,  1.f,  -1.f, -1.f, 1.f,

      1.f,  -1.f, -1.f, 1.f,  -1.f, 1.f,  1.f,  1.f,  1.f,
      1.f,  1.f,  1.f,  1.f,  1.f,  -1.f, 1.f,  -1.f, -1.f,

      -1.f, -1.f, 1.f,  -1.f, 1.f,  1.f,  1.f,  1.f,  1.f,
      1.f,  1.f,  1.f,  1.f,  -1.f, 1.f,  -1.f, -1.f, 1.f,

      -1.f, 1.f,  -1.f, 1.f,  1.f,  -1.f, 1.f,  1.f,  1.f,
      1.f,  1.f,  1.f,  -1.f, 1.f,  1.f,  -1.f, 1.f,  -1.f,

      -1.f, -1.f, -1.f, -1.f, -1.f, 1.f,  1.f,  -1.f, -1.f,
      1.f,  -1.f, -1.f, -1.f, -1.f, 1.f,  1.f,  -1.f, 1.f,
  };

  const uint32_t posLoc = glGetAttribLocation(
      container.envMapShader.GetProgramId(), "inVertexPosition");

  glBindVertexArray(container.cubeVao);
  glBindBuffer(GL_ARRAY_BUFFER, container.cubeVbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(kVertices), kVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(posLoc);
  glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                        nullptr);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
}

void TextureManager::SetupQuad(
    Mgtt::Rendering::RenderTexturesContainer& container) {
  if (HasValuesGreaterThanZero({container.quadVao, container.quadVbo})) {
    std::cout << "Quad VAO/VBO already allocated, skipping\n";
    return;
  }

  static constexpr float kVertices[] = {
      -1.f, 1.f, 0.f, 0.f, 1.f, -1.f, -1.f, 0.f, 0.f, 0.f,
      1.f,  1.f, 0.f, 1.f, 1.f, 1.f,  -1.f, 0.f, 1.f, 0.f,
  };

  const uint32_t posLoc = glGetAttribLocation(
      container.brdfLutShader.GetProgramId(), "inVertexPosition");
  const uint32_t texLoc = glGetAttribLocation(
      container.brdfLutShader.GetProgramId(), "inVertexTextureCoordinates");

  glGenVertexArrays(1, &container.quadVao);
  glGenBuffers(1, &container.quadVbo);
  glBindVertexArray(container.quadVao);
  glBindBuffer(GL_ARRAY_BUFFER, container.quadVbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(kVertices), kVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(posLoc);
  glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        nullptr);
  glEnableVertexAttribArray(texLoc);
  glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        reinterpret_cast<void*>(3 * sizeof(float)));
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}

void TextureManager::GenerateIrradianceMap(
    Mgtt::Rendering::RenderTexturesContainer& container) {
  // TBD
}

}  // namespace Mgtt::Rendering
