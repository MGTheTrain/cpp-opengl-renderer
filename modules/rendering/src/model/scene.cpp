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