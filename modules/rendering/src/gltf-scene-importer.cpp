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
 * This method overrides the corresponding method in the ISceneImporter
 * interface. It loads the 3D scene from the specified file path.
 *
 * @param path The file path from which to load the 3D scene.
 * @param An instance of the loaded 3D scene.
 *
 * @note This implementation is inspired by the Vulkan glTF PBR example:
 * https://github.com/SaschaWillems/Vulkan-glTF-PBR/blob/master/base/VulkanglTFModel.cpp
 */
void Mgtt::Rendering::GltfSceneImporter::Load(Mgtt::Rendering::Scene& mgttScene,
                                              const std::string& path) {
  try {
    if (mgttScene.shader.GetProgramId() == 0) {
      throw std::runtime_error(
          "LOAD ERROR: Ensure that a shader program for [scene.shader]exists");
    }

    mgttScene.path = path;

    bool hasGltfSuffix = (path.substr(path.size() - 5, 5) == ".GLTF" ||
                          path.substr(path.size() - 5, 5) == ".gltf" ||
                          path.substr(path.size() - 4, 4) == ".GLB" ||
                          path.substr(path.size() - 4, 4) == ".glb");

    if (!hasGltfSuffix) {
      throw std::runtime_error("GLTF IMPORTER ERROR: No proper suffix for: " +
                               path);
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
    bool fileLoaded = binary ? gltfContext.LoadBinaryFromFile(
                                   &gltfModel, &err, &warn, path.c_str())
                             : gltfContext.LoadASCIIFromFile(
                                   &gltfModel, &err, &warn, path.c_str());

    if (fileLoaded) {
      this->LoadTextures(mgttScene, gltfModel);
      this->LoadMaterials(mgttScene, gltfModel);
      const tinygltf::Scene& scene =
          gltfModel
              .scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];
      for (size_t i = 0; i < scene.nodes.size(); i++) {
        const tinygltf::Node node = gltfModel.nodes[scene.nodes[i]];
        LoadNode(nullptr, mgttScene, node, scene.nodes[i], gltfModel);
      }
    } else {
      throw std::runtime_error("GLTF IMPORTER ERROR: Could not load file: " +
                               path);
    }
  } catch (const std::runtime_error& ex) {
    this->Clear(mgttScene);
    std::cerr << ex.what() << std::endl;
  }
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
 * @brief Extracts the folder path from a given file path.
 *
 * This function takes a file path as input and extracts the folder path
 * by finding the last occurrence of the directory separator ('/' or '\\').
 *
 * @param filePath The full file path from which to extract the folder path.
 * @return The extracted folder path. If no directory separator is found,
 *         an empty string is returned.
 *
 * @note The function uses the platform-specific directory separator ('/' or
 *'\\').
 * @note The returned folder path includes the trailing directory separator.
 **/
std::string Mgtt::Rendering::GltfSceneImporter::ExtractFolderPath(
    const std::string& filePath) {
  const size_t lastSeparatorIdx = filePath.find_last_of("\\/");
  std::string folderPath = "";
  if (std::string::npos != lastSeparatorIdx) {
    folderPath = filePath.substr(0, lastSeparatorIdx + 1);
  }

  return folderPath;
}

/**
 * @brief Clear the RAM resources associated with the Texture object.
 *
 * This method releases the memory resources in RAM associated with the Texture
 * object, freeing up memory. It is recommended to call this method when the
 * Texture is no longer needed to avoid unnecessary memory usage.
 *
 * @param texture Reference to the Texture object for which RAM resources should
 * be cleared.
 */
void Mgtt::Rendering::GltfSceneImporter::ClearRAM(
    Mgtt::Rendering::Texture& texture) {
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
void Mgtt::Rendering::GltfSceneImporter::LoadTextures(
    Mgtt::Rendering::Scene& scene, tinygltf::Model& gltfModel) {
  auto efp = this->ExtractFolderPath(scene.path);
  for (tinygltf::Texture& tex : gltfModel.textures) {
    Mgtt::Rendering::Texture texture;
    tinygltf::Image image = gltfModel.images[tex.source];

    // Consider primarily gltf with non-embedded inmages
    texture.name = image.name;
    texture.path = efp + image.uri;
    texture.width = image.width;
    texture.height = image.height;
    texture.nrComponents = image.component;
    texture.data = stbi_load(texture.path.c_str(), &image.height, &image.height,
                             &image.component, image.component);
    if (texture.data) {
      this->SetupTexture(texture);
      if (texture.data) {
        stbi_image_free(texture.data);
        texture.data = nullptr;
      }

      scene.textureMap[image.uri] = texture;
    }
  }
  std::cout << "LOAD TEXTURES INFO: Successfully loaded all textures for scene "
            << scene.path << std::endl;
}

/**
 * @brief Sets up a texture for rendering.
 *
 * It ensures that the texture data is properly uploaded to video memory (VRAM)
 * for efficient rendering.
 *
 * @param texture A reference to the `Mgtt::Rendering::Texture` object to
 * modify.
 */
void Mgtt::Rendering::GltfSceneImporter::SetupTexture(
    Mgtt::Rendering::Texture& texture) {
  if (texture.data) {
    auto format = GL_RGBA;
    if (texture.nrComponents == 1) {
      format = GL_RED;
    } else if (texture.nrComponents == 2) {
      format = GL_RG;
    } else if (texture.nrComponents == 3) {
      format = GL_RGB;
    } else if (texture.nrComponents == 4) {
      format = GL_RGBA;
    }

    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);
    glTexImage2D(GL_TEXTURE_2D, 0, format, texture.width, texture.height, 0,
                 format, GL_UNSIGNED_BYTE, texture.data);

    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  } else {
    std::cout << "LOG INFO: Texture data is a nullptr" << std::endl;
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
void Mgtt::Rendering::GltfSceneImporter::LoadMaterials(
    Mgtt::Rendering::Scene& scene, tinygltf::Model& gltfModel) {
  for (tinygltf::Material& material : gltfModel.materials) {
    Mgtt::Rendering::PbrMaterial pbrMaterial;
    pbrMaterial.alphaCutoff = material.alphaCutoff;
    pbrMaterial.doubleSided = material.doubleSided;
    if (material.alphaMode == "OPAQUE") {
      pbrMaterial.alphaMode = Mgtt::Rendering::AlphaMode::OPAQ;
    } else if (material.alphaMode == "MASK") {
      pbrMaterial.alphaMode = Mgtt::Rendering::AlphaMode::MASK;
    } else if (material.alphaMode == "BLEND") {
      pbrMaterial.alphaMode = Mgtt::Rendering::AlphaMode::BLEND;
    } else {
      pbrMaterial.alphaMode = Mgtt::Rendering::AlphaMode::NONE;
    }

    // Base color
    if (material.pbrMetallicRoughness.baseColorTexture.index > -1) {
      pbrMaterial.baseColorTexture = BaseColorTexture(
          scene.textureMap
              [gltfModel
                   .images[gltfModel
                               .textures[material.pbrMetallicRoughness
                                             .baseColorTexture.index]
                               .source]
                   .uri],
          glm::make_vec4(material.pbrMetallicRoughness.baseColorFactor.data()));
    } else {
      pbrMaterial.baseColorTexture = BaseColorTexture(
          Texture(),
          glm::make_vec4(material.pbrMetallicRoughness.baseColorFactor.data()));
    }

    // Normal
    if (material.normalTexture.index > -1) {
      pbrMaterial.normalTexture = NormalTexture(
          scene.textureMap
              [gltfModel
                   .images[gltfModel.textures[material.normalTexture.index]
                               .source]
                   .uri],
          material.normalTexture.scale);
    } else {
      pbrMaterial.normalTexture =
          NormalTexture(Texture(), material.normalTexture.scale);
    }

    // Normal
    if (material.occlusionTexture.index > -1) {
      pbrMaterial.occlusionTexture = OcclusionTexture(
          scene.textureMap
              [gltfModel
                   .images[gltfModel.textures[material.occlusionTexture.index]
                               .source]
                   .uri],
          material.occlusionTexture.strength);
    } else {
      pbrMaterial.occlusionTexture =
          OcclusionTexture(Texture(), material.occlusionTexture.strength);
    }

    // Emissive
    if (material.emissiveTexture.index > -1) {
      pbrMaterial.emissiveTexture = EmissiveTexture(
          scene.textureMap
              [gltfModel
                   .images[gltfModel.textures[material.emissiveTexture.index]
                               .source]
                   .uri],
          glm::make_vec3(material.emissiveFactor.data()));
    } else {
      pbrMaterial.emissiveTexture = EmissiveTexture(
          Texture(), glm::make_vec3(material.emissiveFactor.data()));
    }

    // Metallic roughness
    if (material.pbrMetallicRoughness.metallicRoughnessTexture.index > -1) {
      pbrMaterial.metallicRoughnessTexture = MetallicRoughnessTexture(
          scene.textureMap
              [gltfModel
                   .images[gltfModel
                               .textures[material.pbrMetallicRoughness
                                             .metallicRoughnessTexture.index]
                               .source]
                   .uri],
          static_cast<float>(material.pbrMetallicRoughness.metallicFactor),
          static_cast<float>(material.pbrMetallicRoughness.roughnessFactor));
    } else {
      pbrMaterial.metallicRoughnessTexture = MetallicRoughnessTexture(
          Texture(),
          static_cast<float>(material.pbrMetallicRoughness.metallicFactor),
          static_cast<float>(material.pbrMetallicRoughness.roughnessFactor));
    }

    scene.materials.push_back(pbrMaterial);
  }
  std::cout
      << "LOAD MATERIALS INFO: Successfully loaded all materials for scene "
      << scene.path << std::endl;
}

/**
 * @brief Sets up a mesh for rendering, including vertex attribute
 * configuration.
 *
 * This method prepares a mesh for rendering by configuring its vertex
 * attributes, associating it with the specified shader, and potentially moving
 * vertex data to VRAM.
 *
 * @param mesh A shared pointer to the `Mgtt::Rendering::Mesh` object
 * representing the mesh.
 * @param shaderId An unsigned 32-bit integer representing the shader ID.
 */
void Mgtt::Rendering::GltfSceneImporter::SetupMesh(
    std::shared_ptr<Mgtt::Rendering::Mesh>& mesh, uint32_t& shaderId) {
  if (mesh) {
    if (mesh->ebo > 0) {
      throw std::runtime_error(
          "OPENGL SCENE ALLOCATOR ERROR: mesh ebo needs to be equal 0");
    } else if (mesh->pos > 0) {
      throw std::runtime_error(
          "OPENGL SCENE ALLOCATOR ERROR: mesh pos id needs to be equal 0");
    } else if (mesh->normal > 0) {
      throw std::runtime_error(
          "OPENGL SCENE ALLOCATOR ERROR: mesh normal id needs to be equal 0");
    } else if (mesh->tex > 0) {
      throw std::runtime_error(
          "OPENGL SCENE ALLOCATOR ERROR: mesh tex id needs to be equal 0");
    } else if (mesh->vertexPositionAttribs.size() == 0) {
      throw std::runtime_error(
          "OPENGL SCENE ALLOCATOR ERROR: mesh vertex position attributes needs "
          "contain elements");
    } else if (mesh->vertexPositionAttribs.size() == 0) {
      throw std::runtime_error(
          "OPENGL SCENE ALLOCATOR ERROR: mesh vertex position attributes need "
          "to contain elements");
    }

    glGenVertexArrays(1, &mesh->vao);
    glGenBuffers(1, &mesh->pos);
    glGenBuffers(1, &mesh->normal);
    glGenBuffers(1, &mesh->tex);
    glGenBuffers(1, &mesh->ebo);

    glBindVertexArray(mesh->vao);

    // indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 mesh->indices.size() * sizeof(unsigned int), &mesh->indices[0],
                 GL_STATIC_DRAW);

    // pos
    glBindBuffer(GL_ARRAY_BUFFER, mesh->pos);
    glBufferData(GL_ARRAY_BUFFER,
                 mesh->vertexPositionAttribs.size() * sizeof(glm::vec3),
                 &mesh->vertexPositionAttribs[0], GL_STATIC_DRAW);
    uint32_t posLoc = glGetAttribLocation(shaderId, "inVertexPosition");
    glEnableVertexAttribArray(posLoc);
    glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                          reinterpret_cast<void*>(0));

    // normal
    glBindBuffer(GL_ARRAY_BUFFER, mesh->normal);
    glBufferData(GL_ARRAY_BUFFER,
                 mesh->vertexNormalAttribs.size() * sizeof(glm::vec3),
                 &mesh->vertexNormalAttribs[0], GL_STATIC_DRAW);
    uint32_t normalLoc = glGetAttribLocation(shaderId, "inVertexNormal");
    glEnableVertexAttribArray(normalLoc);
    glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                          reinterpret_cast<void*>(0));

    // uv
    glBindBuffer(GL_ARRAY_BUFFER, mesh->tex);
    glBufferData(GL_ARRAY_BUFFER,
                 mesh->vertexTextureAttribs.size() * sizeof(glm::vec2),
                 &mesh->vertexTextureAttribs[0], GL_STATIC_DRAW);
    uint32_t texLoc =
        glGetAttribLocation(shaderId, "inVertexTextureCoordinates");
    glEnableVertexAttribArray(texLoc);
    glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2),
                          reinterpret_cast<void*>(0));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);

    glBindVertexArray(0);
  } else {
    std::cout
        << "LOG INFO: Node is not a mesh"
        << std::endl;  // We might utilize spdlog as an improvement for logging
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
    std::shared_ptr<Mgtt::Rendering::Node> parent,
    Mgtt::Rendering::Scene& scene, const tinygltf::Node& node,
    const uint32_t nodeIndex, const tinygltf::Model& model) {
  std::shared_ptr<Mgtt::Rendering::Node> newNode =
      std::make_shared<Mgtt::Rendering::Node>();
  newNode->index = nodeIndex;
  newNode->parent = parent;
  newNode->name = node.name;

  glm::vec3 translation =
      (node.translation.size() == 3)
          ? glm::vec3(glm::make_vec3(node.translation.data()))
          : glm::vec3(0.0f);
  newNode->pos = translation;

  glm::mat4 rotation =
      (node.rotation.size() == 4)
          ? glm::mat4(glm::quat(glm::make_quat(node.rotation.data())))
          : glm::mat4(1.0f);
  newNode->rot = rotation;

  glm::vec3 scale = (node.scale.size() == 3)
                        ? glm::vec3(glm::make_vec3(node.scale.data()))
                        : glm::vec3(1.0f);
  newNode->scale = scale;

  if (node.matrix.size() == 16) {
    newNode->matrix = glm::make_mat4x4(node.matrix.data());
  }

  if (node.children.size() > 0) {
    for (uint32_t i = 0; i < node.children.size(); i++) {
      LoadNode(newNode, scene, model.nodes[node.children[i]], node.children[i],
               model);
    }
  }

  if (node.mesh > -1) {
    const tinygltf::Mesh mesh = model.meshes[node.mesh];
    std::shared_ptr<Mgtt::Rendering::Mesh> newMesh =
        std::make_shared<Mgtt::Rendering::Mesh>();
    for (uint32_t i = 0; i < mesh.primitives.size(); i++) {
      const tinygltf::Primitive& primitive = mesh.primitives[i];
      uint32_t indexStart = newMesh->indices.size();
      uint32_t vertexStart = newMesh->vertexPositionAttribs.size();
      uint32_t indexCount = 0;
      uint32_t vertexCount = 0;

      bool hasSkin = false;
      bool hasIndices = primitive.indices > -1;
      glm::vec3 posMin = glm::vec3(FLT_MAX);
      glm::vec3 posMax = glm::vec3(-FLT_MAX);

      const float* bufferPos = nullptr;
      const float* bufferNormals = nullptr;
      const float* bufferTexCoordSet = nullptr;

      int posByteStride;
      int normByteStride;
      int uv0ByteStride;

      const tinygltf::Accessor& posAccessor =
          model.accessors[primitive.attributes.find("POSITION")->second];
      const tinygltf::BufferView& posView =
          model.bufferViews[posAccessor.bufferView];
      bufferPos = reinterpret_cast<const float*>(
          &(model.buffers[posView.buffer]
                .data[posAccessor.byteOffset + posView.byteOffset]));
      posMin = glm::vec3(posAccessor.minValues[0], posAccessor.minValues[1],
                         posAccessor.minValues[2]);
      posMax = glm::vec3(posAccessor.maxValues[0], posAccessor.maxValues[1],
                         posAccessor.maxValues[2]);
      vertexCount = static_cast<uint32_t>(posAccessor.count);
      posByteStride =
          posAccessor.ByteStride(posView)
              ? (posAccessor.ByteStride(posView) / sizeof(float))
              : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);

      if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
        const tinygltf::Accessor& normAccessor =
            model.accessors[primitive.attributes.find("NORMAL")->second];
        const tinygltf::BufferView& normView =
            model.bufferViews[normAccessor.bufferView];
        bufferNormals = reinterpret_cast<const float*>(
            &(model.buffers[normView.buffer]
                  .data[normAccessor.byteOffset + normView.byteOffset]));
        normByteStride =
            normAccessor.ByteStride(normView)
                ? (normAccessor.ByteStride(normView) / sizeof(float))
                : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);
      }

      if (primitive.attributes.find("TEXCOORD_0") !=
          primitive.attributes.end()) {
        const tinygltf::Accessor& uvAccessor =
            model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
        const tinygltf::BufferView& uvView =
            model.bufferViews[uvAccessor.bufferView];
        bufferTexCoordSet = reinterpret_cast<const float*>(
            &(model.buffers[uvView.buffer]
                  .data[uvAccessor.byteOffset + uvView.byteOffset]));
        uv0ByteStride =
            uvAccessor.ByteStride(uvView)
                ? (uvAccessor.ByteStride(uvView) / sizeof(float))
                : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC2);
      }

      // TBD: joints & weights for animation
      for (size_t v = 0; v < posAccessor.count; v++) {
        newMesh->vertexPositionAttribs.push_back(
            glm::make_vec3(&bufferPos[v * posByteStride]));

        glm::vec3 tmpNormal =
            bufferNormals ? glm::make_vec3(&bufferNormals[v * normByteStride])
                          : glm::vec3(0.0f);
        newMesh->vertexNormalAttribs.push_back(glm::normalize(tmpNormal));

        glm::vec2 tmpTex =
            bufferTexCoordSet
                ? glm::make_vec2(&bufferTexCoordSet[v * uv0ByteStride])
                : glm::vec2(0.0f);
        newMesh->vertexTextureAttribs.push_back(tmpTex);
      }
      if (hasIndices) {
        const tinygltf::Accessor& accessor =
            model.accessors[primitive.indices > -1 ? primitive.indices : 0];
        const tinygltf::BufferView& bufferView =
            model.bufferViews[accessor.bufferView];
        const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
        indexCount = static_cast<unsigned int>(accessor.count);

        const void* dataPtr =
            &(buffer.data[accessor.byteOffset + bufferView.byteOffset]);

        switch (accessor.componentType) {
          case GLTF_PARAMETER_TYPE_UNSIGNED_INT: {
            const uint32_t* buf = static_cast<const uint32_t*>(dataPtr);
            for (size_t index = 0; index < accessor.count; index++) {
              newMesh->indices.push_back(buf[index] + vertexStart);
            }
            break;
          }
          case GLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
            const uint16_t* buf = static_cast<const uint16_t*>(dataPtr);
            for (size_t index = 0; index < accessor.count; index++) {
              newMesh->indices.push_back(buf[index] + vertexStart);
            }
            break;
          }
          case GLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
            const uint8_t* buf = static_cast<const uint8_t*>(dataPtr);
            for (size_t index = 0; index < accessor.count; index++) {
              newMesh->indices.push_back(buf[index] + vertexStart);
            }
            break;
          }
          default:
            return;
        }
      }

      Mgtt::Rendering::MeshPrimitive newPrimitive;
      newPrimitive.firstIndex = indexStart;
      newPrimitive.indexCount = indexCount;
      newPrimitive.hasIndices = true;
      newPrimitive.vertexCount = vertexCount;
      if (primitive.material > -1) {
        // newPrimitive.pbrMaterial =
        newPrimitive.pbrMaterial = scene.materials[primitive.material];
      }
      // else {
      //     newPrimitive.pbrMaterial =
      //     std::make_shared<Mgtt::Rendering::PbrMaterial>();
      // }
      newPrimitive.aabb.min = posMin;
      newPrimitive.aabb.max = posMax;
      newPrimitive.name = node.name;
      newMesh->meshPrimitives.push_back(newPrimitive);
    }
    this->SetupMesh(newMesh, scene.shader.GetProgramId());
    newNode->mesh = newMesh;
  }
  if (parent) {
    parent->children.push_back(newNode);
  } else {
    scene.nodes.push_back(newNode);
    std::cout << "LOAD NODE INFO: Successfully loaded node for scene "
              << newNode->name << " with index " << nodeIndex << std::endl;
  }
}

/**
 * @brief Load cube map textures from the given folder path.
 *
 * This function loads cube map textures and
 * associates it with the provided RenderTexturesContainer. It performs
 * necessary operations to make the textures available for use in rendering.
 *
 * @param container The RenderTexturesContainer to associate with the loaded
 * cube map.
 * @param texturePathes The cube map texture pathes
 */
void Mgtt::Rendering::TextureManager::LoadFromEnvMap(
    Mgtt::Rendering::RenderTexturesContainer& container,
    const std::vector<std::string>& texturePathes) {
  // TBD
  for (auto& texturePath : texturePathes) {
    std::cout << "LOAD FROM ENV INFO: Successfully loaded env map "
              << texturePath << std::endl;
  }
}

/**
 * @brief Load an HDR texture from the given file path.
 *
 * This function loads an HDR texture from the specified file path and
 * associates it with the provided RenderTexturesContainer. It performs
 * necessary operations to make the texture available for use in rendering.
 *
 * @param container The RenderTexturesContainer to associate with the loaded HDR
 * texture.
 * @param texturePath The file path to the HDR texture.
 */
void Mgtt::Rendering::TextureManager::LoadFromHdr(
    Mgtt::Rendering::RenderTexturesContainer& container,
    const std::string& texturePath) {
  Mgtt::Rendering::Texture texture;
  if (!texture.data) {
    texture.data = stbi_load(texturePath.c_str(), &texture.width,
                             &texture.height, &texture.nrComponents, 0);
    if (texture.data) {
      glGenTextures(1, &container.hdrTextureId);
      glBindTexture(GL_TEXTURE_2D, container.hdrTextureId);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, texture.width, texture.height, 0,
                   GL_RGB, GL_UNSIGNED_BYTE, texture.data);

      stbi_image_free(texture.data);
      texture.data = nullptr;

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      glGenFramebuffers(1, &container.fboId);
      glGenRenderbuffers(1, &container.rboId);

      glBindFramebuffer(GL_FRAMEBUFFER, container.fboId);
      glBindRenderbuffer(GL_RENDERBUFFER, container.rboId);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 128, 128);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                GL_RENDERBUFFER, container.rboId);

      if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        container.Clear();
        throw std::runtime_error(
            "OPENGL FRAMEBUFFER ERROR: Framebuffer not complete");
      }

      // Cube map texture id
      glGenTextures(1, &container.cubeMapTextureId);
      glBindTexture(GL_TEXTURE_CUBE_MAP, container.cubeMapTextureId);
      for (uint32_t i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB8, 128, 128,
                     0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
      }
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      // Set up projection and view matrices capturing image data onto the 6
      // cubemap faces
      glm::mat4 captureProjection =
          glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
      std::vector<glm::mat4> captureViews = {
          glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
                      glm::vec3(0.0f, -1.0f, 0.0f)),
          glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f),
                      glm::vec3(0.0f, -1.0f, 0.0f)),
          glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
                      glm::vec3(0.0f, 0.0f, 1.0f)),
          glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),
                      glm::vec3(0.0f, 0.0f, -1.0f)),
          glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f),
                      glm::vec3(0.0f, -1.0f, 0.0f)),
          glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f),
                      glm::vec3(0.0f, -1.0f, 0.0f))};

      if (container.eq2CubeMapShader.GetProgramId() == 0) {
        container.Clear();
        throw std::runtime_error(
            "LOAD FROM HDR ERROR: Ensure that a shader program for "
            "[eq2CubeMapShader]exists");
      }

      glUseProgram(container.eq2CubeMapShader.GetProgramId());
      glUniform1i(
          glGetUniformLocation(container.eq2CubeMapShader.GetProgramId(),
                               "equirectangularMap"),
          0);
      glUniformMatrix4fv(
          glGetUniformLocation(container.eq2CubeMapShader.GetProgramId(),
                               "projection"),
          1, GL_FALSE, &captureProjection[0][0]);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, container.hdrTextureId);

      glViewport(0, 0, 128, 128);
      glBindFramebuffer(GL_FRAMEBUFFER, container.fboId);
      for (uint32_t i = 0; i < 6; ++i) {
        glUniformMatrix4fv(
            glGetUniformLocation(container.eq2CubeMapShader.GetProgramId(),
                                 "view"),
            1, GL_FALSE, &captureViews[i][0][0]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                               container.cubeMapTextureId, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        this->SetupCube(container);
      }
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      // glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureId);
      // glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

      glDeleteTextures(1, &container.hdrTextureId);
      container.hdrTextureId = 0;
      container.textures.push_back(texture);
    }
  }
  std::cout << "LOAD FROM HDR INFO: Successfully loaded env map from hdr "
            << texturePath << std::endl;
}

/**
 * @brief Clear the provided render textures container.
 *
 * This function clears the contents of the provided RenderTexturesContainer,
 * releasing any resources associated with the textures.
 *
 * @param container The RenderTexturesContainer to clear.
 */
void Mgtt::Rendering::TextureManager::Clear(
    Mgtt::Rendering::RenderTexturesContainer& container) {
  container.Clear();
}

/**
 * @brief Check if any value in the given vector is greater than zero.
 *
 * The function iterates through the elements of the vector and returns true
 * if it finds at least one element greater than zero; otherwise, it returns
 * false.
 *
 * @param vec A const reference to a vector of unsigned integers to be checked.
 * @return true if any value in the vector is greater than zero, false
 * otherwise.
 */
bool Mgtt::Rendering::TextureManager::HasValuesGreaterThanZero(
    const std::vector<unsigned int>& vec) {
  for (auto& val : vec) {
    if (val > 0) {
      return true;
    }
  }
  return false;
}

/**
 * @brief Set up rendering resources for a cube.
 *
 * The SetupCube function initializes and configures rendering resources
 * for a cube, including textures and buffers. It uses the provided
 * RenderTexturesContainer to manage the associated textures.
 *
 * @param container A reference to a RenderTexturesContainer used to manage
 *                  rendering-related textures and resources.
 */
void Mgtt::Rendering::TextureManager::SetupCube(
    Mgtt::Rendering::RenderTexturesContainer& container) {
  std::vector<uint32_t> vec{container.cubeVao, container.cubeVbo};
  if (!this->HasValuesGreaterThanZero(vec)) {
    glGenVertexArrays(1, &container.cubeVao);
    glGenBuffers(1, &container.cubeVbo);
  }
  std::vector<float> vertices = {
      -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
      1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

      -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,
      -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,

      1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,

      -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,

      -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,

      -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,
      1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f};
  uint32_t posLoc = glGetAttribLocation(container.envMapShader.GetProgramId(),
                                        "inVertexPosition");

  glBindVertexArray(container.cubeVao);
  glBindBuffer(GL_ARRAY_BUFFER, container.cubeVbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(),
               vertices.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(posLoc);
  glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                        reinterpret_cast<void*>(0));
  glBindVertexArray(container.cubeVao);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
}

/**
 * @brief Set up rendering resources for a cube.
 *
 * The SetupQuad function initializes and configures rendering resources
 * for a quad, including textures and buffers. It uses the provided
 * RenderTexturesContainer to manage the associated textures.
 *
 * @param container A reference to a RenderTexturesContainer used to manage
 *                  rendering-related textures and resources.
 */
void Mgtt::Rendering::TextureManager::SetupQuad(
    Mgtt::Rendering::RenderTexturesContainer& container) {
  std::vector<uint32_t> vec{container.quadVao, container.quadVbo};
  if (!this->HasValuesGreaterThanZero(vec)) {
    uint32_t posLoc = glGetAttribLocation(
        container.brdfLutShader.GetProgramId(), "inVertexPosition");
    uint32_t texLoc = glGetAttribLocation(
        container.brdfLutShader.GetProgramId(), "inVertexTextureCoordinates");

    std::vector<float> vertices = {
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
    };
    glGenVertexArrays(1, &container.quadVao);
    glGenBuffers(1, &container.quadVbo);

    glBindVertexArray(container.quadVao);
    glBindBuffer(GL_ARRAY_BUFFER, container.quadVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(),
                 vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(posLoc);
    glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(texLoc);
    glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          reinterpret_cast<void*>(3 * sizeof(float)));

    glBindVertexArray(container.quadVao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
  } else {
    std::cout << "TEXTURE MANAGER ERROR: OpenGl resources have been already "
                 "allocated. Check for: [quadVao, quadVbo]"
              << std::endl;
  }
}

/**
 * @brief Load the BRDF Lookup Texture into the provided HDR texture container.
 *
 * This function loads the BRDF Lookup Texture into the provided HDR texture
 * container. It performs necessary operations to make the BRDF texture
 * available for use in rendering.
 *
 * @param container The RenderTexturesContainer to associate with the loaded
 * BRDF texture.
 */
void Mgtt::Rendering::TextureManager::LoadBrdfLut(
    Mgtt::Rendering::RenderTexturesContainer& container) {
  if (container.brdfLutShader.GetProgramId() == 0) {
    throw std::runtime_error(
        "LOAD BRDF LUT ERROR: Ensure that a shader program for "
        "[brdfLutShader]exists");
  }
  std::vector<uint32_t> vec{container.brdfLutTextureId};
  if (!this->HasValuesGreaterThanZero(vec)) {
    glGenTextures(1, &container.brdfLutTextureId);
    glBindTexture(GL_TEXTURE_2D, container.brdfLutTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, 128, 128, 0, GL_RG, GL_UNSIGNED_BYTE,
                 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, container.fboId);
    glBindRenderbuffer(GL_RENDERBUFFER, container.rboId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 128, 128);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           container.brdfLutTextureId, 0);

    glViewport(0, 0, 128, 128);
    glUseProgram(container.brdfLutShader.GetProgramId());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    this->SetupQuad(container);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  } else {
    std::cout << "TEXTURE MANAGER ERROR: OpenGl resources have been already "
                 "allocated. Check for: [brdfLutTextureId]"
              << std::endl;
  }
  std::cout << "LOAD FROM HDR INFO: Successfully loaded brdf lut texture"
            << std::endl;
}

/**
 * @brief Generate the irradiance map for the provided HDR texture container.
 *
 * This function generates the irradiance map for the provided HDR texture
 * container. It performs necessary operations to make the irradiance map
 * available for use in rendering.
 *
 * @param container The RenderTexturesContainer to associate with the generated
 * irradiance map.
 */
void Mgtt::Rendering::TextureManager::GenerateIrradianceMap(
    Mgtt::Rendering::RenderTexturesContainer& container) {}
