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

#pragma once

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <aabb.h>
#include <opengl-shader.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace Mgtt::Rendering {

struct TextureBase {
  TextureBase() = default;
  virtual ~TextureBase() = default;

  std::string name;
  std::string path;
  int32_t width{0};
  int32_t height{0};
  int32_t nrComponents{0};
  unsigned char* data{nullptr};
  uint32_t sizeInBytes{0};
};

struct Texture : public TextureBase {
  Texture() = default;
  virtual ~Texture() = default;
  Texture(const Texture& texture) = default;

  void Clear();

  uint32_t id{0};
};

struct NormalTexture : public Texture {
  NormalTexture() = default;
  ~NormalTexture() = default;
  NormalTexture(const Texture& texture, float scale);
  float scale{1.0f};
};

struct EmissiveTexture : public Texture {
  EmissiveTexture() = default;
  ~EmissiveTexture() = default;
  EmissiveTexture(const Texture& texture, const glm::vec3& color);
  glm::vec3 color{0.0f};
};

struct MetallicRoughnessTexture : public Texture {
  MetallicRoughnessTexture() = default;
  ~MetallicRoughnessTexture() = default;
  MetallicRoughnessTexture(const Texture& texture, float metallicFactor,
                           float roughnessFactor);
  float metallicFactor{0.0f};
  float roughnessFactor{1.0f};
};

struct OcclusionTexture : public Texture {
  OcclusionTexture() = default;
  ~OcclusionTexture() = default;
  OcclusionTexture(const Texture& texture, float strength);
  float strength{0.0f};
};

struct BaseColorTexture : public Texture {
  BaseColorTexture() = default;
  ~BaseColorTexture() = default;
  BaseColorTexture(const Texture& texture, const glm::vec4& color);
  glm::vec4 color{1.0f};
};

/**
 * @brief Container for managing rendering-related textures and IBL resources.
 */
struct RenderTexturesContainer {
  RenderTexturesContainer() = default;
  ~RenderTexturesContainer() = default;

  // Move-only — OpenGlShader members are move-only
  RenderTexturesContainer(RenderTexturesContainer&&) = default;
  RenderTexturesContainer& operator=(RenderTexturesContainer&&) = default;

  RenderTexturesContainer(const RenderTexturesContainer&) = delete;
  RenderTexturesContainer& operator=(const RenderTexturesContainer&) = delete;

  /**
   * @brief Construct and immediately compile shaders from std::string paths.
   */
  RenderTexturesContainer(
      const std::pair<std::string, std::string>& eq2CubeMapShaderPaths,
      const std::pair<std::string, std::string>& brdfLutShaderPaths,
      const std::pair<std::string, std::string>& envMapShaderPaths);

  /**
   * @brief Construct and immediately compile shaders from string_view paths.
   *        Delegates to the std::string overload.
   */
  RenderTexturesContainer(
      const std::pair<std::string_view, std::string_view>&
          eq2CubeMapShaderPaths,
      const std::pair<std::string_view, std::string_view>& brdfLutShaderPaths,
      const std::pair<std::string_view, std::string_view>& envMapShaderPaths);

  /**
   * @brief Release all GL resources.
   */
  void Clear();

  uint32_t cubeMapTextureId{0};
  uint32_t irradianceMapTextureId{0};
  uint32_t brdfLutTextureId{0};
  uint32_t hdrTextureId{0};
  uint32_t fboId{0};
  uint32_t rboId{0};
  uint32_t cubeVao{0};
  uint32_t cubeVbo{0};
  uint32_t quadVao{0};
  uint32_t quadVbo{0};

  // Either a single HDR texture or six cube map face textures
  std::vector<TextureBase> textures;

  Mgtt::Rendering::OpenGlShader eq2CubeMapShader;
  Mgtt::Rendering::OpenGlShader brdfLutShader;
  Mgtt::Rendering::OpenGlShader envMapShader;
};

}  // namespace Mgtt::Rendering
