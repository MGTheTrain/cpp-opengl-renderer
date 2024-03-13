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

#include <node.h>

/**
 * @brief Constructor for the Node structure.
 */
Mgtt::Rendering::Node::Node() {
  this->index = 0;
  this->pos = glm::vec3(0.0f);
  // this->rot = glm::quat();
  this->scale = glm::vec3(1.0f);
  this->matrix = glm::mat4(1.0f);
}

/**
 * @brief Clear releases resources.
 */
void Mgtt::Rendering::Node::Clear() {
  this->mesh->Clear();
  this->index = 0;
  this->pos = glm::vec3(0.0f);
  // this->rot = glm::quat();
  this->scale = glm::vec3(1.0f);
  this->matrix = glm::mat4(1.0f);
}

/**
 * @brief Calculates the local transformation matrix of the node.
 * @return Local transformation matrix.
 */
glm::mat4 Mgtt::Rendering::Node::LocalMatrix() {
  return glm::translate(glm::mat4(1.0f), this->pos) * glm::toMat4(this->rot) *
         glm::scale(glm::mat4(1.0f), this->scale) * this->matrix;
}

/**
 * @brief Calculates the global transformation matrix of the node.
 * @return Global transformation matrix.
 */
glm::mat4 Mgtt::Rendering::Node::GetGlobalMatrix() {
  glm::mat4 m = LocalMatrix();
  std::shared_ptr<Mgtt::Rendering::Node> p = this->parent;
  while (p) {
    m = p->LocalMatrix() * m;
    p = p->parent;
  }
  return m;
}

/**
 * @brief Applies the initial transformation to the specified node and its child nodes recursively.
 *
 * This function applies the initial transformation to the given node and its child nodes recursively.
 * It ensures that the transformation is propagated down the hierarchy of nodes, allowing for
 * hierarchical transformations in the scene graph.
 *
 * @param node A shared pointer to the node to which the initial transformation should be applied.
 */
void Mgtt::Rendering::Node::InitialTransform() {
  if (this->mesh) {
    this->mesh->matrix = this->GetGlobalMatrix();
  }
  for (auto & child : children) {
    child->InitialTransform();
  }
}