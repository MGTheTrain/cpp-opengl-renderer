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

TextureBase::TextureBase()
    : width(0), height(0), nrComponents(0), data(nullptr), sizeInBytes(0) {}

Texture::Texture() : TextureBase(), id(0) {}

Texture::Texture(const Texture& other) : TextureBase(other), id(other.id) {}

void Texture::Clear() {
  if (id > 0) {
    glDeleteTextures(1, &id);
    id = 0;
  }
}

NormalTexture::NormalTexture() : scale(1.0f) {}

NormalTexture::NormalTexture(const Texture& texture, float scale)
    : Texture(texture), scale(scale) {}

EmissiveTexture::EmissiveTexture() : color(0.0f) {}

EmissiveTexture::EmissiveTexture(const Texture& texture, const glm::vec3& color)
    : Texture(texture), color(color) {}

MetallicRoughnessTexture::MetallicRoughnessTexture()
    : metallicFactor(0.0f), roughnessFactor(1.0f) {}

MetallicRoughnessTexture::MetallicRoughnessTexture(const Texture& texture,
                                                   float metallicFactor,
                                                   float roughnessFactor)
    : Texture(texture),
      metallicFactor(metallicFactor),
      roughnessFactor(roughnessFactor) {}

OcclusionTexture::OcclusionTexture() : strength(0.0f) {}

OcclusionTexture::OcclusionTexture(const Texture& texture, float strength)
    : Texture(texture), strength(strength) {}

BaseColorTexture::BaseColorTexture() : color(1.0f) {}

BaseColorTexture::BaseColorTexture(const Texture& texture,
                                   const glm::vec4& color)
    : Texture(texture), color(color) {}

RenderTexturesContainer::RenderTexturesContainer()
    : cubeMapTextureId(0),
      irradianceMapTextureId(0),
      brdfLutTextureId(0),
      hdrTextureId(0),
      fboId(0),
      rboId(0),
      cubeVao(0),
      cubeVbo(0),
      quadVao(0),
      quadVbo(0) {}

RenderTexturesContainer::RenderTexturesContainer(
    const std::pair<std::string, std::string>& eq2CubeMapShaderPaths,
    const std::pair<std::string, std::string>& brdfLutShaderPaths,
    const std::pair<std::string, std::string>& envMapShaderPaths)
    : cubeMapTextureId(0),
      irradianceMapTextureId(0),
      brdfLutTextureId(0),
      hdrTextureId(0),
      fboId(0),
      rboId(0),
      cubeVao(0),
      cubeVbo(0),
      quadVao(0),
      quadVbo(0),
      eq2CubeMapShader(OpenGlShader(eq2CubeMapShaderPaths)),
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
  auto delTex = [](uint32_t& id) {
    if (id > 0) {
      glDeleteTextures(1, &id);
      id = 0;
    }
  };
  auto delFbo = [](uint32_t& id) {
    if (id > 0) {
      glDeleteFramebuffers(1, &id);
      id = 0;
    }
  };
  auto delRbo = [](uint32_t& id) {
    if (id > 0) {
      glDeleteRenderbuffers(1, &id);
      id = 0;
    }
  };
  auto delVao = [](uint32_t& id) {
    if (id > 0) {
      glDeleteVertexArrays(1, &id);
      id = 0;
    }
  };
  auto delVbo = [](uint32_t& id) {
    if (id > 0) {
      glDeleteBuffers(1, &id);
      id = 0;
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
