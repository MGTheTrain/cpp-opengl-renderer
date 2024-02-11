#include "model.h" 

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

/**
 * @brief Constructor for the Scene structure.
 */
Mgtt::Rendering::Scene::Scene() {
  this->name = "";
  this->path = "";
  this->pos = glm::vec3(0.0f);
  this->rot = glm::vec3(0.0f); 
  this->scale = 1.0f;
}

Mgtt::Rendering::Scene::~Scene() {
    
}


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
}

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
    this->alphaCutoff = 0.0f;
    this->doubleSided = false;
    this->alphaMode = Mgtt::Rendering::AlphaMode::OPAQE;

    this->normalTexture = std::make_unique<NormalTexture>();
    this->occlusionTexture = std::make_unique<OcclusionTexture>();
    this->emissiveTexture = std::make_unique<EmissiveTexture>();
    this->baseColorTexture = std::make_unique<BaseColorTexture>();
    this->metallicRoughnessTexture = std::make_unique<MetallicRoughnessTexture>();
}

/**
 * @brief Constructor for the Texture structure.
 *
 * This constructor initializes a Texture object with the specified texture path.
 *
 * @param texturePath The file path to the texture.
 Mgtt::Rendering::*/
Mgtt::Rendering::Texture::Texture(const std::string& texturePath) {
    this->name = "";
    this->path = "";
    this->width = 0;
    this->height = 0;
    this->nrComponents = 0;
    this->data = nullptr;
    this->sizeInBytes = 0;
}

/**
 * @brief Load a texture from the specified file path.
 *
 * This method loads a texture from the given file path and updates the Texture object.
 *
 * @param texturePath The file path to the texture.
 */
void Mgtt::Rendering::Texture::Load(const std::string& texturePath) {
    if(!this->data) {
        this->data = stbi_load(texturePath.c_str(),
            &this->width, &this->height, &this->nrComponents, 0);
    }
}

/**
 * @brief Clear the Texture resources.
 *
 * This method clears the resources associated with the Texture object, freeing up memory.
 * It is recommended to call this method when the Texture is no longer needed.
 */
void Mgtt::Rendering::Texture::Clear() {
    if (this->data) {
        stbi_image_free(this->data);
        this->data = nullptr;
    }
}

/**
* @brief @brief Constructor for the Texture structure
*/
Mgtt::Rendering::Texture::Texture() {
    this->name = "";
    this->path = "";
    this->width = 0;
    this->height = 0;
    this->nrComponents = 0;
    this->data = nullptr;
    this->sizeInBytes = 0;
}

/**
 * @brief Constructor for the NormalTexture structure.
 */
Mgtt::Rendering::NormalTexture::NormalTexture() {
    this->scale = 1.0f;
}

/**
 * @brief Constructor for the EmissiveTexture structure.
 */
Mgtt::Rendering::EmissiveTexture::EmissiveTexture() {
    this->color = glm::vec3(0.0f);
}

/**
 * @brief Constructor for the MetallicRoughnessTexture structure.
 */
Mgtt::Rendering::MetallicRoughnessTexture::MetallicRoughnessTexture() {
    this->metallicFactor = 0.0f;
    this->roughnessFactor = 1.0f;
}

/**
 * @brief Constructor for the OcclusionTexture structure.
 */
Mgtt::Rendering::OcclusionTexture::OcclusionTexture() {
    this->color = glm::vec3(0.0f);
}

/**
 * @brief Constructor for the BaseColorTexture structure.
 */
Mgtt::Rendering::BaseColorTexture::BaseColorTexture() {
    this->color = glm::vec4(1.0f);
}