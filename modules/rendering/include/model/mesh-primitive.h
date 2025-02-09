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

#include <material.h>
#include <scene.h>

#include <memory>
#include <string>

namespace Mgtt::Rendering {
/**
 * @brief Represents a primitive mesh in the scene.
 */
struct MeshPrimitive {
  /**
   * @brief Constructor for the MeshPrimitive structure.
   */
  MeshPrimitive();

  ~MeshPrimitive() = default;

  /**
   * @brief Clear releases resources.
   */
  void Clear();

  std::string name;
  uint32_t firstIndex;   // NOTE: utilized in glDrawElements(...)
  uint32_t indexCount;   // NOTE: utilized in glDrawElements(...)
  uint32_t vertexCount;  // NOTE: utilized in glDrawElements(...) or
                         // glDrawArrays(...)
  bool hasSkin;
  bool hasIndices;
  Mgtt::Rendering::PbrMaterial pbrMaterial;
  AABB aabb;
};
}  // namespace Mgtt::Rendering
