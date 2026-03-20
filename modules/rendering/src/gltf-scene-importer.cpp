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

  const std::string pathStr(path);
  mgttScene.path = pathStr;

  // --- validate suffix ---
  auto hasSuffix = [&](std::string_view s) {
    return pathStr.size() >= s.size() &&
           pathStr.compare(pathStr.size() - s.size(), s.size(), s) == 0;
  };
  if (!hasSuffix(".gltf") && !hasSuffix(".GLTF") && !hasSuffix(".glb") &&
      !hasSuffix(".GLB")) {
    return Mgtt::Common::Result<void>::Err("Unsupported file suffix: " +
                                           pathStr);
  }

  const bool binary = hasSuffix(".glb") || hasSuffix(".GLB");

  std::string err, warn;
  tinygltf::Model gltfModel;
  tinygltf::TinyGLTF gltfContext;

  const bool fileLoaded = binary
                              ? gltfContext.LoadBinaryFromFile(
                                    &gltfModel, &err, &warn, pathStr.c_str())
                              : gltfContext.LoadASCIIFromFile(
                                    &gltfModel, &err, &warn, pathStr.c_str());

  if (!fileLoaded) {
    Clear(mgttScene);
    return Mgtt::Common::Result<void>::Err(
        "Failed to load glTF file: " + pathStr +
        (err.empty() ? "" : " — " + err));
  }

  // --- textures & materials ---
  if (auto r = LoadTextures(mgttScene, gltfModel); r.err()) {
    Clear(mgttScene);
    return r;
  }
  LoadMaterials(mgttScene, gltfModel);

  // --- nodes ---
  const tinygltf::Scene& scene =
      gltfModel
          .scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];

  for (size_t i = 0; i < scene.nodes.size(); ++i) {
    const tinygltf::Node& node = gltfModel.nodes[scene.nodes[i]];
    if (auto r = LoadNode(nullptr, mgttScene, node,
                          static_cast<uint32_t>(scene.nodes[i]), gltfModel);
        r.err()) {
      Clear(mgttScene);
      return r;
    }
  }

  for (const auto& node : mgttScene.nodes) {
    UpdateNodeMeshMatrices(node);
  }
  CalculateSceneDimensions(mgttScene);

  mgttScene.aabb.center = (mgttScene.aabb.min + mgttScene.aabb.max) * 0.5f;

  const glm::vec3 tmpScale = mgttScene.aabb.max - mgttScene.aabb.min;
  mgttScene.aabb.scale =
      (tmpScale.x <= 0.0f || tmpScale.y <= 0.0f || tmpScale.z <= 0.0f)
          ? 1.0f
          : glm::max(tmpScale.x, glm::max(tmpScale.y, tmpScale.z));

  std::cout << "Scale equals " << mgttScene.aabb.scale << '\n';
  return Mgtt::Common::Result<void>::Ok();
}

void Mgtt::Rendering::GltfSceneImporter::Clear(Mgtt::Rendering::Scene& scene) {
  scene.Clear();
}

std::string Mgtt::Rendering::GltfSceneImporter::ExtractFolderPath(
    std::string_view path) const {
  const std::string pathStr(path);
  const size_t sep = pathStr.find_last_of("\\/");
  return sep != std::string::npos ? pathStr.substr(0, sep + 1) : "";
}

void Mgtt::Rendering::GltfSceneImporter::FreeTextureData(
    Mgtt::Rendering::Texture& texture) {
  if (texture.data) {
    stbi_image_free(texture.data);
    texture.data = nullptr;
  }
}

Mgtt::Common::Result<void> Mgtt::Rendering::GltfSceneImporter::LoadTextures(
    Mgtt::Rendering::Scene& scene, tinygltf::Model& gltfModel) {
  const auto folderPath = ExtractFolderPath(scene.path);

  for (tinygltf::Texture& tex : gltfModel.textures) {
    tinygltf::Image& image = gltfModel.images[tex.source];

    Mgtt::Rendering::Texture texture;
    texture.name = image.name;
    texture.path = folderPath + image.uri;
    texture.width = image.width;
    texture.height = image.height;
    texture.nrComponents = image.component;
    texture.data = stbi_load(texture.path.c_str(), &texture.width,
                             &texture.height, &texture.nrComponents, 0);

    if (!texture.data) {
      return Mgtt::Common::Result<void>::Err("Failed to load texture: " +
                                             texture.path);
    }

    SetupTexture(texture);
    FreeTextureData(texture);
    scene.textureMap[image.uri] = texture;
  }

  std::cout << "All textures allocated for scene " << scene.path << '\n';
  return Mgtt::Common::Result<void>::Ok();
}

void Mgtt::Rendering::GltfSceneImporter::SetupTexture(
    Mgtt::Rendering::Texture& texture) {
  if (!texture.data) {
    std::cout << "SetupTexture: data is null, skipping\n";
    return;
  }

  GLenum format = GL_RGBA;
  switch (texture.nrComponents) {
    case 1:
      format = GL_RED;
      break;
    case 2:
      format = GL_RG;
      break;
    case 3:
      format = GL_RGB;
      break;
    default:
      break;
  }

  glGenTextures(1, &texture.id);
  glBindTexture(GL_TEXTURE_2D, texture.id);
  glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(format), texture.width,
               texture.height, 0, format, GL_UNSIGNED_BYTE, texture.data);
  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Mgtt::Rendering::GltfSceneImporter::LoadMaterials(
    Mgtt::Rendering::Scene& scene, tinygltf::Model& gltfModel) {
  auto lookupTex = [&](int texIndex) -> Mgtt::Rendering::Texture {
    if (texIndex < 0) return Texture{};
    const auto& uri = gltfModel.images[gltfModel.textures[texIndex].source].uri;
    auto it = scene.textureMap.find(uri);
    return it != scene.textureMap.end() ? it->second : Texture{};
  };

  for (const tinygltf::Material& material : gltfModel.materials) {
    Mgtt::Rendering::PbrMaterial pbr;
    pbr.alphaCutoff = static_cast<float>(material.alphaCutoff);
    pbr.doubleSided = material.doubleSided;

    if (material.alphaMode == "MASK")
      pbr.alphaMode = AlphaMode::MASK;
    else if (material.alphaMode == "BLEND")
      pbr.alphaMode = AlphaMode::BLEND;
    else if (material.alphaMode == "OPAQUE")
      pbr.alphaMode = AlphaMode::OPAQ;
    else
      pbr.alphaMode = AlphaMode::NONE;

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

Mgtt::Common::Result<void> Mgtt::Rendering::GltfSceneImporter::SetupMesh(
    std::shared_ptr<Mgtt::Rendering::Mesh>& mesh, uint32_t shaderId) {
  if (!mesh) {
    // Not an error — a node simply has no geometry.
    return Mgtt::Common::Result<void>::Ok();
  }
  if (HasValuesGreaterThanZero(
          {mesh->ebo, mesh->pos, mesh->normal, mesh->tex})) {
    return Mgtt::Common::Result<void>::Err(
        "Mesh GL buffers must be 0 before SetupMesh (already uploaded?)");
  }
  if (mesh->vertexPositionAttribs.empty()) {
    return Mgtt::Common::Result<void>::Err(
        "Mesh has no vertex position attributes");
  }

  glGenVertexArrays(1, &mesh->vao);
  glGenBuffers(1, &mesh->pos);
  glGenBuffers(1, &mesh->normal);
  glGenBuffers(1, &mesh->tex);
  glGenBuffers(1, &mesh->ebo);

  glBindVertexArray(mesh->vao);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               static_cast<GLsizeiptr>(mesh->indices.size() * sizeof(uint32_t)),
               mesh->indices.data(), GL_STATIC_DRAW);

  auto uploadAttrib = [&](uint32_t buf, auto& attribs, const char* name,
                          GLint components) {
    using T = typename std::decay_t<decltype(attribs)>::value_type;
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(attribs.size() * sizeof(T)),
                 attribs.data(), GL_STATIC_DRAW);
    const uint32_t loc = glGetAttribLocation(shaderId, name);
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, components, GL_FLOAT, GL_FALSE, sizeof(T),
                          nullptr);
  };

  uploadAttrib(mesh->pos, mesh->vertexPositionAttribs, "inVertexPosition", 3);
  uploadAttrib(mesh->normal, mesh->vertexNormalAttribs, "inVertexNormal", 3);
  uploadAttrib(mesh->tex, mesh->vertexTextureAttribs,
               "inVertexTextureCoordinates", 2);

  // Re-bind EBO inside the VAO so it is recorded in the VAO state.
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
  glBindVertexArray(0);

  return Mgtt::Common::Result<void>::Ok();
}

Mgtt::Common::Result<void> Mgtt::Rendering::GltfSceneImporter::LoadNode(
    std::shared_ptr<Mgtt::Rendering::Node> parent,
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
    if (auto r = LoadNode(newNode, scene, model.nodes[childIdx],
                          static_cast<uint32_t>(childIdx), model);
        r.err()) {
      return r;
    }
  }

  if (node.mesh > -1) {
    const tinygltf::Mesh& gltfMesh = model.meshes[node.mesh];
    auto newMesh = std::make_shared<Mgtt::Rendering::Mesh>();

    for (const tinygltf::Primitive& primitive : gltfMesh.primitives) {
      const uint32_t indexStart =
          static_cast<uint32_t>(newMesh->indices.size());
      const uint32_t vertexStart =
          static_cast<uint32_t>(newMesh->vertexPositionAttribs.size());
      uint32_t indexCount = 0;
      uint32_t vertexCount = 0;

      glm::vec3 posMin(FLT_MAX);
      glm::vec3 posMax(-FLT_MAX);

      // --- position (required) ---
      const auto& posAccessor =
          model.accessors[primitive.attributes.at("POSITION")];
      const auto& posView = model.bufferViews[posAccessor.bufferView];
      const auto* bufferPos = reinterpret_cast<const float*>(
          model.buffers[posView.buffer].data.data() + posAccessor.byteOffset +
          posView.byteOffset);
      const int32_t posByteStride =
          posAccessor.ByteStride(posView)
              ? posAccessor.ByteStride(posView) /
                    static_cast<int32_t>(sizeof(float))
              : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);
      posMin = glm::make_vec3(posAccessor.minValues.data());
      posMax = glm::make_vec3(posAccessor.maxValues.data());
      vertexCount = static_cast<uint32_t>(posAccessor.count);

      // --- normal (optional) ---
      const float* bufferNormals = nullptr;
      int32_t normByteStride = 0;
      if (auto it = primitive.attributes.find("NORMAL");
          it != primitive.attributes.end()) {
        const auto& acc = model.accessors[it->second];
        const auto& view = model.bufferViews[acc.bufferView];
        bufferNormals = reinterpret_cast<const float*>(
            model.buffers[view.buffer].data.data() + acc.byteOffset +
            view.byteOffset);
        normByteStride =
            acc.ByteStride(view)
                ? acc.ByteStride(view) / static_cast<int32_t>(sizeof(float))
                : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);
      }

      // --- texcoord (optional) ---
      const float* bufferUV = nullptr;
      int32_t uvByteStride = 0;
      if (auto it = primitive.attributes.find("TEXCOORD_0");
          it != primitive.attributes.end()) {
        const auto& acc = model.accessors[it->second];
        const auto& view = model.bufferViews[acc.bufferView];
        bufferUV = reinterpret_cast<const float*>(
            model.buffers[view.buffer].data.data() + acc.byteOffset +
            view.byteOffset);
        uvByteStride =
            acc.ByteStride(view)
                ? acc.ByteStride(view) / static_cast<int32_t>(sizeof(float))
                : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC2);
      }

      // --- fill vertex arrays ---
      for (size_t v = 0; v < posAccessor.count; ++v) {
        newMesh->vertexPositionAttribs.push_back(
            glm::make_vec3(&bufferPos[v * posByteStride]));
        newMesh->vertexNormalAttribs.push_back(
            bufferNormals ? glm::normalize(glm::make_vec3(
                                &bufferNormals[v * normByteStride]))
                          : glm::vec3(0.0f));
        newMesh->vertexTextureAttribs.push_back(
            bufferUV ? glm::make_vec2(&bufferUV[v * uvByteStride])
                     : glm::vec2(0.0f));
      }

      // --- indices ---
      const bool hasIndices = primitive.indices > -1;
      if (hasIndices) {
        const auto& acc = model.accessors[primitive.indices];
        const auto& bv = model.bufferViews[acc.bufferView];
        indexCount = static_cast<uint32_t>(acc.count);
        const void* dataPtr = model.buffers[bv.buffer].data.data() +
                              acc.byteOffset + bv.byteOffset;

        switch (static_cast<GLTFParameterType>(acc.componentType)) {
          case GLTFParameterType::UNSIGNED_INT: {
            const auto* buf = static_cast<const uint32_t*>(dataPtr);
            for (size_t i = 0; i < acc.count; ++i)
              newMesh->indices.push_back(buf[i] + vertexStart);
            break;
          }
          case GLTFParameterType::UNSIGNED_SHORT: {
            const auto* buf = static_cast<const uint16_t*>(dataPtr);
            for (size_t i = 0; i < acc.count; ++i)
              newMesh->indices.push_back(buf[i] + vertexStart);
            break;
          }
          case GLTFParameterType::UNSIGNED_BYTE: {
            const auto* buf = static_cast<const uint8_t*>(dataPtr);
            for (size_t i = 0; i < acc.count; ++i)
              newMesh->indices.push_back(buf[i] + vertexStart);
            break;
          }
          default:
            return Mgtt::Common::Result<void>::Err(
                "Unsupported index component type in primitive");
        }
      }

      Mgtt::Rendering::MeshPrimitive prim;
      prim.firstIndex = indexStart;
      prim.indexCount = indexCount;
      prim.vertexCount = vertexCount;
      prim.hasIndices = hasIndices;
      prim.name = node.name;
      prim.aabb.min = posMin;
      prim.aabb.max = posMax;
      if (primitive.material > -1) {
        prim.pbrMaterial = scene.materials[primitive.material];
      }
      newMesh->meshPrimitives.push_back(std::move(prim));
    }

    for (const auto& mp : newMesh->meshPrimitives) {
      newMesh->aabb.min = glm::min(newMesh->aabb.min, mp.aabb.min);
      newMesh->aabb.max = glm::max(newMesh->aabb.max, mp.aabb.max);
    }

    if (auto r = SetupMesh(newMesh, scene.shader.GetProgramId()); r.err()) {
      return r;
    }
    newNode->mesh = std::move(newMesh);
  }

  if (parent) {
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
  if (node->mesh) {
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
  if (node->mesh) {
    scene.aabb.min = glm::min(scene.aabb.min, node->mesh->aabb.min);
    scene.aabb.max = glm::max(scene.aabb.max, node->mesh->aabb.max);
  }
  for (const auto& child : node->children) {
    CalculateSceneAABB(scene, child);
  }
}

void Mgtt::Rendering::GltfSceneImporter::CalculateSceneNodesAABBs(
    const std::shared_ptr<Mgtt::Rendering::Node>& node) {
  if (node->mesh) {
    node->mesh->aabb.CalculateBoundingBox(node->GetGlobalMatrix());
  }
  for (const auto& child : node->children) {
    CalculateSceneNodesAABBs(child);
  }
}
