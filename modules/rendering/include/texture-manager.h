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

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <GL/glew.h>
#endif>

#include <stb_image.h>
#include <texture.h>
#include <utils.h>

#include <iostream>
#include <string>
#include <vector>

namespace Mgtt::Rendering {
/**
 * @brief Class for importing and managing various textures.
 *
 * @note Take a read on: https://learnopengl.com/PBR/IBL/Diffuse-irradiance
 */
class TextureManager {
 public:
  /**
   * @brief Load cube map textures from the given folder path.
   *
   * @param container The RenderTexturesContainer to associate with the loaded
   * cube map.
   * @param texturePathes The cube map texture pathes
   */
  void LoadFromEnvMap(Mgtt::Rendering::RenderTexturesContainer& container,
                      const std::vector<std::string>& texturePathes);

  /**
   * @brief Load an HDR texture from the given file path.
   *
   * @param container The RenderTexturesContainer to associate with the loaded
   * HDR texture.
   * @param texturePath The file path to the HDR texture.
   */
  void LoadFromHdr(Mgtt::Rendering::RenderTexturesContainer& container,
                   const std::string& texturePath);

  /**
   * @brief Load the BRDF Lookup Texture into the provided HDR texture
   * container.
   *
   * @param container The RenderTexturesContainer to associate with the loaded
   * BRDF texture.
   */
  void LoadBrdfLut(Mgtt::Rendering::RenderTexturesContainer& container);

  /**
   * @brief Clear the provided render textures container.
   *
   * @param container The RenderTexturesContainer to clear.
   */
  void Clear(Mgtt::Rendering::RenderTexturesContainer& container);

 private:
  /**
   * @brief Set up rendering resources for a cube.
   *
   * @param container A reference to a RenderTexturesContainer used to manage
   *                  rendering-related textures and resources.
   */
  void SetupCube(Mgtt::Rendering::RenderTexturesContainer& container);

  /**
   * @brief Set up rendering resources for a cube.
   *
   * @param container A reference to a RenderTexturesContainer used to manage
   *                  rendering-related textures and resources.
   */
  void SetupQuad(Mgtt::Rendering::RenderTexturesContainer& container);

  /**
   * @brief Generate the irradiance map for the provided HDR texture container.
   *
   * @param container The RenderTexturesContainer to associate with the
   * generated irradiance map.
   */
  void GenerateIrradianceMap(
      Mgtt::Rendering::RenderTexturesContainer& container);
  ;
};
}  // namespace Mgtt::Rendering
