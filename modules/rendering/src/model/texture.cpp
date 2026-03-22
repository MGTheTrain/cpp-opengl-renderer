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

#include <texture.h>

#include <iostream>

namespace Mgtt::Rendering {

void Texture::Clear() {
  if (id > 0) {
    glDeleteTextures(1, &id);
    id = 0;
  }
}

NormalTexture::NormalTexture(Texture&& texture, float scale)
    : Texture(std::move(texture)), scale(scale) {}

EmissiveTexture::EmissiveTexture(Texture&& texture, const glm::vec3& color)
    : Texture(std::move(texture)), color(color) {}

MetallicRoughnessTexture::MetallicRoughnessTexture(Texture&& texture,
                                                   float metallicFactor,
                                                   float roughnessFactor)
    : Texture(std::move(texture)),
      metallicFactor(metallicFactor),
      roughnessFactor(roughnessFactor) {}

OcclusionTexture::OcclusionTexture(Texture&& texture, float strength)
    : Texture(std::move(texture)), strength(strength) {}

BaseColorTexture::BaseColorTexture(Texture&& texture, const glm::vec4& color)
    : Texture(std::move(texture)), color(color) {}

RenderTexturesContainer::~RenderTexturesContainer() noexcept { Clear(); }

RenderTexturesContainer::RenderTexturesContainer(
    RenderTexturesContainer&& other) noexcept
    : cubeMapTextureId(std::exchange(other.cubeMapTextureId, 0)),
      irradianceMapTextureId(std::exchange(other.irradianceMapTextureId, 0)),
      brdfLutTextureId(std::exchange(other.brdfLutTextureId, 0)),
      hdrTextureId(std::exchange(other.hdrTextureId, 0)),
      fboId(std::exchange(other.fboId, 0)),
      rboId(std::exchange(other.rboId, 0)),
      cubeVao(std::exchange(other.cubeVao, 0)),
      cubeVbo(std::exchange(other.cubeVbo, 0)),
      quadVao(std::exchange(other.quadVao, 0)),
      quadVbo(std::exchange(other.quadVbo, 0)),
      textures(std::move(other.textures)),
      eq2CubeMapShader(std::move(other.eq2CubeMapShader)),
      brdfLutShader(std::move(other.brdfLutShader)),
      envMapShader(std::move(other.envMapShader)) {}

RenderTexturesContainer& RenderTexturesContainer::operator=(
    RenderTexturesContainer&& other) noexcept {
  if (this != &other) {
    Clear();
    cubeMapTextureId = std::exchange(other.cubeMapTextureId, 0);
    irradianceMapTextureId = std::exchange(other.irradianceMapTextureId, 0);
    brdfLutTextureId = std::exchange(other.brdfLutTextureId, 0);
    hdrTextureId = std::exchange(other.hdrTextureId, 0);
    fboId = std::exchange(other.fboId, 0);
    rboId = std::exchange(other.rboId, 0);
    cubeVao = std::exchange(other.cubeVao, 0);
    cubeVbo = std::exchange(other.cubeVbo, 0);
    quadVao = std::exchange(other.quadVao, 0);
    quadVbo = std::exchange(other.quadVbo, 0);
    textures = std::move(other.textures);
    eq2CubeMapShader = std::move(other.eq2CubeMapShader);
    brdfLutShader = std::move(other.brdfLutShader);
    envMapShader = std::move(other.envMapShader);
  }
  return *this;
}

RenderTexturesContainer::RenderTexturesContainer(
    const std::pair<std::string, std::string>& eq2CubeMapShaderPaths,
    const std::pair<std::string, std::string>& brdfLutShaderPaths,
    const std::pair<std::string, std::string>& envMapShaderPaths)
    : eq2CubeMapShader(OpenGlShader(eq2CubeMapShaderPaths)),
      brdfLutShader(OpenGlShader(brdfLutShaderPaths)),
      envMapShader(OpenGlShader(envMapShaderPaths)) {}

RenderTexturesContainer::RenderTexturesContainer(
    const std::pair<std::string_view, std::string_view>& eq2CubeMapShaderPaths,
    const std::pair<std::string_view, std::string_view>& brdfLutShaderPaths,
    const std::pair<std::string_view, std::string_view>& envMapShaderPaths)
    : RenderTexturesContainer(
          std::pair<std::string, std::string>{
              std::string(eq2CubeMapShaderPaths.first),
              std::string(eq2CubeMapShaderPaths.second)},
          std::pair<std::string, std::string>{
              std::string(brdfLutShaderPaths.first),
              std::string(brdfLutShaderPaths.second)},
          std::pair<std::string, std::string>{
              std::string(envMapShaderPaths.first),
              std::string(envMapShaderPaths.second)}) {}

void RenderTexturesContainer::Clear() {
  auto delTex = [](uint32_t& texId) {
    if (texId > 0) {
      glDeleteTextures(1, &texId);
      texId = 0;
    }
  };
  auto delFbo = [](uint32_t& fboId) {
    if (fboId > 0) {
      glDeleteFramebuffers(1, &fboId);
      fboId = 0;
    }
  };
  auto delRbo = [](uint32_t& rboId) {
    if (rboId > 0) {
      glDeleteRenderbuffers(1, &rboId);
      rboId = 0;
    }
  };
  auto delVao = [](uint32_t& vaoId) {
    if (vaoId > 0) {
      glDeleteVertexArrays(1, &vaoId);
      vaoId = 0;
    }
  };
  auto delVbo = [](uint32_t& vboId) {
    if (vboId > 0) {
      glDeleteBuffers(1, &vboId);
      vboId = 0;
    }
  };

  delTex(cubeMapTextureId);
  delTex(irradianceMapTextureId);
  delTex(brdfLutTextureId);
  delTex(hdrTextureId);
  delFbo(fboId);
  delRbo(rboId);
  delVao(cubeVao);
  delVao(quadVao);
  delVbo(cubeVbo);
  delVbo(quadVbo);

  eq2CubeMapShader.Clear();
  brdfLutShader.Clear();
  envMapShader.Clear();

  std::cout << "RenderTexturesContainer cleared\n";
}

}  // namespace Mgtt::Rendering
