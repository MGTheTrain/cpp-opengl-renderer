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
#include <texture.h>

#include <string>

namespace Mgtt::Rendering {
/**
 * @brief Represents a generic material.
 */
struct Material {
  /**
   * @brief Constructor for the Material structure.
   */
  Material() = default;

  /**
   * @brief Virtual destructor for the Material structure.
   */
  virtual ~Material() = default;

  std::string name;
};

enum class AlphaMode { NONE, OPAQ, MASK, BLEND };

/**
 * @brief Represents a physically based rendering (PBR) material.
 */
struct PbrMaterial : public Material {
  /**
   * @brief Constructor for the PbrMaterial structure.
   */
  PbrMaterial();

  ~PbrMaterial() = default;

  /**
   * @brief Clear releases resources.
   */
  void Clear();

  std::string name;
  NormalTexture normalTexture;
  OcclusionTexture occlusionTexture;
  EmissiveTexture emissiveTexture;
  BaseColorTexture baseColorTexture;
  MetallicRoughnessTexture metallicRoughnessTexture;

  float alphaCutoff;
  bool doubleSided;
  AlphaMode alphaMode;
};
}  // namespace Mgtt::Rendering
