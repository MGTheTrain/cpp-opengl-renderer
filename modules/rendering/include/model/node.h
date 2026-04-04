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

#include <mesh.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <memory>
#include <string>
#include <vector>

namespace Mgtt::Rendering {

struct Mesh;

/**
 * @brief Represents a node in the scene hierarchy.
 */
struct Node {
  Node() = default;
  ~Node() = default;

  Node(const Node&) = delete;
  Node& operator=(const Node&) = delete;
  Node(Node&&) = default;
  Node& operator=(Node&&) = default;

  void Clear();

  /**
   * @brief Compute the local transformation matrix from pos, rot, scale and
   * matrix.
   */
  [[nodiscard]] glm::mat4 LocalMatrix() const;

  /**
   * @brief Compute the world-space transformation by walking up the parent
   * chain.
   */
  [[nodiscard]] glm::mat4 GetGlobalMatrix() const;

  /**
   * @brief Propagate the global transform to mesh->matrix recursively.
   */
  void InitialTransform();

  std::string name;
  std::weak_ptr<Mgtt::Rendering::Node> parent;
  std::shared_ptr<Mgtt::Rendering::Mesh> mesh;
  std::vector<std::shared_ptr<Mgtt::Rendering::Node>> children;

  glm::vec3 pos{0.0f};
  glm::quat rot{1.0f, 0.0f, 0.0f, 0.0f};
  glm::vec3 scale{1.0f};
  glm::mat4 matrix{1.0f};
  uint32_t index{0};
};

}  // namespace Mgtt::Rendering
