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
    //this->joint = 0;
    //this->weight = 0;
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
    for(auto mehsPrimitive: this->meshPrimitives) {
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