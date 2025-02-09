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

#include <texture.h>

/**
 * @brief Constructor
 */
Mgtt::Rendering::TextureBase::TextureBase() {
  this->width = 0;
  this->height = 0;
  this->nrComponents = 0;
  this->data = nullptr;
  this->sizeInBytes = 0;
}

/**
 * @brief Constructor
 **/
Mgtt::Rendering::Texture::Texture() : TextureBase(), id(0) {}

/**
 * @brief Copy Constructor
 *
 * @param texture The texture to be associated with this structure.
 */
Mgtt::Rendering::Texture::Texture(const Texture& texture) {
  this->name = texture.name;
  this->id = texture.id;
  this->path = texture.path;
  this->width = texture.width;
  this->height = texture.height;
  this->nrComponents = texture.nrComponents;
  this->data = texture.data;
  this->sizeInBytes = texture.sizeInBytes;
}

/**
 * @brief This method clears the resources associated with the Texture object,
 * freeing up memory. It is recommended to call this method when the Texture is
 * no longer needed.
 */
void Mgtt::Rendering::Texture::Clear() {
  if (this->id > 0) {
    glDeleteTextures(1, &this->id);
    this->id = 0;
  }
}

/**
 * @brief Constructor
 */
Mgtt::Rendering::NormalTexture::NormalTexture() { this->scale = 1.0f; }

/**
 * @brief Constructor
 *
 * @param texture The normal texture to be associated with this structure.
 * @param scale The scale factor applied to the normal texture.
 */
Mgtt::Rendering::NormalTexture::NormalTexture(const Texture& texture,
                                              float scale)
    : Texture(texture), scale(scale) {}

/**
 * @brief Constructor
 */
Mgtt::Rendering::EmissiveTexture::EmissiveTexture() {
  this->color = glm::vec3(0.0f);
}

/**
 * @brief Constructor
 *
 * @param texture The emissive texture to be associated with this structure.
 * @param scale The emissive color applied to the normal texture.
 */
Mgtt::Rendering::EmissiveTexture::EmissiveTexture(const Texture& texture,
                                                  const glm::vec3& color)
    : Texture(texture), color(color) {}

/**
 * @brief Constructor
 */
Mgtt::Rendering::MetallicRoughnessTexture::MetallicRoughnessTexture() {
  this->metallicFactor = 0.0f;
  this->roughnessFactor = 1.0f;
}

/**
 * @brief Constructor
 *
 * @param texture The Metallic roughness texture to be associated with this
 * structure.
 * @param metallicFactor The metallic factor applied to the Metallic roughness
 * texture.
 * @param roughnnessFactor The roughness factor applied to the Metallic
 * roughness texture.
 */
Mgtt::Rendering::MetallicRoughnessTexture::MetallicRoughnessTexture(
    const Texture& texture, float metallicFactor, float roughnessFactor)
    : Texture(texture),
      metallicFactor(metallicFactor),
      roughnessFactor(roughnessFactor) {}

/**
 * @brief Constructor
 */
Mgtt::Rendering::OcclusionTexture::OcclusionTexture() { this->strength = 0.0f; }

/**
 * @brief Constructor
 *
 * @param texture The occlusion texture to be associated with this structure.
 * @param scale The occlusion color applied to the occlusion texture.
 */
Mgtt::Rendering::OcclusionTexture::OcclusionTexture(const Texture& texture,
                                                    float strength)
    : Texture(texture), strength(strength) {}

/**
 * @brief Constructor
 */
Mgtt::Rendering::BaseColorTexture::BaseColorTexture() {
  this->color = glm::vec4(1.0f);
}

/**
 * @brief Constructor
 *
 * @param texture The base color texture to be associated with this structure.
 * @param scale The base color applied to the base color texture.
 */
Mgtt::Rendering::BaseColorTexture::BaseColorTexture(const Texture& texture,
                                                    const glm::vec4& color)
    : Texture(texture), color(color) {}

/**
 * @brief Constructor
 */
Mgtt::Rendering::RenderTexturesContainer::RenderTexturesContainer()
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

/**
 * @brief Constructor
 *
 * @param eq2CubeMapshaderPaths The equirectangular to cube map vertex and
 * fragment shader path
 * @param eq2CubeMapshaderPaths The brdf lut vertex and fragment shader path
 * @param envMapshaderPaths The env map vertex and fragment shader path
 */
Mgtt::Rendering::RenderTexturesContainer::RenderTexturesContainer(
    const std::pair<std::string, std::string>& eq2CubeMapshaderPaths,
    const std::pair<std::string, std::string>& brdfLutshaderPaths,
    const std::pair<std::string, std::string>& envMapshaderPaths)
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
      eq2CubeMapShader(OpenGlShader(eq2CubeMapshaderPaths)),
      brdfLutShader(OpenGlShader(brdfLutshaderPaths)),
      envMapShader(OpenGlShader(envMapshaderPaths)) {}

/**
 * @brief This method clears the resources associated with the
 * RenderTextureContainer object, freeing up memory. It is recommended to call
 * this method when the RenderTextureContainer is no longer needed.
 */
void Mgtt::Rendering::RenderTexturesContainer::Clear() {
  if (this->cubeMapTextureId > 0) {
    glDeleteTextures(1, &this->cubeMapTextureId);
    this->cubeMapTextureId = 0;
  }
  if (this->irradianceMapTextureId > 0) {
    glDeleteTextures(1, &this->irradianceMapTextureId);
    this->irradianceMapTextureId = 0;
  }
  if (this->brdfLutTextureId > 0) {
    glDeleteTextures(1, &this->brdfLutTextureId);
    this->brdfLutTextureId = 0;
  }
  if (this->hdrTextureId > 0) {
    glDeleteTextures(1, &this->hdrTextureId);
    this->hdrTextureId = 0;
  }
  if (this->fboId > 0) {
    glDeleteFramebuffers(1, &this->fboId);
    this->fboId = 0;
  }
  if (this->rboId > 0) {
    glDeleteRenderbuffers(1, &this->rboId);
    this->rboId = 0;
  }

  if (this->cubeVao > 0) {
    glDeleteVertexArrays(1, &this->cubeVao);
    this->cubeVao = 0;
  }
  if (this->quadVao > 0) {
    glDeleteVertexArrays(1, &this->quadVao);
    this->quadVao = 0;
  }

  if (this->cubeVbo > 0) {
    glDeleteBuffers(1, &this->cubeVbo);
    this->cubeVbo = 0;
  }
  if (this->quadVbo > 0) {
    glDeleteBuffers(1, &this->quadVbo);
    this->quadVbo = 0;
  }

  this->eq2CubeMapShader.Clear();
  this->brdfLutShader.Clear();
  this->envMapShader.Clear();

#ifdef __ANDROID__
  __android_log_write(ANDROID_LOG_INFO, "CLEAR INFO",
                      "Successfully deleted the render texture container with "
                      "IBL related components");
#else
  std::cout << "Successfully deleted the render texture container "
               "with IBL related components"
            << std::endl;
#endif
}
