#include "model.h" 

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

/**
 * @brief Constructor for the Scene structure.
 */
Mgtt::Rendering::Scene::Scene() {
    
}

/**
 * @brief Constructor for the Node structure.
 */
Mgtt::Rendering::Node::Node() {
    
}

/**
 * @brief Calculates the local transformation matrix of the node.
 * @return Local transformation matrix.
 */
glm::mat4 Mgtt::Rendering::Node::LocalMatrix() {
    
    return glm::mat4(); 
}

/**
 * @brief Calculates the global transformation matrix of the node.
 * @return Global transformation matrix.
 */
glm::mat4 Mgtt::Rendering::Node::GetGlobalMatrix() {
    
    return glm::mat4();
}

/**
 * @brief Constructor for the MeshPrimitive structure.
 */
Mgtt::Rendering::MeshPrimitive::MeshPrimitive() {
    
}

/**
 * @brief Constructor for the Mesh structure.
 */
Mgtt::Rendering::Mesh::Mesh() {
    
}

/**
 * @brief Constructor for the Material structure.
 */
Mgtt::Rendering::Material::Material() {
    
}

/**
 * @brief Constructor for the PbrMaterial structure.
 */
Mgtt::Rendering::PbrMaterial::PbrMaterial() {
    
}

/**
 * @brief Constructor for the Texture structure.
 *
 * This constructor initializes a Texture object with the specified texture path.
 *
 * @param texturePath The file path to the texture.
 Mgtt::Rendering::*/
Mgtt::Rendering::Texture::Texture(const std::string& texturePath) {
    
}

/**
 * @brief Load a texture from the specified file path.
 *
 * This method loads a texture from the given file path and updates the Texture object.
 *
 * @param texturePath The file path to the texture.
 */
void Mgtt::Rendering::Texture::Load(const std::string& texturePath) {
    this->data = stbi_load(texturePath.c_str(),
        &this->width, &this->height, &this->nrComponents, 0);
}

/**
 * @brief Clear the Texture resources.
 *
 * This method clears the resources associated with the Texture object, freeing up memory.
 * It is recommended to call this method when the Texture is no longer needed.
 */
void Mgtt::Rendering::Texture::Clear() {
    stbi_image_free(this->data);
}

/**
* @brief @brief Constructor for the Texture structure
*/
Mgtt::Rendering::Texture::Texture() {

}

/**
 * @brief Constructor for the NormalTexture structure.
 */
Mgtt::Rendering::NormalTexture::NormalTexture() {
    
}

/**
 * @brief Constructor for the EmissiveTexture structure.
 */
Mgtt::Rendering::EmissiveTexture::EmissiveTexture() {
    
}

/**
 * @brief Constructor for the MetallicRoughnessTexture structure.
 */
Mgtt::Rendering::MetallicRoughnessTexture::MetallicRoughnessTexture() {
    
}

/**
 * @brief Constructor for the OcclusionTexture structure.
 */
Mgtt::Rendering::OcclusionTexture::OcclusionTexture() {
    // Constructor implementation
}

/**
 * @brief Constructor for the BaseColorTexture structure.
 */
Mgtt::Rendering::BaseColorTexture::BaseColorTexture() {
    
}