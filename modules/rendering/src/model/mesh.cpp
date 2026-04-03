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

#include <mesh.h>

namespace Mgtt::Rendering {

Mesh::~Mesh() noexcept { Clear(); }

Mesh::Mesh(Mesh&& other) noexcept
    : name(std::move(other.name)),
      meshPrimitives(std::move(other.meshPrimitives)),
      indices(std::move(other.indices)),
      vertexPositionAttribs(std::move(other.vertexPositionAttribs)),
      vertexNormalAttribs(std::move(other.vertexNormalAttribs)),
      vertexTextureAttribs(std::move(other.vertexTextureAttribs)),
      vertexJointAttribs(std::move(other.vertexJointAttribs)),
      vertexWeightAttribs(std::move(other.vertexWeightAttribs)),
      matrix(other.matrix),
      vao(std::exchange(other.vao, 0)),
      ebo(std::exchange(other.ebo, 0)),
      pos(std::exchange(other.pos, 0)),
      normal(std::exchange(other.normal, 0)),
      tex(std::exchange(other.tex, 0)),
      aabb(other.aabb) {}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
  if (this != &other) {
    Clear();
    name = std::move(other.name);
    meshPrimitives = std::move(other.meshPrimitives);
    indices = std::move(other.indices);
    vertexPositionAttribs = std::move(other.vertexPositionAttribs);
    vertexNormalAttribs = std::move(other.vertexNormalAttribs);
    vertexTextureAttribs = std::move(other.vertexTextureAttribs);
    vertexJointAttribs = std::move(other.vertexJointAttribs);
    vertexWeightAttribs = std::move(other.vertexWeightAttribs);
    matrix = other.matrix;
    vao = std::exchange(other.vao, 0);
    ebo = std::exchange(other.ebo, 0);
    pos = std::exchange(other.pos, 0);
    normal = std::exchange(other.normal, 0);
    tex = std::exchange(other.tex, 0);
    aabb = other.aabb;
  }
  return *this;
}

void Mesh::Clear() {
  auto delBuf = [](uint32_t& bufId) {
    if (bufId > 0) {
      glDeleteBuffers(1, &bufId);
      bufId = 0;
    }
  };

  delBuf(pos);
  delBuf(normal);
  delBuf(tex);
  delBuf(ebo);

  if (vao > 0) {
    glDeleteVertexArrays(1, &vao);
    vao = 0;
  }

  for (auto& primitive : meshPrimitives) {
    primitive.Clear();
  }

  meshPrimitives.clear();
  indices.clear();
  vertexPositionAttribs.clear();
  vertexNormalAttribs.clear();
  vertexTextureAttribs.clear();
  vertexJointAttribs.clear();
  vertexWeightAttribs.clear();

  matrix = glm::mat4(1.0f);
  name.clear();
}

}  // namespace Mgtt::Rendering
