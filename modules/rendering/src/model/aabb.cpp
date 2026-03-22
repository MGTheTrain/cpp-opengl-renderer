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

#include <aabb.h>

namespace Mgtt::Rendering {

void AABB::CalculateBoundingBox(const glm::mat4& mat) {
  glm::vec3 minCorner = glm::vec3(mat[3]);
  glm::vec3 maxCorner = minCorner;

  const glm::vec3 kRight = glm::vec3(mat[0]);
  glm::vec3 vec0 = kRight * min.x;
  glm::vec3 vec1 = kRight * max.x;
  minCorner += glm::min(vec0, vec1);
  maxCorner += glm::max(vec0, vec1);

  const glm::vec3 kUpDir = glm::vec3(mat[1]);
  vec0 = kUpDir * min.y;
  vec1 = kUpDir * max.y;
  minCorner += glm::min(vec0, vec1);
  maxCorner += glm::max(vec0, vec1);

  const glm::vec3 kBack = glm::vec3(mat[2]);
  vec0 = kBack * min.z;
  vec1 = kBack * max.z;
  minCorner += glm::min(vec0, vec1);
  maxCorner += glm::max(vec0, vec1);

  min = minCorner;
  max = maxCorner;
}

}  // namespace Mgtt::Rendering
