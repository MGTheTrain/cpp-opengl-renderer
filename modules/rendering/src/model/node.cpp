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
    return glm::translate(glm::mat4(1.0f), this->pos) * glm::toMat4(this->rot) * glm::scale(glm::mat4(1.0f), this->scale) * this->matrix;
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