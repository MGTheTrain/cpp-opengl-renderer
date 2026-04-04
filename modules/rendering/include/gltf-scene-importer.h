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

#pragma once

#include <iscene-importer.h>
#include <stb_image.h>
#include <tiny_gltf.h>

#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace Mgtt::Rendering {

/**
 * @brief GLTF accessor component types.
 */
enum class GLTFParameterType {
  BYTE = 5120,
  UNSIGNED_BYTE = 5121,
  SHORT = 5122,
  UNSIGNED_SHORT = 5123,
  INT = 5124,
  UNSIGNED_INT = 5125,
  FLOAT = 5126
};

/**
 * @brief CPU-side parser for glTF / glb scenes.
 *
 * Populates a Scene with geometry, materials, and texture pixel data on the
 * CPU. Does not touch the GPU. Call SceneUploader::Upload() afterwards to
 * transfer the parsed data to GL.
 */
class GltfSceneImporter : public ISceneImporter {
 public:
  GltfSceneImporter() = default;
  ~GltfSceneImporter() noexcept = default;

  GltfSceneImporter(const GltfSceneImporter&) = delete;
  GltfSceneImporter& operator=(const GltfSceneImporter&) = delete;
  GltfSceneImporter(GltfSceneImporter&&) noexcept = default;
  GltfSceneImporter& operator=(GltfSceneImporter&&) noexcept = default;

  /**
   * @brief Parse a glTF or glb file into CPU-side scene data.
   *
   * Textures are loaded into RAM (stbi data pointers). Meshes are
   * populated with vertex and index data. No GL calls are made.
   *
   * @param scene Reference to the scene to populate.
   * @param path  Path to the .gltf or .glb file.
   * @return Ok on success, Err with a descriptive message on failure.
   */
  [[nodiscard]] Mgtt::Common::Result<void> Load(Mgtt::Rendering::Scene& scene,
                                                std::string_view path) override;

  /**
   * @brief Release all CPU and GPU resources held by the scene.
   *
   * @param scene Scene to clear.
   */
  void Clear(Mgtt::Rendering::Scene& scene) noexcept override;

 private:
  [[nodiscard]] std::string ExtractFolderPath(std::string_view path) const;

  [[nodiscard]] Mgtt::Common::Result<void> LoadTextures(
      Mgtt::Rendering::Scene& scene, tinygltf::Model& gltfModel);

  void LoadMaterials(Mgtt::Rendering::Scene& scene, tinygltf::Model& gltfModel);

  [[nodiscard]] Mgtt::Common::Result<void> LoadNode(
      const std::shared_ptr<Mgtt::Rendering::Node>& parent,
      Mgtt::Rendering::Scene& scene, const tinygltf::Node& node,
      uint32_t nodeIndex, const tinygltf::Model& model);

  void FreeTextureData(Mgtt::Rendering::Texture& texture);
  void UpdateNodeMeshMatrices(
      const std::shared_ptr<Mgtt::Rendering::Node>& node);
  void CalculateSceneDimensions(Mgtt::Rendering::Scene& scene);
  void CalculateSceneAABB(Mgtt::Rendering::Scene& scene,
                          const std::shared_ptr<Mgtt::Rendering::Node>& node);
  void CalculateSceneNodesAABBs(
      const std::shared_ptr<Mgtt::Rendering::Node>& node);
};

}  // namespace Mgtt::Rendering
