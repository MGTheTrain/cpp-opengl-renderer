#include "model.h" 

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
}

/**
 * @brief Recursively linearizes the scene hierarchy starting from the given node.
 *
 * This method flattens the hierarchical structure of the scene, storing nodes in a linearNodes vector.
 *
 * @param node The starting node to linearize.
 */
void Mgtt::Rendering::Scene::Linearize(std::shared_ptr<Mgtt::Rendering::Node> node) {
    this->linearNodes.push_back(node);

    for (uint32_t i = 0; i < node->children.size(); i++) {
        this->Linearize(node->children[i]);
    }
}

/**
 * @brief Constructor for the AABB structure.
 */
Mgtt::Rendering::AABB::AABB() {
    this->min = glm::vec3(FLT_MAX);
    this->max = glm::vec3(-FLT_MAX);
    this->center = glm::vec3(0.0f);
}

  /**
 * @brief CalculateBoundingBox calculates the bounding box of an object transformed by the given matrix.
 *
 * This function takes a 4x4 transformation matrix and calculates the bounding box
 * of an object after being transformed by the matrix. The resulting bounding box
 * can be used for various purposes, such as collision detection or rendering optimizations.
 *
 * @param m A 4x4 transformation matrix representing the object's transformation.
 *
 * @return void This function does not return a value. The bounding box information is typically
 *              stored or used internally within the calling code.
 *
 * @note The function assumes that the object's original bounding box is defined in its local space.
 *       The resulting bounding box is in the same coordinate space as the transformed object.
 *
 * @see glm::mat4 - The type of the transformation matrix.
 */
void Mgtt::Rendering::AABB::CalculateBoundingBox(const glm::mat4 &m) {
  glm::vec3 min = glm::vec3(m[3]);
  glm::vec3 max = min;
  glm::vec3 v0, v1;

  glm::vec3 right = glm::vec3(m[0]);
  v0 = right * this->min.x;
  v1 = right * this->max.x;
  min += glm::min(v0, v1);
  max += glm::max(v0, v1);

  glm::vec3 up = glm::vec3(m[1]);
  v0 = up * this->min.y;
  v1 = up * this->max.y;
  min += glm::min(v0, v1);
  max += glm::max(v0, v1);

  glm::vec3 back = glm::vec3(m[2]);
  v0 = back * this->min.z;
  v1 = back * this->max.z;
  min += glm::min(v0, v1);
  max += glm::max(v0, v1);

  this->min = min;
  this->max = max;
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
 * @brief Clear releases resources.
 */
void Mgtt::Rendering::Node::Clear() {
    this->mesh->Clear();
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
 * @brief Clear releases resources.
 */
void Mgtt::Rendering::MeshPrimitive::Clear() {
    this->pbrMaterial->Clear();
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
 * @brief Clear releases resources.
 */
void Mgtt::Rendering::PbrMaterial::Clear() {
    this->baseColorTexture->Clear();
    this->metallicRoughnessTexture->Clear();
    this->normalTexture->Clear();
    this->emissiveTexture->Clear();
    this->occlusionTexture->Clear();
}

/**
 * @brief Constructor for the Texture structure.
 *
 * This constructor initializes a Texture object with the specified texture path.
 *
 **/
Mgtt::Rendering::Texture::Texture() {
    this->name = "";
    this->id = 0;
    this->path = "";
    this->width = 0;
    this->height = 0;
    this->nrComponents = 0;
    this->data = nullptr;
    this->sizeInBytes = 0;
}

/**
 * @brief Clear releases resources.
 *
 * This method clears the resources associated with the Texture object, freeing up memory.
 * It is recommended to call this method when the Texture is no longer needed.
 */
void Mgtt::Rendering::Texture::Clear() {
    if (this->id > 0) {
        glDeleteTextures(1, &this->id);
        this->id = 0;
    }
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