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

#pragma once

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <GL/glew.h>
#endif>
#include <iscene-importer.h>
#include <stb_image.h>
#include <tiny_gltf.h>
#include <utils.h>

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace Mgtt::Rendering {
/**
 * Enum class representing parameter types used in GLTF (e.g. BYTE,
 * UNSIGNED_INT).
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
 * @brief Implementation of the ISceneImporter interface for importing 3D
 * scenes.
 */
class GltfSceneImporter : public ISceneImporter {
 public:
  /**
   * @brief Default constructor for the scene importer.
   */
  GltfSceneImporter() = default;

  /**
   * @brief Load the 3D scene from a specified file path.
   *
   * @param path The file path from which to load the 3D scene.
   * @param scene Reference to an instance of the loaded 3D scene.
   */
  void Load(Mgtt::Rendering::Scene& mgttScene,
            const std::string& path) override;

  /**
   * @brief Clear the allocated resources in RAM and VRAM for the scene object.
   * 
   * @param scene Reference to an instance of a 3D scene to be cleared.
   */
  void Clear(Mgtt::Rendering::Scene& scene) override;

 private:
  /**
   * @brief Extracts the folder path from a given file path.
   * 
   * @param path The full file path from which to extract the folder path.
   * @return The extracted folder path. If no directory separator is found,
   *         an empty string is returned.
   **/
  std::string ExtractFolderPath(const std::string& path);

  /**
   * @brief Load texture from the provided glTF model.
   *
   * @param scene Reference to an instance of the loaded 3D scene to be updated.
   * @param gltfModel The glTF model containing node information.
   *
   */
  void LoadTextures(Mgtt::Rendering::Scene& scene, tinygltf::Model& gltfModel);

  /**
   * @brief Sets up a texture for rendering.
   * 
   * @param texture A reference to the `Mgtt::Rendering::Texture` object to
   * modify.
   */
  void SetupTexture(Mgtt::Rendering::Texture& texture);

  /**
   * @brief Load materials from the provided glTF model.
   * 
   * @param scene Reference to an instance of the loaded 3D scene to be updated.
   * @param gltfModel The glTF model containing node information.
   */
  void LoadMaterials(Mgtt::Rendering::Scene& scene, tinygltf::Model& gltfModel);

  /**
   * @brief Load nodes from the provided glTF model.
   * 
   * @param parent A shared pointer to the parent node in the 3D scene.
   * @param scene Reference to an instance of the loaded 3D scene to be updated.
   * @param node Reference to the glTF node containing information.
   * @param nodeIndex Index of the current node in the glTF model.
   * @param model Reference to the glTF model containing node information.
   */
  void LoadNode(std::shared_ptr<Mgtt::Rendering::Node> parent,
                Mgtt::Rendering::Scene& scene, const tinygltf::Node& node,
                uint32_t nodeIndex, const tinygltf::Model& model);

  /**
   * @brief Sets up a mesh for rendering, including vertex attribute
   * configuration.
   * 
   * @param mesh A shared pointer to the `Mgtt::Rendering::Mesh` object
   * representing the mesh.
   * @param shaderId An unsigned 32-bit integer representing the shader ID.
   */
  void SetupMesh(std::shared_ptr<Mgtt::Rendering::Mesh>& mesh,
                 uint32_t shaderId);

  /**
   * @brief Clear the RAM resources associated with the Texture object.
   * 
   * @param texture Reference to the Texture object for which RAM resources
   * should be cleared.
   */
  void Clear(Mgtt::Rendering::Texture& texture);

  /**
   * @brief Updates the mesh matrices of the given node and its child nodes
   * recursively
   *
   * @param node A shared pointer to the node whose mesh matrices need to be
   * updated.
   */
  void UpdateNodeMeshMatrices(std::shared_ptr<Mgtt::Rendering::Node> node);

  /**
   * @brief Calculates the dimensions of the entire scene.
   * 
   * @param scene Reference to an instance of the loaded 3D scene to be updated.
   */
  void CalculateSceneDimensions(Mgtt::Rendering::Scene& scene);

  /**
   * @brief Calculates the axis-aligned bounding box (AABB) of the entire scene.
   * 
   * @param scene Reference to an instance of the loaded 3D scene to be updated.
   * @param node A shared pointer to the node
   */
  void CalculateSceneAABB(Mgtt::Rendering::Scene& scene,
                          std::shared_ptr<Mgtt::Rendering::Node> node);

  /**
   * @brief Calculates the axis-aligned bounding boxes (AABBs) for each node in
   * the scene.
   * 
   * @param node A shared pointer to the node
   */
  void CalculateSceneNodesAABBs(std::shared_ptr<Mgtt::Rendering::Node> node);
};
}  // namespace Mgtt::Rendering
