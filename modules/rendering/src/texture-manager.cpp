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

#include <texture-manager.h>

/**
 * @brief Load cube map textures from the given folder path.
 *
 * This method loads cube map textures and
 * associates it with the provided RenderTexturesContainer. It performs
 * necessary operations to make the textures available for use in rendering.
 *
 * @param container The RenderTexturesContainer to associate with the loaded
 * cube map.
 * @param texturePathes The cube map texture pathes
 */
void Mgtt::Rendering::TextureManager::LoadFromEnvMap(
    Mgtt::Rendering::RenderTexturesContainer& container,
    const std::vector<std::string>& texturePathes) {
  // TBD
  for (auto& texturePath : texturePathes) {
    std::cout << "Allocated env map " << texturePath << std::endl;
  }
}

/**
 * @brief Load an HDR texture from the given file path.
 *
 * @param container The RenderTexturesContainer to associate with the loaded HDR
 * texture.
 * @param texturePath The file path to the HDR texture.
 */
void Mgtt::Rendering::TextureManager::LoadFromHdr(
    Mgtt::Rendering::RenderTexturesContainer& container,
    const std::string& texturePath) {
  Mgtt::Rendering::Texture texture;
  if (!texture.data) {
    texture.data = stbi_load(texturePath.c_str(), &texture.width,
                             &texture.height, &texture.nrComponents, 0);
    if (texture.data) {
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
        throw std::runtime_error("Framebuffer not complete");
      }

      
      glGenTextures(1, &container.cubeMapTextureId);
      glBindTexture(GL_TEXTURE_CUBE_MAP, container.cubeMapTextureId);
      for (uint32_t i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB8, 128, 128,
                     0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
      }
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      
      
      glm::mat4 captureProjection =
          glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
      std::vector<glm::mat4> captureViews = {
          glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
                      glm::vec3(0.0f, -1.0f, 0.0f)),
          glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f),
                      glm::vec3(0.0f, -1.0f, 0.0f)),
          glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
                      glm::vec3(0.0f, 0.0f, 1.0f)),
          glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),
                      glm::vec3(0.0f, 0.0f, -1.0f)),
          glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f),
                      glm::vec3(0.0f, -1.0f, 0.0f)),
          glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f),
                      glm::vec3(0.0f, -1.0f, 0.0f))};

      if (container.eq2CubeMapShader.GetProgramId() == 0) {
        container.Clear();
        throw std::runtime_error(
            "Ensure that a shader program for the eq2CubeMapShader exists");
      }

      glUseProgram(container.eq2CubeMapShader.GetProgramId());
      glUniform1i(
          glGetUniformLocation(container.eq2CubeMapShader.GetProgramId(),
                               "equirectangularMap"),
          0);
      glUniformMatrix4fv(
          glGetUniformLocation(container.eq2CubeMapShader.GetProgramId(),
                               "projection"),
          1, GL_FALSE, &captureProjection[0][0]);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, container.hdrTextureId);

      glViewport(0, 0, 128, 128);
      glBindFramebuffer(GL_FRAMEBUFFER, container.fboId);
      for (uint32_t i = 0; i < 6; ++i) {
        glUniformMatrix4fv(
            glGetUniformLocation(container.eq2CubeMapShader.GetProgramId(),
                                 "view"),
            1, GL_FALSE, &captureViews[i][0][0]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                               container.cubeMapTextureId, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        this->SetupCube(container);
      }
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      
      

      glDeleteTextures(1, &container.hdrTextureId);
      container.hdrTextureId = 0;
      container.textures.push_back(texture);
    }
  }
  std::cout << "Allocated env map from hdr " << texturePath << std::endl;
}

/**
 * @brief Clear the provided render textures container.
 *
 * @param container The RenderTexturesContainer to clear.
 */
void Mgtt::Rendering::TextureManager::Clear(
    Mgtt::Rendering::RenderTexturesContainer& container) {
  container.Clear();
}

/**
 * @brief Set up rendering resources for a cube.
 *
 * @param container A reference to a RenderTexturesContainer used to manage
 *                  rendering-related textures and resources.
 */
void Mgtt::Rendering::TextureManager::SetupCube(
    Mgtt::Rendering::RenderTexturesContainer& container) {
  if (!HasValuesGreaterThanZero({container.cubeVao, container.cubeVbo})) {
    glGenVertexArrays(1, &container.cubeVao);
    glGenBuffers(1, &container.cubeVbo);
  }
  std::vector<float> vertices = {
      -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
      1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

      -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,
      -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,

      1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,

      -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,

      -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,

      -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,
      1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f};
  uint32_t posLoc = glGetAttribLocation(container.envMapShader.GetProgramId(),
                                        "inVertexPosition");

  glBindVertexArray(container.cubeVao);
  glBindBuffer(GL_ARRAY_BUFFER, container.cubeVbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(),
               vertices.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(posLoc);
  glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                        reinterpret_cast<void*>(0));
  glBindVertexArray(container.cubeVao);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
}

/**
 * @brief Set up rendering resources for a cube.
 *
 * @param container A reference to a RenderTexturesContainer used to manage
 *                  rendering-related resources.
 */
void Mgtt::Rendering::TextureManager::SetupQuad(
    Mgtt::Rendering::RenderTexturesContainer& container) {
  if (!HasValuesGreaterThanZero({container.quadVao, container.quadVbo})) {
    uint32_t posLoc = glGetAttribLocation(
        container.brdfLutShader.GetProgramId(), "inVertexPosition");
    uint32_t texLoc = glGetAttribLocation(
        container.brdfLutShader.GetProgramId(), "inVertexTextureCoordinates");

    std::vector<float> vertices = {
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
    };
    glGenVertexArrays(1, &container.quadVao);
    glGenBuffers(1, &container.quadVbo);

    glBindVertexArray(container.quadVao);
    glBindBuffer(GL_ARRAY_BUFFER, container.quadVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(),
                 vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(posLoc);
    glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(texLoc);
    glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          reinterpret_cast<void*>(3 * sizeof(float)));

    glBindVertexArray(container.quadVao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
  } else {
    std::cout << "Quad VAO and Quad VBO resources have already been allocated"
              << std::endl;
  }
}

/**
 * @brief Load the BRDF Lookup Texture into the provided HDR texture container.
 *
 * @param container The RenderTexturesContainer to associate with the loaded
 * BRDF texture.
 */
void Mgtt::Rendering::TextureManager::LoadBrdfLut(
    Mgtt::Rendering::RenderTexturesContainer& container) {
  if (container.brdfLutShader.GetProgramId() == 0) {
    throw std::runtime_error("Ensure that a BRDF lut shader program exists");
  }
  if (!HasValuesGreaterThanZero({container.brdfLutTextureId})) {
    glGenTextures(1, &container.brdfLutTextureId);
    glBindTexture(GL_TEXTURE_2D, container.brdfLutTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, 128, 128, 0, GL_RG, GL_UNSIGNED_BYTE,
                 0);

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
    this->SetupQuad(container);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  } else {
    std::cout << "Brdf lut texture has already been allocated" << std::endl;
  }
  std::cout << "Brdf lut texture allocated" << std::endl;
}

/**
 * @brief Generate the irradiance map for the provided HDR texture container.
 *
 * @param container The RenderTexturesContainer to associate with the generated
 * irradiance map.
 */
void Mgtt::Rendering::TextureManager::GenerateIrradianceMap(
    Mgtt::Rendering::RenderTexturesContainer& container) {}