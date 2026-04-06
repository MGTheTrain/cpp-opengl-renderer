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

#ifndef __EMSCRIPTEN__
#include <stb_image.h>
#include <usd-scene-importer.h>

#include <cfloat>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>

namespace Mgtt::Rendering {

Mgtt::Common::Result<void> UsdSceneImporter::Load(Mgtt::Rendering::Scene& scene,
                                                  std::string_view path) {
  if (scene.shader.GetProgramId() == 0) {
    return Mgtt::Common::Result<void>::Err("Shader program does not exist");
  }

  const std::string kPathStr(path);

  auto hasSuffix = [&](std::string_view suffix) {
    return kPathStr.size() >= suffix.size() &&
           kPathStr.compare(kPathStr.size() - suffix.size(), suffix.size(),
                            suffix) == 0;
  };

  if (!hasSuffix(".usd") && !hasSuffix(".USD") && !hasSuffix(".usda") &&
      !hasSuffix(".USDA") && !hasSuffix(".usdc") && !hasSuffix(".USDC") &&
      !hasSuffix(".usdz") && !hasSuffix(".USDZ")) {
    return Mgtt::Common::Result<void>::Err("Unsupported file suffix: " +
                                           kPathStr);
  }

  if (!tinyusdz::IsUSD(kPathStr)) {
    return Mgtt::Common::Result<void>::Err(
        "File not found or not a USD format: " + kPathStr);
  }

  scene.path = kPathStr;

  std::string warn;
  std::string err;
  tinyusdz::Stage stage;

  if (!tinyusdz::LoadUSDFromFile(kPathStr, &stage, &warn, &err)) {
    Clear(scene);
    return Mgtt::Common::Result<void>::Err(
        "Failed to load USD file: " + kPathStr +
        (err.empty() ? "" : " — " + err));
  }
  if (!warn.empty()) {
    std::cout << "USD load warning: " << warn << '\n';
  }

  tinyusdz::tydra::RenderScene renderScene;
  tinyusdz::tydra::RenderSceneConverter converter;
  tinyusdz::tydra::RenderSceneConverterEnv env(stage);

  env.mesh_config.triangulate = true;
  env.mesh_config.build_vertex_indices = true;
  env.scene_config.load_texture_assets = true;

  const size_t kSep = kPathStr.find_last_of("\\/");
  const std::string kBaseDir =
      kSep != std::string::npos ? kPathStr.substr(0, kSep + 1) : "";
  env.set_search_paths({kBaseDir});

  if (!converter.ConvertToRenderScene(env, &renderScene)) {
    Clear(scene);
    return Mgtt::Common::Result<void>::Err(
        "Failed to convert USD stage to RenderScene: " + converter.GetError());
  }

  const std::string kConverterWarn = converter.GetWarning();
  if (!kConverterWarn.empty()) {
    std::cout << "RenderScene converter warning: " << kConverterWarn << '\n';
  }

  if (auto r = LoadTextures(scene, renderScene); r.err()) {
    Clear(scene);
    return r;
  }

  LoadMaterials(scene, renderScene);

  if (auto r = LoadMeshes(scene, renderScene); r.err()) {
    Clear(scene);
    return r;
  }

  // Compute scene AABB from all loaded meshes
  scene.aabb.min = glm::vec3(FLT_MAX);
  scene.aabb.max = glm::vec3(-FLT_MAX);

  for (const auto& node : scene.nodes) {
    if (node->mesh != nullptr) {
      scene.aabb.min = glm::min(scene.aabb.min, node->mesh->aabb.min);
      scene.aabb.max = glm::max(scene.aabb.max, node->mesh->aabb.max);
    }
  }

  scene.aabb.center = (scene.aabb.min + scene.aabb.max) * 0.5f;

  const glm::vec3 kTmpScale = scene.aabb.max - scene.aabb.min;
  scene.aabb.scale =
      (kTmpScale.x <= 0.0f || kTmpScale.y <= 0.0f || kTmpScale.z <= 0.0f)
          ? 1.0f
          : glm::max(kTmpScale.x, glm::max(kTmpScale.y, kTmpScale.z));

  std::cout << "USD scene loaded: " << kPathStr
            << " (scale=" << scene.aabb.scale << ")\n";
  return Mgtt::Common::Result<void>::Ok();
}

void UsdSceneImporter::Clear(Mgtt::Rendering::Scene& scene) noexcept {
  scene.Clear();
}

Mgtt::Common::Result<void> UsdSceneImporter::LoadTextures(
    Mgtt::Rendering::Scene& scene,
    const tinyusdz::tydra::RenderScene& renderScene) {
  // Derive the directory of the USD file for resolving relative texture paths
  const size_t kSep = scene.path.find_last_of("\\/");
  const std::string kBaseDir =
      kSep != std::string::npos ? scene.path.substr(0, kSep + 1) : "";

  for (const auto& uvTex : renderScene.textures) {
    if (uvTex.texture_image_id < 0) {
      continue;
    }

    const size_t kImgIdx = static_cast<size_t>(uvTex.texture_image_id);
    if (kImgIdx >= renderScene.images.size()) {
      continue;
    }

    const auto& texImage = renderScene.images[kImgIdx];

    Mgtt::Rendering::Texture texture;
    texture.name = uvTex.prim_name;
    texture.path = texImage.asset_identifier;

    if (!texture.path.empty()) {
      // Resolve relative paths against the USD file's directory
      std::string kResolvedPath = texture.path;
      if (!kBaseDir.empty() && texture.path.find(":/") == std::string::npos &&
          texture.path.find(":\\") == std::string::npos &&
          texture.path[0] != '/') {
        kResolvedPath = kBaseDir + texture.path;
      }

      texture.data = stbi_load(kResolvedPath.c_str(), &texture.width,
                               &texture.height, &texture.nrComponents, 0);
      if (texture.data == nullptr) {
        return Mgtt::Common::Result<void>::Err(
            "Failed to load texture from path: " + kResolvedPath);
      }
    } else if (texImage.buffer_id >= 0) {
      const size_t kBufIdx = static_cast<size_t>(texImage.buffer_id);
      if (kBufIdx >= renderScene.buffers.size()) {
        return Mgtt::Common::Result<void>::Err(
            "Buffer index out of range for texture: " + texture.path);
      }

      const auto& buf = renderScene.buffers[kBufIdx];
      if (!buf.data.empty()) {
        texture.data = stbi_load_from_memory(
            reinterpret_cast<const stbi_uc*>(buf.data.data()),
            static_cast<int>(buf.data.size()), &texture.width, &texture.height,
            &texture.nrComponents, 0);

        if (texture.data == nullptr) {
          return Mgtt::Common::Result<void>::Err(
              "Failed to decode texture from buffer: " + texture.path);
        }
      }
    } else {
      continue;
    }

    scene.textureMap[uvTex.prim_name] = std::move(texture);
  }

  std::cout << "All textures loaded to RAM for USD scene " << scene.path
            << '\n';
  return Mgtt::Common::Result<void>::Ok();
}

void UsdSceneImporter::LoadMaterials(
    Mgtt::Rendering::Scene& scene,
    const tinyusdz::tydra::RenderScene& renderScene) {
  // Look up a Texture from the map by UVTexture index in renderScene.textures
  auto lookupTex = [&](int32_t texId) -> Mgtt::Rendering::Texture {
    if (texId < 0 ||
        static_cast<size_t>(texId) >= renderScene.textures.size()) {
      return Texture{};
    }
    const auto& uvTex = renderScene.textures[static_cast<size_t>(texId)];
    auto iter = scene.textureMap.find(uvTex.prim_name);
    return iter != scene.textureMap.end() ? iter->second : Texture{};
  };

  for (const auto& tydraMaterial : renderScene.materials) {
    Mgtt::Rendering::PbrMaterial pbr;
    pbr.name = tydraMaterial.name;

    // surfaceShader is a PreviewSurfaceShader
    // Each ShaderParam<T> has:
    //   .texture_id (int32_t, -1 = no texture)
    //   .value (T, the fallback/constant value)
    const auto& ss = tydraMaterial.surfaceShader;

    pbr.baseColorTexture = BaseColorTexture(
        lookupTex(ss.diffuseColor.texture_id),
        glm::vec4(ss.diffuseColor.value[0], ss.diffuseColor.value[1],
                  ss.diffuseColor.value[2], 1.0f));

    pbr.metallicRoughnessTexture =
        MetallicRoughnessTexture(lookupTex(ss.metallic.texture_id),
                                 ss.metallic.value, ss.roughness.value);

    pbr.normalTexture = NormalTexture(lookupTex(ss.normal.texture_id), 1.0f);

    pbr.occlusionTexture = OcclusionTexture(lookupTex(ss.occlusion.texture_id),
                                            ss.occlusion.value);

    pbr.emissiveTexture = EmissiveTexture(
        lookupTex(ss.emissiveColor.texture_id),
        glm::vec3(ss.emissiveColor.value[0], ss.emissiveColor.value[1],
                  ss.emissiveColor.value[2]));

    // opacityThreshold maps to alphaCutoff
    pbr.alphaCutoff = ss.opacityThreshold.value;
    pbr.doubleSided = false;  // not on RenderMaterial — default false
    pbr.alphaMode = AlphaMode::OPAQ;

    scene.materials.push_back(std::move(pbr));
  }
}

Mgtt::Common::Result<void> UsdSceneImporter::LoadMeshes(
    Mgtt::Rendering::Scene& scene,
    const tinyusdz::tydra::RenderScene& renderScene) {
  for (const auto& tydraMesh : renderScene.meshes) {
    auto newNode = std::make_shared<Mgtt::Rendering::Node>();
    newNode->name = tydraMesh.prim_name;
    newNode->matrix = glm::mat4(1.0f);  // world matrix not on RenderMesh;
                                        // use node hierarchy if needed

    auto newMesh = std::make_shared<Mgtt::Rendering::Mesh>();
    newMesh->name = tydraMesh.prim_name;

    // Vertex positions — vec3 array
    for (const auto& pt : tydraMesh.points) {
      newMesh->vertexPositionAttribs.emplace_back(pt[0], pt[1], pt[2]);
    }

    // Normals — VertexAttribute with raw float3 data
    if (!tydraMesh.normals.empty()) {
      const size_t kCount = tydraMesh.normals.vertex_count();
      const auto* kData = reinterpret_cast<const tinyusdz::value::float3*>(
          tydraMesh.normals.get_data().data());
      for (size_t i = 0; i < kCount; ++i) {
        newMesh->vertexNormalAttribs.emplace_back(kData[i][0], kData[i][1],
                                                  kData[i][2]);
      }
    } else {
      newMesh->vertexNormalAttribs.assign(newMesh->vertexPositionAttribs.size(),
                                          glm::vec3(0.0f));
    }

    // Texcoords — unordered_map<uint32_t, VertexAttribute>, slot 0 = primary
    auto tcIt = tydraMesh.texcoords.find(0);
    if (tcIt != tydraMesh.texcoords.end() && !tcIt->second.empty()) {
      const size_t kCount = tcIt->second.vertex_count();
      const auto* kData = reinterpret_cast<const tinyusdz::value::float2*>(
          tcIt->second.get_data().data());
      for (size_t i = 0; i < kCount; ++i) {
        newMesh->vertexTextureAttribs.emplace_back(kData[i][0], kData[i][1]);
      }
    } else {
      newMesh->vertexTextureAttribs.assign(
          newMesh->vertexPositionAttribs.size(), glm::vec2(0.0f));
    }

    // Indices — faceVertexIndices() returns triangulatedFaceVertexIndices
    // when triangulated, usdFaceVertexIndices otherwise
    for (const auto kIdx : tydraMesh.faceVertexIndices()) {
      newMesh->indices.push_back(kIdx);
    }

    // AABB — compute from points since RenderMesh has no extent field
    newMesh->aabb.min = glm::vec3(FLT_MAX);
    newMesh->aabb.max = glm::vec3(-FLT_MAX);
    for (const auto& pos : newMesh->vertexPositionAttribs) {
      newMesh->aabb.min = glm::min(newMesh->aabb.min, pos);
      newMesh->aabb.max = glm::max(newMesh->aabb.max, pos);
    }

    Mgtt::Rendering::MeshPrimitive prim;
    prim.name = tydraMesh.prim_name;
    prim.firstIndex = 0;
    prim.indexCount = static_cast<uint32_t>(newMesh->indices.size());
    prim.vertexCount =
        static_cast<uint32_t>(newMesh->vertexPositionAttribs.size());
    prim.hasIndices = !newMesh->indices.empty();
    prim.aabb = newMesh->aabb;

    const int kMatId = tydraMesh.material_id;
    if (kMatId >= 0 && static_cast<size_t>(kMatId) < scene.materials.size()) {
      prim.pbrMaterial =
          std::move(scene.materials[static_cast<size_t>(kMatId)]);
    }

    newMesh->meshPrimitives.push_back(std::move(prim));

    // GPU upload deferred to SceneUploader::Upload()
    newNode->mesh = std::move(newMesh);

    std::cout << "Allocated USD node " << newNode->name << '\n';
    scene.nodes.push_back(std::move(newNode));
  }

  return Mgtt::Common::Result<void>::Ok();
}

}  // namespace Mgtt::Rendering
#endif
