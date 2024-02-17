#include <mesh-primitive.h>

/**
 * @brief Constructor for the MeshPrimitive structure.
 */
Mgtt::Rendering::MeshPrimitive::MeshPrimitive() {
    this->name = "";
    this->hasSkin = false;
    this->hasIndices = false;
    this->firstIndex = 0;
    this->indexCount = 0;
    this->vertexCount = 0;
}

/**
 * @brief Clear releases resources.
 */
void Mgtt::Rendering::MeshPrimitive::Clear() {
    this->pbrMaterial.Clear();
    this->name = "";
    this->hasSkin = false;
    this->hasIndices = false;
    this->firstIndex = 0;
    this->indexCount = 0;
    this->vertexCount = 0;
}