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

#include <node.h>

namespace Mgtt::Rendering {

void Node::Clear() {
  if (mesh) {
    mesh->Clear();
  }
  index = 0;
  pos = glm::vec3(0.0f);
  scale = glm::vec3(1.0f);
  matrix = glm::mat4(1.0f);
}

glm::mat4 Node::LocalMatrix() const {
  return glm::translate(glm::mat4(1.0f), pos) * glm::toMat4(rot) *
         glm::scale(glm::mat4(1.0f), scale) * matrix;
}

glm::mat4 Node::GetGlobalMatrix() const {
  glm::mat4 globalMatrix = LocalMatrix();
  std::shared_ptr<Mgtt::Rendering::Node> currentParent = parent;
  while (currentParent != nullptr) {
    globalMatrix = currentParent->LocalMatrix() * globalMatrix;
    currentParent = currentParent->parent;
  }
  return globalMatrix;
}

void Node::InitialTransform() {
  if (mesh) {
    mesh->matrix = GetGlobalMatrix();
  }
  for (auto& child : children) {
    child->InitialTransform();
  }
}

}  // namespace Mgtt::Rendering
