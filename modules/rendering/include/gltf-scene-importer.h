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

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <iscene-importer.h>
#include <stb_image.h>
#include <tiny_gltf.h>
#include <utils.h>

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
 * @brief Concrete ISceneImporter for loading glTF / glb scenes.
 */
class GltfSceneImporter : public ISceneImporter {
 public:
  GltfSceneImporter() = default;
  ~GltfSceneImporter() = default;

  GltfSceneImporter(const GltfSceneImporter&) = delete;
  GltfSceneImporter& operator=(const GltfSceneImporter&) = delete;
  GltfSceneImporter(GltfSceneImporter&&) = delete;
  GltfSceneImporter& operator=(GltfSceneImporter&&) = delete;

  /**
   * @brief Load a glTF or glb scene from disk.
   *
   * @param scene Reference to the 3D scene to populate.
   * @param path  Path to the .gltf or .glb file.
   * @return Ok on success, Err with a descriptive message on failure.
   */
  [[nodiscard]] Mgtt::Common::Result<void> Load(Mgtt::Rendering::Scene& scene,
                                                std::string_view path) override;

  /**
   * @brief Release all RAM and VRAM resources held by the scene.
   *
   * @param scene Reference to the 3D scene to clear.
   */
  void Clear(Mgtt::Rendering::Scene& scene) override;

 private:
  /**
   * @brief Extract the directory portion of a file path.
   *
   * @param path Full file path.
   * @return Directory path including trailing separator, or empty string.
   */
  [[nodiscard]] std::string ExtractFolderPath(std::string_view path) const;

  /**
   * @brief Upload all textures referenced by the glTF model to the GPU.
   *
   * @param scene     Scene whose textureMap will be populated.
   * @param gltfModel Source glTF model.
   * @return Ok on success, Err on the first texture that fails to load.
   */
  [[nodiscard]] Mgtt::Common::Result<void> LoadTextures(
      Mgtt::Rendering::Scene& scene, tinygltf::Model& gltfModel);

  /**
   * @brief Allocate a GL texture from already-loaded CPU image data.
   *
   * @param texture Texture whose data pointer and metadata are set.
   */
  void SetupTexture(Mgtt::Rendering::Texture& texture);

  /**
   * @brief Build PBR materials from the glTF model and append them to the
   *        scene's material list.
   *
   * @param scene     Scene whose materials vector will be populated.
   * @param gltfModel Source glTF model.
   */
  void LoadMaterials(Mgtt::Rendering::Scene& scene, tinygltf::Model& gltfModel);

  /**
   * @brief Recursively load a node and its children from the glTF model.
   *
   * @param parent    Parent node, or nullptr for root nodes.
   * @param scene     Scene to append nodes to.
   * @param node      glTF node to load.
   * @param nodeIndex Index of the node in the glTF model.
   * @param model     Source glTF model.
   * @return Ok on success, Err if mesh setup fails.
   */
  [[nodiscard]] Mgtt::Common::Result<void> LoadNode(
      std::shared_ptr<Mgtt::Rendering::Node> parent,
      Mgtt::Rendering::Scene& scene, const tinygltf::Node& node,
      uint32_t nodeIndex, const tinygltf::Model& model);

  /**
   * @brief Upload mesh vertex data to the GPU and configure VAO attributes.
   *
   * @param mesh     Mesh to set up.
   * @param shaderId Compiled shader program whose attribute locations are used.
   * @return Ok on success, Err if the mesh is in an invalid state.
   */
  [[nodiscard]] Mgtt::Common::Result<void> SetupMesh(
      std::shared_ptr<Mgtt::Rendering::Mesh>& mesh, uint32_t shaderId);

  /**
   * @brief Free CPU-side image data held by a texture.
   *
   * @param texture Texture whose stbi data pointer will be freed.
   */
  void FreeTextureData(Mgtt::Rendering::Texture& texture);

  /**
   * @brief Recursively propagate global transforms down to each mesh matrix.
   *
   * @param node Root of the subtree to update.
   */
  void UpdateNodeMeshMatrices(
      const std::shared_ptr<Mgtt::Rendering::Node>& node);

  /**
   * @brief Compute the scene-level AABB by iterating all nodes.
   *
   * @param scene Scene whose aabb fields will be updated.
   */
  void CalculateSceneDimensions(Mgtt::Rendering::Scene& scene);

  /**
   * @brief Expand the scene AABB to include the given node's mesh AABB.
   *
   * @param scene Scene whose aabb fields will be updated.
   * @param node  Node to include.
   */
  void CalculateSceneAABB(Mgtt::Rendering::Scene& scene,
                          const std::shared_ptr<Mgtt::Rendering::Node>& node);

  /**
   * @brief Recompute the world-space AABB for a node and its children.
   *
   * @param node Root of the subtree to update.
   */
  void CalculateSceneNodesAABBs(
      const std::shared_ptr<Mgtt::Rendering::Node>& node);
};

}  // namespace Mgtt::Rendering
