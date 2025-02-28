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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <string>
#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include <opengl-shader.h>

#include <glm/gtx/quaternion.hpp>
#include <map>

namespace Mgtt::Rendering {
/**
 * @brief Represents a collidable aabb
 */
struct AABB {
  /**
   * @brief Constructor for the AABB struct.
   */
  AABB();

  ~AABB() = default;

  glm::vec3 min;
  glm::vec3 max;
  glm::vec3 center;
  float scale;

  /**
   * @brief CalculateBoundingBox calculates the bounding box of an object
   * transformed by the given matrix.
   *
   * @param m A 4x4 transformation matrix representing the object's
   * transformation.
   */
  void CalculateBoundingBox(const glm::mat4& m);
};
}  // namespace Mgtt::Rendering
