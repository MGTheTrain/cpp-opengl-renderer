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

#pragma once

#if defined(__EMSCRIPTEN__)
#include <GLES3/gl3.h>
#else
#if defined(__ANDROID__)
#include <GLES2/gl2.h>
#else
#include <GL/glew.h>
#endif
#endif
#ifdef __ANDROID__
#include <android/log.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include <aabb.h>
#include <opengl-shader.h>

#include <glm/gtx/quaternion.hpp>

namespace Mgtt::Rendering {
/**
 * @brief Represents a generic texture base.
 */
struct TextureBase {
  /**
   * @brief Constructor
   */
  TextureBase();

  virtual ~TextureBase() = default;

  std::string name;
  std::string path;
  int32_t width;
  int32_t height;
  int32_t nrComponents;
  unsigned char* data;
  uint32_t sizeInBytes;
};

/**
 * @brief Represents a generic texture.
 */
struct Texture : public TextureBase {
  /**
   * @brief Constructor
   */
  Texture();

  virtual ~Texture() = default;

  /**
   * @brief Copy Constructor
   *
   * @param texture The texture to be associated with this struct.
   */
  Texture(const Texture& texture);

  /**
   * @brief This method clears the resources associated with the Texture object,
   * freeing up memory. It is recommended to call this method when the Texture
   * is no longer needed.
   */
  void Clear();

  uint32_t id;
};

/**
 * @brief Represents a normal map texture.
 */
struct NormalTexture : public Texture {
  /**
   * @brief Constructor
   */
  NormalTexture();

  ~NormalTexture() = default;

  /**
   * @brief Constructor
   *
   * @param texture The normal texture to be associated with this struct.
   * @param scale The scale factor applied to the normal texture.
   */
  NormalTexture(const Texture& texture, float scale);

  float scale;
};

/**
 * @brief Represents an emissive map texture.
 */
struct EmissiveTexture : public Texture {
  /**
   * @brief Constructor
   */
  EmissiveTexture();

  ~EmissiveTexture() = default;

  /**
   * @brief Constructor
   *
   * @param texture The emissive texture to be associated with this struct.
   * @param scale The emissive color applied to the normal texture.
   */
  EmissiveTexture(const Texture& texture, const glm::vec3& color);

  glm::vec3 color;
};

/**
 * @brief Represents a metallic and roughness map texture.
 */
struct MetallicRoughnessTexture : public Texture {
  /**
   * @brief Constructor
   */
  MetallicRoughnessTexture();

  ~MetallicRoughnessTexture() = default;

  /**
   * @brief Constructor
   *
   * @param texture The Metallic roughness texture to be associated with this
   * struct.
   * @param metallicFactor The metallic factor applied to the Metallic roughness
   * texture.
   * @param roughnnessFactor The roughness factor applied to the Metallic
   * roughness texture.
   */
  MetallicRoughnessTexture(const Texture& texture, float metallicFactor,
                           float roughnessFactor);

  float metallicFactor;
  float roughnessFactor;
};

/**
 * @brief Represents an occlusion map texture.
 */
struct OcclusionTexture : public Texture {
  /**
   * @brief Constructor
   */
  OcclusionTexture();

  ~OcclusionTexture() = default;

  /**
   * @brief Constructor
   *
   * @param texture The occlusion texture to be associated with this struct.
   * @param scale The occlusion color applied to the occlusion texture.
   */
  OcclusionTexture(const Texture& texture, float color);

  float strength;
};

/**
 * @brief Represents a base color map texture.
 */
struct BaseColorTexture : public Texture {
  /**
   * @brief Constructor
   */
  BaseColorTexture();

  ~BaseColorTexture() = default;

  /**
   * @brief Constructor
   *
   * @param texture The base color texture to be associated with this struct.
   * @param scale The base color applied to the base color texture.
   */
  BaseColorTexture(const Texture& texture, const glm::vec4& color);

  glm::vec4 color;
};

/**
 * @brief Container for managing rendering-related textures and resources.
 *
 * The RenderTexturesContainer struct encapsulates various texture IDs and
 * rendering resources used in a graphics rendering pipeline.
 */
struct RenderTexturesContainer {
  /**
   * @brief Constructor
   */
  RenderTexturesContainer();

  ~RenderTexturesContainer() = default;

  /**
   * @brief Constructor
   *
   * @param eq2CubeMapshaderPaths The equirectangular to cube map vertex and
   * fragment shader path
   * @param eq2CubeMapshaderPaths The brdf lut vertex and fragment shader path
   * @param envMapshaderPaths The env map vertex and fragment shader path
   */
  RenderTexturesContainer(
      const std::pair<std::string, std::string>& eq2CubeMapshaderPaths,
      const std::pair<std::string, std::string>& brdfLutshaderPaths,
      const std::pair<std::string, std::string>& envMapshaderPaths);

  /**
   * @brief This method clears the resources associated with the
   * RenderTextureContainer object, freeing up memory. It is recommended to call
   * this method when the RenderTextureContainer is no longer needed.
   */
  void Clear();

  uint32_t cubeMapTextureId;
  uint32_t irradianceMapTextureId;
  uint32_t brdfLutTextureId;
  uint32_t hdrTextureId;
  uint32_t fboId;
  uint32_t rboId;
  uint32_t cubeVao;
  uint32_t cubeVbo;
  uint32_t quadVao;
  uint32_t quadVbo;
  std::vector<TextureBase>
      textures;  // NOTE: This vector can be used to store either a single HDR
                 // texture or multiple cube map textures.

  Mgtt::Rendering::OpenGlShader eq2CubeMapShader;
  Mgtt::Rendering::OpenGlShader brdfLutShader;
  Mgtt::Rendering::OpenGlShader envMapShader;
};
}  // namespace Mgtt::Rendering
