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
#include <result.h>
#include <stb_image.h>
#include <texture.h>
#include <utils.h>

#include <string>
#include <string_view>
#include <vector>

namespace Mgtt::Rendering {

/**
 * @brief Manages GPU texture resources for IBL and environment mapping.
 *
 * @note Reference: https://learnopengl.com/PBR/IBL/Diffuse-irradiance
 */
class TextureManager {
 public:
  TextureManager() noexcept = default;
  ~TextureManager() noexcept = default;
  TextureManager(const TextureManager&) = delete;
  TextureManager& operator=(const TextureManager&) = delete;
  TextureManager(TextureManager&&) = delete;
  TextureManager& operator=(TextureManager&&) = delete;

  /**
   * @brief Upload cube map faces from individual image files.
   *
   * @param container Target container for the loaded cube map.
   * @param texturePaths Ordered list of six face image paths.
   * @return Ok on success, Err on the first path that fails to load.
   */
  [[nodiscard]] Mgtt::Common::Result<void> LoadFromEnvMap(
      Mgtt::Rendering::RenderTexturesContainer& container,
      const std::vector<std::string>& texturePaths);

  /**
   * @brief Load an equirectangular HDR image and convert it to a cube map.
   *
   * @param container Target container; eq2CubeMapShader must already be
   *                  compiled before calling this.
   * @param texturePath Path to the .hdr file.
   * @return Ok on success, Err with a descriptive message on failure.
   */
  [[nodiscard]] Mgtt::Common::Result<void> LoadFromHdr(
      Mgtt::Rendering::RenderTexturesContainer& container,
      std::string_view texturePath);

  /**
   * @brief Render the BRDF look-up texture into the container's FBO.
   *
   * @param container Target container; brdfLutShader and FBO must already
   *                  exist before calling this.
   * @return Ok on success, Err if the shader program is missing.
   */
  [[nodiscard]] Mgtt::Common::Result<void> LoadBrdfLut(
      Mgtt::Rendering::RenderTexturesContainer& container);

  /**
   * @brief Release all GL resources held by the container.
   *
   * @param container Container to clear.
   */
  void Clear(Mgtt::Rendering::RenderTexturesContainer& container) noexcept;

 private:
  /**
   * @brief Allocate and draw a unit cube into the container's VAO/VBO.
   *
   * Idempotent: skips allocation if the VAO/VBO are already non-zero.
   *
   * @param container Container whose cubeVao/cubeVbo will be used.
   */
  void SetupCube(Mgtt::Rendering::RenderTexturesContainer& container);

  /**
   * @brief Allocate and draw a full-screen quad into the container's VAO/VBO.
   *
   * Idempotent: skips allocation if the VAO/VBO are already non-zero.
   *
   * @param container Container whose quadVao/quadVbo will be used.
   */
  void SetupQuad(Mgtt::Rendering::RenderTexturesContainer& container);

  /**
   * @brief Convolve the cube map into a diffuse irradiance map.
   *
   * @param container Container whose cubeMapTextureId will be used as source.
   */
  void GenerateIrradianceMap(
      Mgtt::Rendering::RenderTexturesContainer& container);
};

}  // namespace Mgtt::Rendering
