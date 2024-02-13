#include <gltf-scene-importer.h>

// Keep MACROS in one cpp file only
// @note https://github.com/syoyo/tinygltf/issues/175
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#include <tiny_gltf.h>

/**
 * @brief Load the 3D scene from a specified file path.
 * 
 * This method overrides the corresponding method in the ISceneImporter interface.
 * It loads the 3D scene from the specified file path.
 * 
 * @param path The file path from which to load the 3D scene.
 * @return An instance of the loaded 3D scene.
 * 
 * @note This implementation is inspired by the Vulkan glTF PBR example:
 * https://github.com/SaschaWillems/Vulkan-glTF-PBR/blob/master/base/VulkanglTFModel.cpp
 */
Mgtt::Rendering::Scene& Mgtt::Rendering::GltfSceneImporter::Load(const std::string& path) {
    Mgtt::Rendering::Scene mgttScene;
    mgttScene.path = path;

    bool hasGltfSuffix = (path.substr(path.size() - 5, 5) == ".GLTF" ||
                          path.substr(path.size() - 5, 5) == ".gltf" ||
                          path.substr(path.size() - 4, 4) == ".GLB" ||
                          path.substr(path.size() - 4, 4) == ".glb");

    if (!hasGltfSuffix) {
        throw std::runtime_error("GLTF IMPORTER ERROR: No proper suffix for: " + path);
    }

    bool binary = false;
    size_t suffix = path.rfind('.', path.size());
    if (suffix != std::string::npos) {
        binary = (path.substr(suffix + 1, path.length() - suffix) == "glb");
    }

    std::string err;
    std::string warn;
    tinygltf::Model gltfModel;
    tinygltf::TinyGLTF gltfContext;
    bool fileLoaded = binary ? gltfContext.LoadBinaryFromFile(&gltfModel, &err, &warn, path.c_str()) : gltfContext.LoadASCIIFromFile(&gltfModel, &err, &warn, path.c_str());

    if (fileLoaded) {
        this->LoadTextures(mgttScene, gltfModel);
        this->LoadMaterials(mgttScene, gltfModel);
        const tinygltf::Scene& scene = gltfModel.scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];
        for (size_t i = 0; i < scene.nodes.size(); i++) {
            const tinygltf::Node node = gltfModel.nodes[scene.nodes[i]];
            LoadNode(nullptr, mgttScene, node, scene.nodes[i], gltfModel);
        }
    }
    else {
        this->Clear(mgttScene);
        throw std::runtime_error("GLTF IMPORTER ERROR: Could not load file: " + path);
    }

    return mgttScene;
}

/**
 * @brief Clear the allocated resources in RAM and VRAM for the scene object.
 * 
 * This method is used to reset the internal state of the scene.
 * @param scene A unique pointer to the scene to clear.
 */
void Mgtt::Rendering::GltfSceneImporter::Clear(Mgtt::Rendering::Scene& scene) {
    scene.Clear();
}

/**
 * @brief Load a texture from the specified file path and update the Texture object.
 *
 * This method loads a texture from the given file path and updates the provided Texture object with the loaded data.
 *
 * @param texturePath The file path to the texture.
 * @param texture Reference to the Texture object to be updated with the loaded data.
 */
void Mgtt::Rendering::GltfSceneImporter::Load(const std::string& texturePath, Mgtt::Rendering::Texture& texture) {
    if (!texture.data) {
        texture.data = stbi_load(texturePath.c_str(),
            &texture.width, &texture.height, &texture.nrComponents, 0);
    }
}

/**
 * @brief Clear the RAM resources associated with the Texture object.
 *
 * This method releases the memory resources in RAM associated with the Texture object, freeing up memory.
 * It is recommended to call this method when the Texture is no longer needed to avoid unnecessary memory usage.
 *
 * @param texture Reference to the Texture object for which RAM resources should be cleared.
 */
void Mgtt::Rendering::GltfSceneImporter::ClearRAM(Mgtt::Rendering::Texture& texture) {
    if (texture.data) {
        stbi_image_free(texture.data);
        texture.data = nullptr;
    }
}

/**
 * @brief Load texture from the provided glTF model.
 *
 * This method loads texture from the given glTF model and updates the
 * internal representation of the 3D scene accordingly.
 *
 * @param scene A reference to the updated 3D scene after loading nodes.
 * @param gltfModel The glTF model containing node information.
 */
void Mgtt::Rendering::GltfSceneImporter::LoadTextures(Mgtt::Rendering::Scene& scene, tinygltf::Model& gltfModel) {
    for (tinygltf::Texture& tex : gltfModel.textures) {
        Mgtt::Rendering::Texture texture;
        tinygltf::Image image = gltfModel.images[tex.source];
       
        // Consider primarily gltf with non-embedded inmages
        texture.name= image.name;
        texture.path = image.uri;
        texture.width = image.width;
        texture.height = image.height;
        texture.nrComponents = image.component;
        texture.data = stbi_load(image.uri.c_str(), &image.height, &image.height, &image.component, 0); // data needs to be transferred to VRAM first and can then be freed
        this->SetupTexture(texture);
        if(texture.data) {
            stbi_image_free(texture.data);
            texture.data = nullptr;
        }

        scene.textureMap[image.name] = texture;
    }
}

/**
 * @brief Sets up a texture for rendering.
 *
 * It ensures that the texture data is properly uploaded to video memory (VRAM) for efficient rendering.
 *
 * @param texture A reference to the `Mgtt::Rendering::Texture` object to modify.
 */
void Mgtt::Rendering::GltfSceneImporter::SetupTexture(Mgtt::Rendering::Texture& texture) {
    if (texture.data) {
        auto format = GL_RGBA;
        if (texture.nrComponents == 1) {
            format = GL_RED;
        }
        else if (texture.nrComponents == 2) {
            format = GL_RG;
        }
        else if (texture.nrComponents == 3) {
            format = GL_RGB;
        }
        else if (texture.nrComponents == 4) {
            format = GL_RGBA;
        }

        glGenTextures(1, &texture.id);
        glBindTexture(GL_TEXTURE_2D, texture.id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, texture.width, texture.height, 0, format, GL_UNSIGNED_BYTE, texture.data);

        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else {
        std::cout << "LOG INFO: Texture is a nullptr" << std::endl;
    }
}

/**
 * @brief Load materials from the provided glTF model.
 *
 * This method loads materials from the given glTF model and updates the
 * internal representation of the 3D scene accordingly.
 *
 * @param scene A reference to the updated 3D scene after loading nodes.
 * @param gltfModel The glTF model containing node information.
 */
void Mgtt::Rendering::GltfSceneImporter::LoadMaterials(Mgtt::Rendering::Scene& scene, tinygltf::Model& gltfModel) {
    for (tinygltf::Material& material : gltfModel.materials) {
        Mgtt::Rendering::PbrMaterial pbrMaterial;
        pbrMaterial.alphaCutoff = material.alphaCutoff;
        pbrMaterial.doubleSided = material.doubleSided;
        if (material.alphaMode == "OPAQUE") {
            pbrMaterial.alphaMode = Mgtt::Rendering::AlphaMode::OPAQ;
        }
        else if (material.alphaMode == "MASK") {
            pbrMaterial.alphaMode = Mgtt::Rendering::AlphaMode::MASK;
        }
        else if (material.alphaMode == "BLEND") {
            pbrMaterial.alphaMode = Mgtt::Rendering::AlphaMode::BLEND;
        }
        else  {
            pbrMaterial.alphaMode = Mgtt::Rendering::AlphaMode::NONE;
        }

        // Base color
        BaseColorTexture baseColorTexture;
        if (material.values.find("baseColorTexture") != material.values.end() && material.values.find("baseColorFactor") != material.values.end()) {
            baseColorTexture = BaseColorTexture(
                scene.textureMap[gltfModel.images[gltfModel.textures[material.values["baseColorTexture"].TextureIndex()].source].name],
                glm::make_vec4(material.values["baseColorFactor"].ColorFactor().data()));
        }
        else {
            if (material.values.find("baseColorFactor") != material.values.end()) {
                baseColorTexture = BaseColorTexture(
                    Texture(),
                    glm::make_vec4(material.values["baseColorFactor"].ColorFactor().data()));}
        }

        // Normal
        NormalTexture normalTexture;
        if (material.values.find("normalTexture") != material.values.end()) {
            normalTexture = NormalTexture(
                scene.textureMap[gltfModel.images[gltfModel.textures[material.values["normalTexture"].TextureIndex()].source].name],
                1.0f);
        }
        else {
            normalTexture = NormalTexture(
                Texture(),
                1.0f);
        }

        // Normal
        OcclusionTexture occlusionTexture;
        if (material.values.find("occlusionTexture") != material.values.end()) {
            occlusionTexture = OcclusionTexture(
                scene.textureMap[gltfModel.images[gltfModel.textures[material.values["occlusionTexture"].TextureIndex()].source].name],
                glm::vec3(1.0f));
        }
        else {
            occlusionTexture = OcclusionTexture(
                Texture(),
                glm::vec3(1.0f));
        }

        // Emissive
        EmissiveTexture emissiveTexture;
        if (material.values.find("emissiveTexture") != material.values.end()) {
            emissiveTexture = EmissiveTexture(
                scene.textureMap[gltfModel.images[gltfModel.textures[material.values["emissiveTexture"].TextureIndex()].source].name],
                glm::vec3(0.0f));
        }
        else {
            emissiveTexture = EmissiveTexture(
                Texture(),
                glm::vec3(0.0f));
        }

        // Metallic roughness
        MetallicRoughnessTexture metallicRoughnessTexture;
        if (material.values.find("metallicRoughnessTexture") != material.values.end() && 
            material.values.find("roughnessFactor") != material.values.end() && 
            material.values.find("metallicFactor") != material.values.end()) {
            metallicRoughnessTexture = MetallicRoughnessTexture(
                scene.textureMap[gltfModel.images[gltfModel.textures[material.values["metallicRoughnessTexture"].TextureIndex()].source].name],
                    static_cast<float>(material.values["metallicFactor"].Factor()),
                    static_cast<float>(material.values["roughnessFactor"].Factor())
            );
        }
        else {
            if(material.values.find("roughnessFactor") != material.values.end() && 
               material.values.find("metallicFactor") != material.values.end()) {
                metallicRoughnessTexture = MetallicRoughnessTexture(
                    Texture(),
                    static_cast<float>(material.values["metallicFactor"].Factor()),
                    static_cast<float>(material.values["roughnessFactor"].Factor()));
            }
        }

        scene.materials.push_back(pbrMaterial);
    }
}

/**
 * @brief Sets up a mesh for rendering, including vertex attribute configuration.
 *
 * This method prepares a mesh for rendering by configuring its vertex attributes,
 * associating it with the specified shader, and potentially moving vertex data to VRAM.
 *
 * @param mesh A shared pointer to the `Mgtt::Rendering::Mesh` object representing the mesh.
 * @param shaderId An unsigned 32-bit integer representing the shader ID.
 */
void Mgtt::Rendering::GltfSceneImporter::SetupMesh(std::shared_ptr<Mgtt::Rendering::Mesh>& mesh, uint32_t& shaderId) {
    if(mesh) {
        if(mesh->ebo > 0) {
            throw std::runtime_error("OPENGL SCENE ALLOCATOR ERROR: mesh ebo needs to be equal 0");
        }
        else if(mesh->pos > 0) {
            throw std::runtime_error("OPENGL SCENE ALLOCATOR ERROR: mesh pos id needs to be equal 0");
        }
        else if(mesh->normal > 0) {
            throw std::runtime_error("OPENGL SCENE ALLOCATOR ERROR: mesh normal id needs to be equal 0");
        }
        else if(mesh->tex > 0) {
            throw std::runtime_error("OPENGL SCENE ALLOCATOR ERROR: mesh tex id needs to be equal 0");
        }
        else if(mesh->vertexPositionAttribs.size() == 0) {
            throw std::runtime_error("OPENGL SCENE ALLOCATOR ERROR: mesh vertex position attributes needs contain elements");
        }
        else if(mesh->vertexPositionAttribs.size() == 0) {
            throw std::runtime_error("OPENGL SCENE ALLOCATOR ERROR: mesh vertex position attributes need to contain elements");
        }
        else if(mesh->indices.size() == 0) {
            throw std::runtime_error("OPENGL SCENE ALLOCATOR ERROR: mesh indices need to contain elements");
        }

        glGenVertexArrays(1, &mesh->vao);
        glGenBuffers(1, &mesh->pos);
        glGenBuffers(1, &mesh->normal);
        glGenBuffers(1, &mesh->tex);
        glGenBuffers(1, &mesh->ebo);

        glBindVertexArray(mesh->vao);

        if (mesh->indices.size() > 0) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                mesh->indices.size() * sizeof(unsigned int),
                &mesh->indices[0], GL_STATIC_DRAW);
        }

        // indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(unsigned int), &mesh->indices[0], GL_STATIC_DRAW);

        // pos
        glBindBuffer(GL_ARRAY_BUFFER, mesh->pos);
        glBufferData(GL_ARRAY_BUFFER, mesh->vertexPositionAttribs.size() * sizeof(glm::vec3), &mesh->vertexPositionAttribs[0], GL_STATIC_DRAW);

        uint32_t posLoc = glGetAttribLocation(shaderId, "inVertexPosition");
        glEnableVertexAttribArray(posLoc); glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), reinterpret_cast<void*>(0));

        // normal 
        glBindBuffer(GL_ARRAY_BUFFER, mesh->normal);
        glBufferData(GL_ARRAY_BUFFER, mesh->vertexNormalAttribs.size() * sizeof(glm::vec3), &mesh->vertexNormalAttribs[0], GL_STATIC_DRAW);
        uint32_t normalLoc = glGetAttribLocation(shaderId, "inVertexNormalCoordinates");
        glEnableVertexAttribArray(normalLoc);
        glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), reinterpret_cast<void*>(0));

        // uv
        glBindBuffer(GL_ARRAY_BUFFER, mesh->tex);
        glBufferData(GL_ARRAY_BUFFER, mesh->vertexTextureAttribs.size() * sizeof(glm::vec2), &mesh->vertexTextureAttribs[0], GL_STATIC_DRAW);
        uint32_t texLoc = glGetAttribLocation(shaderId, "inVertexTextureCoordinates");
        glEnableVertexAttribArray(texLoc);
        glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), reinterpret_cast<void*>(0));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);

        glBindVertexArray(0);
    } else {
        std::cout << "LOG INFO: Node is not a mesh" << std::endl; // We might utilize spdlog as an improvement for logging
    }
}

/**
 * @brief Load nodes from the provided glTF model.
 *
 * This method loads nodes from the given glTF model and updates the
 * internal representation of the 3D scene accordingly.
 *
 * @param parent A shared pointer to the parent node in the 3D scene.
 * @param scene Reference to the updated 3D scene after loading nodes.
 * @param node Reference to the glTF node containing information.
 * @param nodeIndex Index of the current node in the glTF model.
 * @param model Reference to the glTF model containing node information.
 */
void Mgtt::Rendering::GltfSceneImporter::LoadNode(
    std::shared_ptr<Mgtt::Rendering::Node> parent, Mgtt::Rendering::Scene &scene,
    const tinygltf::Node &node, const uint32_t nodeIndex, const tinygltf::Model &model) {
    std::shared_ptr<Mgtt::Rendering::Node> newNode =
      std::make_shared<Mgtt::Rendering::Node>();
    newNode->index = nodeIndex;
    newNode->parent = parent;
    newNode->name = node.name;

    glm::vec3 translation = (node.translation.size() == 3) ? glm::vec3(glm::make_vec3(node.translation.data())) : glm::vec3(0.0f);
    newNode->pos = translation;

    glm::mat4 rotation = (node.rotation.size() == 4) ? glm::mat4(glm::quat(glm::make_quat(node.rotation.data()))) : glm::mat4(1.0f);
    newNode->rot = rotation;

    glm::vec3 scale = (node.scale.size() == 3) ? glm::vec3(glm::make_vec3(node.scale.data())) : glm::vec3(1.0f);
    newNode->scale = scale;

    if (node.matrix.size() == 16) {
        newNode->matrix = glm::make_mat4x4(node.matrix.data());
    }

    if (node.children.size() > 0) {
        for (uint32_t i = 0; i < node.children.size(); i++) {
            LoadNode(newNode, scene, model.nodes[node.children[i]], node.children[i], model);
        }
    }

    if (node.mesh > -1) {
        const tinygltf::Mesh mesh = model.meshes[node.mesh];
        std::shared_ptr<Mgtt::Rendering::Mesh> newMesh =
            std::make_shared<Mgtt::Rendering::Mesh>();
        for (uint32_t i = 0; i < mesh.primitives.size(); i++) {
            const tinygltf::Primitive &primitive = mesh.primitives[i];
            uint32_t indexStart = newMesh->indices.size();
            uint32_t vertexStart = newMesh->vertexPositionAttribs.size();
            uint32_t indexCount = 0;
            uint32_t vertexCount = 0;

            bool hasSkin = false;
            bool hasIndices = primitive.indices > -1;
            glm::vec3 posMin = glm::vec3(FLT_MAX);
            glm::vec3 posMax = glm::vec3(-FLT_MAX);

            const float *bufferPos = nullptr;
            const float *bufferNormals = nullptr;
            const float *bufferTexCoordSet = nullptr;

            int posByteStride;
            int normByteStride;
            int uv0ByteStride;

            const tinygltf::Accessor &posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
            const tinygltf::BufferView &posView = model.bufferViews[posAccessor.bufferView];
            bufferPos = reinterpret_cast<const float *>(&(model.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]));
            posMin = glm::vec3(posAccessor.minValues[0], posAccessor.minValues[1], posAccessor.minValues[2]);
            posMax = glm::vec3(posAccessor.maxValues[0], posAccessor.maxValues[1], posAccessor.maxValues[2]);
            vertexCount = static_cast<uint32_t>(posAccessor.count);
            posByteStride = posAccessor.ByteStride(posView) ? (posAccessor.ByteStride(posView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);

            if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
                const tinygltf::Accessor &normAccessor = model.accessors[primitive.attributes.find("NORMAL")->second];
                const tinygltf::BufferView &normView = model.bufferViews[normAccessor.bufferView];
                bufferNormals = reinterpret_cast<const float *>(&(model.buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset]));
                normByteStride = normAccessor.ByteStride(normView) ? (normAccessor.ByteStride(normView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);
            }

            if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
                const tinygltf::Accessor &uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
                const tinygltf::BufferView &uvView = model.bufferViews[uvAccessor.bufferView];
                bufferTexCoordSet = reinterpret_cast<const float *>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
                uv0ByteStride = uvAccessor.ByteStride(uvView) ? (uvAccessor.ByteStride(uvView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC2);
            }

            // TBD: joints & weights for animation
            for (size_t v = 0; v < posAccessor.count; v++) {
                newMesh->vertexPositionAttribs.push_back(glm::make_vec3(&bufferPos[v * posByteStride]));

                glm::vec3 tmpNormal = bufferNormals ? glm::make_vec3(&bufferNormals[v * normByteStride]): glm::vec3(0.0f);
                newMesh->vertexNormalAttribs.push_back(glm::normalize(tmpNormal));
                
                glm::vec2 tmpTex = bufferTexCoordSet? glm::make_vec2(&bufferTexCoordSet[v * uv0ByteStride]): glm::vec2(0.0f);
                newMesh->vertexTextureAttribs.push_back(tmpTex);
            }
            Mgtt::Rendering::MeshPrimitive newPrimitive;
            newPrimitive.firstIndex = indexStart;
            newPrimitive.indexCount = indexCount;
            newPrimitive.hasIndices = true;
            newPrimitive.vertexCount = vertexCount;
            if (primitive.material > -1) {
                newPrimitive.pbrMaterial = 
                    std::make_shared<Mgtt::Rendering::PbrMaterial>(scene.materials[primitive.material]);
            }
            else { 
                newPrimitive.pbrMaterial = std::make_shared<Mgtt::Rendering::PbrMaterial>();
            }
            newPrimitive.aabb.min = posMin;
            newPrimitive.aabb.max = posMax;
            newPrimitive.name = node.name;
            newMesh->meshPrimitives.push_back(newPrimitive);
        }
        this->SetupMesh(newMesh, scene.shaderId);
        newNode->mesh = newMesh;
    }
    if (parent) {
        parent->children.push_back(newNode);
    }
    else {
        scene.nodes.push_back(newNode);
    }
}