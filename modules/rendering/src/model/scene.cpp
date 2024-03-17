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

#include <scene.h>

/**
 * @brief Constructor for the Scene structure.
 */
Mgtt::Rendering::Scene::Scene() {
  this->name = "";
  this->path = "";
  this->pos = glm::vec3(0.0f);
  this->rot = glm::vec3(0.0f);
  this->scale = 1.0f;
  this->mvp = glm::mat4(1.0f);
  this->matrix = glm::mat4(1.0f);
}

/**
 * @brief Clear releases resources.
 */
void Mgtt::Rendering::Scene::Clear() {
  for (auto& node : this->nodes) {
    this->Linearize(node);
  }
  for (auto& linearNode : this->linearNodes) {
    linearNode->Clear();
  }

  this->textureMap.clear();
  this->nodes.clear();
  this->nodes.shrink_to_fit();
  this->linearNodes.clear();
  this->linearNodes.shrink_to_fit();
  this->materials.clear();
  this->materials.shrink_to_fit();

  this->shader.Clear();
  std::cout
      << "CLEAR INFO: Successfully cleared scene with all it's components "
      << this->path << std::endl;

  this->name = "";
  this->path = "";
  this->pos = glm::vec3(0.0f);
  this->rot = glm::vec3(0.0f);
  this->scale = 1.0f;
  this->mvp = glm::mat4(1.0f);
  this->matrix = glm::mat4(1.0f);

  this->aabb.min = glm::vec3(FLT_MAX);
  this->aabb.max = glm::vec3(-FLT_MAX);
  this->aabb.center = glm::vec3(0.0f);
  this->aabb.scale = 1.0f;
}

/**
 * @brief Recursively linearizes the scene hierarchy starting from the given
 * node.
 *
 * This method flattens the hierarchical structure of the scene, storing nodes
 * in a linearNodes vector.
 *
 * @param node The starting node to linearize.
 */
void Mgtt::Rendering::Scene::Linearize(
    std::shared_ptr<Mgtt::Rendering::Node> node) {
  this->linearNodes.push_back(node);

  for (uint32_t i = 0; i < node->children.size(); i++) {
    this->Linearize(node->children[i]);
  }
}
