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

#include <GL/glew.h>
#include <mesh-primitive.h>
#include <aabb.h>
#include <opengl-shader.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <string>
#include <vector>

namespace Mgtt::Rendering {
struct MeshPrimitive;
/**
 * @brief Represents a mesh in the scene.
 */
struct Mesh {
  /**
   * @brief Constructor for the Mesh struct.
   */
  Mesh();

  ~Mesh() {}

  /**
   * @brief Clear releases resources.
   */
  void Clear();

  std::string name;
  std::vector<Mgtt::Rendering::MeshPrimitive> meshPrimitives;
  std::vector<unsigned int> indices;
  std::vector<glm::vec3> vertexPositionAttribs;
  std::vector<glm::vec3> vertexNormalAttribs;
  std::vector<glm::vec2> vertexTextureAttribs;
  std::vector<glm::ivec4> vertexJointAttribs;
  std::vector<glm::vec4> vertexWeightAttribs;
  glm::mat4 matrix;

  uint32_t vao;
  uint32_t ebo;
  uint32_t pos;
  uint32_t normal;
  uint32_t tex;
  // uint32_t joint;
  // uint32_t weight;

  Mgtt::Rendering::AABB aabb;
};
}  // namespace Mgtt::Rendering
