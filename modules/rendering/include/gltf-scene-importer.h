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
 * Enum class representing component types used in GLTF (e.g. BYTE,
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
 *
 * This class provides concrete implementation details for loading and clearing
 * 3D scenes.
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
   * This method overrides the corresponding method in the ISceneImporter
   * interface. It loads the 3D scene from the specified file path.
   *
   * @param path The file path from which to load the 3D scene.
   * @paramAn instance of the loaded 3D scene.
   */
  void Load(Mgtt::Rendering::Scene& mgttScene,
            const std::string& path) override;

  /**
   * @brief Clear the allocated resources in RAM and VRAM for the scene object.
   *
   * This method is used to reset the internal state of the scene.
   * @param scene A unique pointer to the scene to clear.
   */
  void Clear(Mgtt::Rendering::Scene& scene) override;

 private:
  /**
   * @brief Extracts the folder path from a given file path.
   *
   * This method takes a file path as input and extracts the folder path
   * by finding the last occurrence of the directory separator ('/' or '\\').
   *
   * @param path The full file path from which to extract the folder path.
   * @return The extracted folder path. If no directory separator is found,
   *         an empty string is returned.
   *
   * @note The function uses the platform-specific directory separator ('/' or
   *'\\').
   * @note The returned folder path includes the trailing directory separator.
   **/
  std::string ExtractFolderPath(const std::string& path);

  /**
   * @brief Load texture from the provided glTF model.
   *
   * This method loads texture from the given glTF model and updates the
   * internal representation of the 3D scene accordingly.
   *
   * @param scene A reference to the updated 3D scene after loading nodes.
   * @param gltfModel The glTF model containing node information.
   *
   */
  void LoadTextures(Mgtt::Rendering::Scene& scene, tinygltf::Model& gltfModel);

  /**
   * @brief Sets up a texture for rendering.
   *
   * It ensures that the texture data is properly uploaded to video memory
   * (VRAM) for efficient rendering.
   *
   * @param texture A reference to the `Mgtt::Rendering::Texture` object to
   * modify.
   */
  void SetupTexture(Mgtt::Rendering::Texture& texture);

  /**
   * @brief Load materials from the provided glTF model.
   *
   * This method loads materials from the given glTF model and updates the
   * internal representation of the 3D scene accordingly.
   *
   * @param scene A reference to the updated 3D scene after loading nodes.
   * @param gltfModel The glTF model containing node information.
   */
  void LoadMaterials(Mgtt::Rendering::Scene& scene, tinygltf::Model& gltfModel);

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
  void LoadNode(std::shared_ptr<Mgtt::Rendering::Node> parent,
                Mgtt::Rendering::Scene& scene, const tinygltf::Node& node,
                uint32_t nodeIndex, const tinygltf::Model& model);

  /**
   * @brief Sets up a mesh for rendering, including vertex attribute
   * configuration.
   *
   * This method prepares a mesh for rendering by configuring its vertex
   * attributes, associating it with the specified shader, and potentially
   * moving vertex data to VRAM.
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
   * This method releases the memory resources in RAM associated with the
   * Texture object, freeing up memory. It is recommended to call this method
   * when the Texture is no longer needed to avoid unnecessary memory usage.
   *
   * @param texture Reference to the Texture object for which RAM resources
   * should be cleared.
   */
  void Clear(Mgtt::Rendering::Texture& texture);

  /**
   * @brief Updates the mesh matrices of the given node and its child nodes
   * recursively when calling the InitialTransform() method
   *
   * @param node A shared pointer to the node whose mesh matrices need to be
   * updated.
   */
  void UpdateNodeMeshMatrices(std::shared_ptr<Mgtt::Rendering::Node> node);

  /**
   * @brief Calculates the dimensions of the entire scene.
   *
   * This method calculates the dimensions of the entire scene by utilizing
   * the CalculateSceneAABB() and CalculateSceneNodeAABBs() methods. It
   * traverses the nodes of the scene recursively to determine the overall size
   * of the scene. The calculated dimensions typically include the minimum and
   * maximum extents along each axis.
   *
   * @param scene Reference to the updated 3D scene after loading nodes.
   */
  void CalculateSceneDimensions(Mgtt::Rendering::Scene& scene);

  /**
   * @brief Calculates the axis-aligned bounding box (AABB) of the entire scene.
   *
   * This method calculates the axis-aligned bounding box (AABB) of the entire
   * scene. It traverses all nodes in the scene recursively and computes the
   * AABB that encapsulates all geometry within the scene. The AABB represents
   * the minimum volume box that entirely contains all objects in the scene.
   *
   * @param scene Reference to the updated 3D scene after loading nodes.
   * @param node A shared pointer to the node
   */
  void CalculateSceneAABB(Mgtt::Rendering::Scene& scene,
                          std::shared_ptr<Mgtt::Rendering::Node> node);

  /**
   * @brief Calculates the axis-aligned bounding boxes (AABBs) for each node in
   * the scene.
   *
   * This method calculates the axis-aligned bounding boxes (AABBs) for each
   * node in the scene. It traverses all nodes recursively and computes the AABB
   * for each individual node based on its geometry.
   *
   * @param node A shared pointer to the node
   */
  void CalculateSceneNodesAABBs(std::shared_ptr<Mgtt::Rendering::Node> node);
};
}  // namespace Mgtt::Rendering
