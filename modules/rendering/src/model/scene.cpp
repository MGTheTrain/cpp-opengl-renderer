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

#include <scene.h>

#include <iostream>

namespace Mgtt::Rendering {

Scene::~Scene() noexcept { Clear(); }

Scene::Scene(Scene&& other) noexcept
    : name(std::move(other.name)),
      path(std::move(other.path)),
      pos(other.pos),
      rot(other.rot),
      scale(other.scale),
      mvp(other.mvp),
      matrix(other.matrix),
      textureMap(std::move(other.textureMap)),
      nodes(std::move(other.nodes)),
      linearNodes(std::move(other.linearNodes)),
      materials(std::move(other.materials)),
      aabb(other.aabb),
      shader(std::move(other.shader)) {}

Scene& Scene::operator=(Scene&& other) noexcept {
  if (this != &other) {
    Clear();
    name = std::move(other.name);
    path = std::move(other.path);
    pos = other.pos;
    rot = other.rot;
    scale = other.scale;
    mvp = other.mvp;
    matrix = other.matrix;
    textureMap = std::move(other.textureMap);
    nodes = std::move(other.nodes);
    linearNodes = std::move(other.linearNodes);
    materials = std::move(other.materials);
    aabb = other.aabb;
    shader = std::move(other.shader);
  }
  return *this;
}

void Scene::Clear() {
  for (auto& node : nodes) {
    Linearize(node);
  }
  for (auto& linearNode : linearNodes) {
    linearNode->Clear();
  }

  textureMap.clear();

  nodes.clear();
  nodes.shrink_to_fit();

  linearNodes.clear();
  linearNodes.shrink_to_fit();

  materials.clear();
  materials.shrink_to_fit();

  shader.Clear();

  std::cout << "Successfully cleared scene with all it's components " << path
            << '\n';

  name.clear();
  path.clear();
  pos = glm::vec3(0.0f);
  rot = glm::vec3(0.0f);
  scale = 1.0;
  mvp = glm::mat4(1.0f);
  matrix = glm::mat4(1.0f);

  aabb.min = glm::vec3(FLT_MAX);
  aabb.max = glm::vec3(-FLT_MAX);
  aabb.center = glm::vec3(0.0f);
  aabb.scale = 1.0f;
}

void Scene::Linearize(const std::shared_ptr<Mgtt::Rendering::Node>& node) {
  linearNodes.push_back(node);
  for (const auto& child : node->children) {
    Linearize(child);
  }
}

}  // namespace Mgtt::Rendering
