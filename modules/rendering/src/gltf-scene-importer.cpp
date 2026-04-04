// The MIT License
//
// Copyright (c) 2026 MGTheTrain
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

#include <iostream>
#include <string>

/**
 * @note Inspired by the Vulkan glTF PBR example:
 * https://github.com/SaschaWillems/Vulkan-glTF-PBR/blob/master/base/VulkanglTFModel.cpp
 */
Mgtt::Common::Result<void> Mgtt::Rendering::GltfSceneImporter::Load(
    Mgtt::Rendering::Scene& mgttScene, std::string_view path) {
  if (mgttScene.shader.GetProgramId() == 0) {
    return Mgtt::Common::Result<void>::Err("Shader program does not exist");
  }

  const std::string kPathStr(path);
  mgttScene.path = kPathStr;

  auto hasSuffix = [&](std::string_view suffix) {
    return kPathStr.size() >= suffix.size() &&
           kPathStr.compare(kPathStr.size() - suffix.size(), suffix.size(),
                            suffix) == 0;
  };
  if (!hasSuffix(".gltf") && !hasSuffix(".GLTF") && !hasSuffix(".glb") &&
      !hasSuffix(".GLB")) {
    return Mgtt::Common::Result<void>::Err("Unsupported file suffix: " +
                                           kPathStr);
  }

  const bool kBinary = hasSuffix(".glb") || hasSuffix(".GLB");

  std::string err;
  std::string warn;
  tinygltf::Model gltfModel;
  tinygltf::TinyGLTF gltfContext;

  const bool kFileLoaded =
      kBinary
          ? gltfContext.LoadBinaryFromFile(&gltfModel, &err, &warn, kPathStr)
          : gltfContext.LoadASCIIFromFile(&gltfModel, &err, &warn, kPathStr);

  if (!kFileLoaded) {
    Clear(mgttScene);
    return Mgtt::Common::Result<void>::Err(
        "Failed to load glTF file: " + kPathStr +
        (err.empty() ? "" : " — " + err));
  }

  if (auto result = LoadTextures(mgttScene, gltfModel); result.err()) {
    Clear(mgttScene);
    return result;
  }
  LoadMaterials(mgttScene, gltfModel);

  const tinygltf::Scene& scene =
      gltfModel
          .scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];

  // NOLINTNEXTLINE(modernize-loop-convert) — idx used as both array index and
  for (size_t idx = 0; idx < scene.nodes.size(); ++idx) {
    const tinygltf::Node& node = gltfModel.nodes[scene.nodes[idx]];
    if (auto result =
            LoadNode(nullptr, mgttScene, node,
                     static_cast<uint32_t>(scene.nodes[idx]), gltfModel);
        result.err()) {
      Clear(mgttScene);
      return result;
    }
  }

  for (const auto& node : mgttScene.nodes) {
    UpdateNodeMeshMatrices(node);
  }
  CalculateSceneDimensions(mgttScene);

  mgttScene.aabb.center = (mgttScene.aabb.min + mgttScene.aabb.max) * 0.5f;

  const glm::vec3 kTmpScale = mgttScene.aabb.max - mgttScene.aabb.min;
  mgttScene.aabb.scale =
      (kTmpScale.x <= 0.0f || kTmpScale.y <= 0.0f || kTmpScale.z <= 0.0f)
          ? 1.0f
          : glm::max(kTmpScale.x, glm::max(kTmpScale.y, kTmpScale.z));

  std::cout << "Scale equals " << mgttScene.aabb.scale << '\n';
  return Mgtt::Common::Result<void>::Ok();
}

void Mgtt::Rendering::GltfSceneImporter::Clear(
    Mgtt::Rendering::Scene& scene) noexcept {
  scene.Clear();
}

std::string Mgtt::Rendering::GltfSceneImporter::ExtractFolderPath(
    std::string_view path) const {
  const std::string kPathStr(path);
  const size_t kSep = kPathStr.find_last_of("\\/");
  return kSep != std::string::npos ? kPathStr.substr(0, kSep + 1) : "";
}

void Mgtt::Rendering::GltfSceneImporter::FreeTextureData(
    Mgtt::Rendering::Texture& texture) {
  if (texture.data != nullptr) {
    stbi_image_free(texture.data);
    texture.data = nullptr;
  }
}

Mgtt::Common::Result<void> Mgtt::Rendering::GltfSceneImporter::LoadTextures(
    Mgtt::Rendering::Scene& scene, tinygltf::Model& gltfModel) {
  const auto kFolderPath = ExtractFolderPath(scene.path);

  for (tinygltf::Texture& tex : gltfModel.textures) {
    tinygltf::Image& image = gltfModel.images[tex.source];

    Mgtt::Rendering::Texture texture;
    texture.name = image.name;
    texture.path = kFolderPath + image.uri;
    texture.width = image.width;
    texture.height = image.height;
    texture.nrComponents = image.component;
    texture.data = stbi_load(texture.path.c_str(), &texture.width,
                             &texture.height, &texture.nrComponents, 0);

    if (texture.data == nullptr) {
      return Mgtt::Common::Result<void>::Err("Failed to load texture: " +
                                             texture.path);
    }

    // GPU upload is deferred to SceneUploader::Upload()
    scene.textureMap[image.uri] = std::move(texture);
  }

  std::cout << "All textures loaded to RAM for scene " << scene.path << '\n';
  return Mgtt::Common::Result<void>::Ok();
}

void Mgtt::Rendering::GltfSceneImporter::LoadMaterials(
    Mgtt::Rendering::Scene& scene, tinygltf::Model& gltfModel) {
  auto lookupTex = [&](int texIndex) -> Mgtt::Rendering::Texture {
    if (texIndex < 0) {
      return Texture{};
    }
    const auto& uri = gltfModel.images[gltfModel.textures[texIndex].source].uri;
    auto iter = scene.textureMap.find(uri);
    return iter != scene.textureMap.end() ? iter->second : Texture{};
  };

  for (const tinygltf::Material& material : gltfModel.materials) {
    Mgtt::Rendering::PbrMaterial pbr;
    pbr.alphaCutoff = static_cast<float>(material.alphaCutoff);
    pbr.doubleSided = material.doubleSided;

    if (material.alphaMode == "MASK") {
      pbr.alphaMode = AlphaMode::MASK;
    } else if (material.alphaMode == "BLEND") {
      pbr.alphaMode = AlphaMode::BLEND;
    } else if (material.alphaMode == "OPAQUE") {
      pbr.alphaMode = AlphaMode::OPAQ;
    } else {
      pbr.alphaMode = AlphaMode::NONE;
    }

    const auto& pbrmr = material.pbrMetallicRoughness;

    pbr.baseColorTexture =
        BaseColorTexture(lookupTex(pbrmr.baseColorTexture.index),
                         glm::make_vec4(pbrmr.baseColorFactor.data()));
    pbr.normalTexture =
        NormalTexture(lookupTex(material.normalTexture.index),
                      static_cast<float>(material.normalTexture.scale));
    pbr.occlusionTexture = OcclusionTexture(
        lookupTex(material.occlusionTexture.index),
        static_cast<float>(material.occlusionTexture.strength));
    pbr.emissiveTexture =
        EmissiveTexture(lookupTex(material.emissiveTexture.index),
                        glm::make_vec3(material.emissiveFactor.data()));
    pbr.metallicRoughnessTexture = MetallicRoughnessTexture(
        lookupTex(pbrmr.metallicRoughnessTexture.index),
        static_cast<float>(pbrmr.metallicFactor),
        static_cast<float>(pbrmr.roughnessFactor));

    scene.materials.push_back(std::move(pbr));
  }
  std::cout << "Materials allocated from scene " << scene.path << '\n';
}

Mgtt::Common::Result<void> Mgtt::Rendering::GltfSceneImporter::LoadNode(
    const std::shared_ptr<Mgtt::Rendering::Node>& parent,
    Mgtt::Rendering::Scene& scene, const tinygltf::Node& node,
    uint32_t nodeIndex, const tinygltf::Model& model) {
  auto newNode = std::make_shared<Mgtt::Rendering::Node>();
  newNode->index = nodeIndex;
  newNode->parent = parent;
  newNode->name = node.name;
  newNode->pos = node.translation.size() == 3
                     ? glm::vec3(glm::make_vec3(node.translation.data()))
                     : glm::vec3(0.0f);
  newNode->rot = node.rotation.size() == 4
                     ? glm::quat(glm::make_quat(node.rotation.data()))
                     : glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
  newNode->scale = node.scale.size() == 3
                       ? glm::vec3(glm::make_vec3(node.scale.data()))
                       : glm::vec3(1.0f);
  if (node.matrix.size() == 16) {
    newNode->matrix = glm::make_mat4x4(node.matrix.data());
  }

  for (int childIdx : node.children) {
    if (auto result = LoadNode(newNode, scene, model.nodes[childIdx],
                               static_cast<uint32_t>(childIdx), model);
        result.err()) {
      return result;
    }
  }

  if (node.mesh > -1) {
    const tinygltf::Mesh& gltfMesh = model.meshes[node.mesh];
    auto newMesh = std::make_shared<Mgtt::Rendering::Mesh>();

    for (const tinygltf::Primitive& primitive : gltfMesh.primitives) {
      const uint32_t kIndexStart =
          static_cast<uint32_t>(newMesh->indices.size());
      const uint32_t kVertexStart =
          static_cast<uint32_t>(newMesh->vertexPositionAttribs.size());
      uint32_t indexCount = 0;
      uint32_t vertexCount = 0;

      glm::vec3 posMin(FLT_MAX);
      glm::vec3 posMax(-FLT_MAX);

      const auto& posAccessor =
          model.accessors[primitive.attributes.at("POSITION")];
      const auto& posView = model.bufferViews[posAccessor.bufferView];
      const auto* bufferPos = reinterpret_cast<const float*>(
          model.buffers[posView.buffer].data.data() + posAccessor.byteOffset +
          posView.byteOffset);
      const int32_t kPosByteStride =
          (posAccessor.ByteStride(posView) != 0)
              ? posAccessor.ByteStride(posView) /
                    static_cast<int32_t>(sizeof(float))
              : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);
      posMin = glm::make_vec3(posAccessor.minValues.data());
      posMax = glm::make_vec3(posAccessor.maxValues.data());
      vertexCount = static_cast<uint32_t>(posAccessor.count);

      const float* bufferNormals = nullptr;
      int32_t normByteStride = 0;
      if (auto iter = primitive.attributes.find("NORMAL");
          iter != primitive.attributes.end()) {
        const auto& acc = model.accessors[iter->second];
        const auto& view = model.bufferViews[acc.bufferView];
        bufferNormals = reinterpret_cast<const float*>(
            model.buffers[view.buffer].data.data() + acc.byteOffset +
            view.byteOffset);
        normByteStride =
            (acc.ByteStride(view) != 0)
                ? acc.ByteStride(view) / static_cast<int32_t>(sizeof(float))
                : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);
      }

      const float* bufferUV = nullptr;
      int32_t uvByteStride = 0;
      if (auto iter = primitive.attributes.find("TEXCOORD_0");
          iter != primitive.attributes.end()) {
        const auto& acc = model.accessors[iter->second];
        const auto& view = model.bufferViews[acc.bufferView];
        bufferUV = reinterpret_cast<const float*>(
            model.buffers[view.buffer].data.data() + acc.byteOffset +
            view.byteOffset);
        uvByteStride =
            (acc.ByteStride(view) != 0)
                ? acc.ByteStride(view) / static_cast<int32_t>(sizeof(float))
                : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC2);
      }

      for (size_t vtx = 0; vtx < posAccessor.count; ++vtx) {
        newMesh->vertexPositionAttribs.push_back(
            glm::make_vec3(&bufferPos[vtx * kPosByteStride]));
        newMesh->vertexNormalAttribs.push_back(
            (bufferNormals != nullptr)
                ? glm::normalize(
                      glm::make_vec3(&bufferNormals[vtx * normByteStride]))
                : glm::vec3(0.0f));
        newMesh->vertexTextureAttribs.push_back(
            (bufferUV != nullptr)
                ? glm::make_vec2(&bufferUV[vtx * uvByteStride])
                : glm::vec2(0.0f));
      }

      const bool kHasIndices = primitive.indices > -1;
      if (kHasIndices) {
        const auto& acc = model.accessors[primitive.indices];
        const auto& bufView = model.bufferViews[acc.bufferView];
        indexCount = static_cast<uint32_t>(acc.count);
        const void* dataPtr = model.buffers[bufView.buffer].data.data() +
                              acc.byteOffset + bufView.byteOffset;

        switch (static_cast<GLTFParameterType>(acc.componentType)) {
          case GLTFParameterType::UNSIGNED_INT: {
            const auto* buf = static_cast<const uint32_t*>(dataPtr);
            for (size_t idx = 0; idx < acc.count; ++idx) {
              newMesh->indices.push_back(buf[idx] + kVertexStart);
            }
            break;
          }
          case GLTFParameterType::UNSIGNED_SHORT: {
            const auto* buf = static_cast<const uint16_t*>(dataPtr);
            for (size_t idx = 0; idx < acc.count; ++idx) {
              newMesh->indices.push_back(buf[idx] + kVertexStart);
            }
            break;
          }
          case GLTFParameterType::UNSIGNED_BYTE: {
            const auto* buf = static_cast<const uint8_t*>(dataPtr);
            for (size_t idx = 0; idx < acc.count; ++idx) {
              newMesh->indices.push_back(buf[idx] + kVertexStart);
            }
            break;
          }
          default:
            return Mgtt::Common::Result<void>::Err(
                "Unsupported index component type in primitive");
        }
      }

      Mgtt::Rendering::MeshPrimitive prim;
      prim.firstIndex = kIndexStart;
      prim.indexCount = indexCount;
      prim.vertexCount = vertexCount;
      prim.hasIndices = kHasIndices;
      prim.name = node.name;
      prim.aabb.min = posMin;
      prim.aabb.max = posMax;
      if (primitive.material > -1) {
        prim.pbrMaterial = std::move(scene.materials[primitive.material]);
      }
      newMesh->meshPrimitives.push_back(std::move(prim));
    }

    for (const auto& meshPrim : newMesh->meshPrimitives) {
      newMesh->aabb.min = glm::min(newMesh->aabb.min, meshPrim.aabb.min);
      newMesh->aabb.max = glm::max(newMesh->aabb.max, meshPrim.aabb.max);
    }

    // GPU upload deferred to SceneUploader::Upload()
    newNode->mesh = std::move(newMesh);
  }

  if (parent != nullptr) {
    parent->children.push_back(std::move(newNode));
  } else {
    std::cout << "Allocated node " << newNode->name << " with index "
              << nodeIndex << '\n';
    scene.nodes.push_back(std::move(newNode));
  }
  return Mgtt::Common::Result<void>::Ok();
}

void Mgtt::Rendering::GltfSceneImporter::UpdateNodeMeshMatrices(
    const std::shared_ptr<Mgtt::Rendering::Node>& node) {
  if (node->mesh != nullptr) {
    node->InitialTransform();
  }
  for (const auto& child : node->children) {
    UpdateNodeMeshMatrices(child);
  }
}

void Mgtt::Rendering::GltfSceneImporter::CalculateSceneDimensions(
    Mgtt::Rendering::Scene& scene) {
  for (const auto& node : scene.nodes) {
    CalculateSceneNodesAABBs(node);
  }
  for (const auto& node : scene.nodes) {
    CalculateSceneAABB(scene, node);
  }
}

void Mgtt::Rendering::GltfSceneImporter::CalculateSceneAABB(
    Mgtt::Rendering::Scene& scene,
    const std::shared_ptr<Mgtt::Rendering::Node>& node) {
  if (node->mesh != nullptr) {
    scene.aabb.min = glm::min(scene.aabb.min, node->mesh->aabb.min);
    scene.aabb.max = glm::max(scene.aabb.max, node->mesh->aabb.max);
  }
  for (const auto& child : node->children) {
    CalculateSceneAABB(scene, child);
  }
}

void Mgtt::Rendering::GltfSceneImporter::CalculateSceneNodesAABBs(
    const std::shared_ptr<Mgtt::Rendering::Node>& node) {
  if (node->mesh != nullptr) {
    node->mesh->aabb.CalculateBoundingBox(node->GetGlobalMatrix());
  }
  for (const auto& child : node->children) {
    CalculateSceneNodesAABBs(child);
  }
}
