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

#include <mesh.h>

/**
 * @brief Constructor for the Mesh structure.
 */
Mgtt::Rendering::Mesh::Mesh() {
  this->matrix = glm::mat4(1.0f);
  this->name = "";
  this->vao = 0;
  this->ebo = 0;
  this->pos = 0;
  this->normal = 0;
  this->tex = 0;
  // this->joint = 0;
  // this->weight = 0;
  this->matrix = glm::mat4(1.0f);
}

/**
 * @brief Clear releases resources.
 */
void Mgtt::Rendering::Mesh::Clear() {
  if (this->pos > 0) {
    glDeleteBuffers(1, &this->pos);
    this->pos = 0;
  }
  if (this->normal > 0) {
    glDeleteBuffers(1, &this->normal);
    this->normal = 0;
  }
  if (this->tex > 0) {
    glDeleteBuffers(1, &this->tex);
    this->tex = 0;
  }
  if (this->ebo > 0) {
    glDeleteBuffers(1, &this->ebo);
    this->ebo = 0;
  }
  if (this->vao > 0) {
    glDeleteVertexArrays(1, &this->vao);
    this->vao = 0;
  }
  for (auto mehsPrimitive : this->meshPrimitives) {
    mehsPrimitive.Clear();
  }
  this->meshPrimitives.clear();
  this->indices.clear();
  this->vertexPositionAttribs.clear();
  this->vertexNormalAttribs.clear();
  this->vertexTextureAttribs.clear();
  this->vertexJointAttribs.clear();
  this->vertexWeightAttribs.clear();

  this->matrix = glm::mat4(1.0f);
  this->name = "";
}
