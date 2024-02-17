#include <material.h>

/**
 * @brief Constructor for the Material structure.
 */
Mgtt::Rendering::Material::Material() {
    this->name = "";
}

/**
 * @brief Constructor for the PbrMaterial structure.
 */
Mgtt::Rendering::PbrMaterial::PbrMaterial() {
    this->name = "";
    this->alphaCutoff = 0.0f;
    this->doubleSided = false;
    this->alphaMode = Mgtt::Rendering::AlphaMode::OPAQ;
}

/**
 * @brief Clear releases resources.
 */
void Mgtt::Rendering::PbrMaterial::Clear() {
    this->baseColorTexture.Clear();
    this->metallicRoughnessTexture.Clear();
    this->normalTexture.Clear();
    this->emissiveTexture.Clear();
    this->occlusionTexture.Clear();

    this->name = "";
    this->alphaCutoff = 0.0f;
    this->doubleSided = false;
    this->alphaMode = Mgtt::Rendering::AlphaMode::OPAQ;
}

