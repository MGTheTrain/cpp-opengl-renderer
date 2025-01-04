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

#include <iostream>
#include <string>
#include <vector>

namespace Mgtt::Rendering {
/**
 * @brief Class for importing and managing various textures.
 *
 * The TextureManager class provides functionality for loading and managing
 * different textures, including cube maps and High Dynamic Range (HDR)
 * textures.
 * @note Take a read on: https://learnopengl.com/PBR/IBL/Diffuse-irradiance
 */
class TextureManager {
 public:
  /**
   * @brief Load cube map textures from the given folder path.
   *
   * This function loads cube map textures and
   * associates it with the provided RenderTexturesContainer. It performs
   * necessary operations to make the textures available for use in rendering.
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
   * This function loads an HDR texture from the specified file path and
   * associates it with the provided RenderTexturesContainer. It performs
   * necessary operations to make the texture available for use in rendering.
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
   * This function loads the BRDF Lookup Texture into the provided HDR texture
   * container. It performs necessary operations to make the BRDF texture
   * available for use in rendering.
   *
   * @param container The RenderTexturesContainer to associate with the loaded
   * BRDF texture.
   */
  void LoadBrdfLut(Mgtt::Rendering::RenderTexturesContainer& container);

  /**
   * @brief Clear the provided render textures container.
   *
   * This function clears the contents of the provided RenderTexturesContainer,
   * releasing any resources associated with the textures.
   *
   * @param container The RenderTexturesContainer to clear.
   */
  void Clear(Mgtt::Rendering::RenderTexturesContainer& container);

 private:
  /**
   * @brief Check if any value in the given vector is greater than zero.
   *
   * The function iterates through the elements of the vector and returns true
   * if it finds at least one element greater than zero; otherwise, it returns
   * false.
   *
   * @param vec A const reference to a vector of unsigned integers to be
   * checked.
   * @return true if any value in the vector is greater than zero, false
   * otherwise.
   */
  bool HasValuesGreaterThanZero(const std::vector<unsigned int>& vec);

  /**
   * @brief Set up rendering resources for a cube.
   *
   * The SetupCube function initializes and configures rendering resources
   * for a cube, including textures and buffers. It uses the provided
   * RenderTexturesContainer to manage the associated textures.
   *
   * @param container A reference to a RenderTexturesContainer used to manage
   *                  rendering-related textures and resources.
   */
  void SetupCube(Mgtt::Rendering::RenderTexturesContainer& container);

  /**
   * @brief Set up rendering resources for a cube.
   *
   * The SetupQuad function initializes and configures rendering resources
   * for a quad, including textures and buffers. It uses the provided
   * RenderTexturesContainer to manage the associated textures.
   *
   * @param container A reference to a RenderTexturesContainer used to manage
   *                  rendering-related textures and resources.
   */
  void SetupQuad(Mgtt::Rendering::RenderTexturesContainer& container);

  /**
   * @brief Generate the irradiance map for the provided HDR texture container.
   *
   * This function generates the irradiance map for the provided HDR texture
   * container. It performs necessary operations to make the irradiance map
   * available for use in rendering.
   *
   * @param container The RenderTexturesContainer to associate with the
   * generated irradiance map.
   */
  void GenerateIrradianceMap(
      Mgtt::Rendering::RenderTexturesContainer& container);
  ;
};
}  // namespace Mgtt::Rendering
