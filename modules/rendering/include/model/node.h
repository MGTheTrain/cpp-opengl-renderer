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
#include <mesh.h>

#include <glm/gtx/quaternion.hpp>

namespace Mgtt::Rendering {
struct Node;
struct Mesh;
/**
 * @brief Represents a node in the scene hierarchy.
 */
struct Node {
  /**
   * @brief Constructor for the Node struct.
   */
  Node();

  ~Node() = default;

  /**
   * @brief Clear releases resources.
   */
  void Clear();

  /**
   * @brief Calculates the local transformation matrix of the node.
   * @return Local transformation matrix.
   */
  glm::mat4 LocalMatrix();

  /**
   * @brief Calculates the global transformation matrix of the node.
   * @return Global transformation matrix.
   */
  glm::mat4 GetGlobalMatrix();

  /**
   * @brief Applies the initial transformation to the specified node and its
   * child nodes recursively.
   *
   * This function applies the initial transformation to the given node and its
   * child nodes recursively. It ensures that the transformation is propagated
   * down the hierarchy of nodes, allowing for hierarchical transformations in
   * the scene graph.
   *
   * @param node A shared pointer to the node to which the initial
   * transformation should be applied.
   */
  void InitialTransform();

  std::string name;
  std::shared_ptr<Mgtt::Rendering::Node> parent;
  std::shared_ptr<Mgtt::Rendering::Mesh> mesh;
  std::vector<std::shared_ptr<Mgtt::Rendering::Node>> children;
  glm::vec3 pos;
  glm::quat rot;
  glm::vec3 scale;
  glm::mat4 mvp;
  glm::mat4 matrix;
  uint32_t index;
};
}  // namespace Mgtt::Rendering
