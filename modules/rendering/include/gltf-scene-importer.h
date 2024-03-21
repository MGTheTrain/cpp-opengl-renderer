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

#if defined(__EMSCRIPTEN__) || defined(__ANDROID__)
#include <GLES3/gl3.h>
#else
#include <GL/glew.h>
#endif

#ifdef __ANDROID__
#include <android/log.h>
#endif

#include <iscene-importer.h>
#include <tiny_gltf.h>

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#define GLTF_COMPONENT_TYPE_BYTE (5120)
#define GLTF_COMPONENT_TYPE_UNSIGNED_BYTE (5121)
#define GLTF_COMPONENT_TYPE_SHORT (5122)
#define GLTF_COMPONENT_TYPE_UNSIGNED_SHORT (5123)
#define GLTF_COMPONENT_TYPE_INT (5124)
#define GLTF_COMPONENT_TYPE_UNSIGNED_INT (5125)
#define GLTF_COMPONENT_TYPE_FLOAT (5126)
#define GLTF_COMPONENT_TYPE_DOUBLE (5130)

#define GLTF_PARAMETER_TYPE_BYTE (5120)
#define GLTF_PARAMETER_TYPE_UNSIGNED_BYTE (5121)
#define GLTF_PARAMETER_TYPE_SHORT (5122)
#define GLTF_PARAMETER_TYPE_UNSIGNED_SHORT (5123)
#define GLTF_PARAMETER_TYPE_INT (5124)
#define GLTF_PARAMETER_TYPE_UNSIGNED_INT (5125)
#define GLTF_PARAMETER_TYPE_FLOAT (5126)

#define GLTF_TYPE_VEC2 (2)
#define GLTF_TYPE_VEC3 (3)
#define GLTF_TYPE_VEC4 (4)
#define GLTF_TYPE_MAT2 (32 + 2)
#define GLTF_TYPE_MAT3 (32 + 3)
#define GLTF_TYPE_MAT4 (32 + 4)
#define GLTF_TYPE_SCALAR (64 + 1)
#define GLTF_TYPE_VECTOR (64 + 4)
#define GLTF_TYPE_MATRIX (64 + 16)

namespace Mgtt::Rendering {
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
  GltfSceneImporter() {}

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
#ifdef __ANDROID__
  std::string logMessage;
#endif
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
  std::string ExtractFolderPath(const std::string& filePath);

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
                const uint32_t nodeIndex, const tinygltf::Model& model);

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
                 uint32_t& shaderId);

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
  void ClearRAM(Mgtt::Rendering::Texture& texture);

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
   * This function calculates the dimensions of the entire scene by utilizing
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
   * This function calculates the axis-aligned bounding box (AABB) of the entire
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
   * This function calculates the axis-aligned bounding boxes (AABBs) for each
   * node in the scene. It traverses all nodes recursively and computes the AABB
   * for each individual node based on its geometry.
   *
   * @param node A shared pointer to the node
   */
  void CalculateSceneNodesAABBs(std::shared_ptr<Mgtt::Rendering::Node> node);
};

/**
 * @brief Class for importing and managing various textures.
 *
 * The TextureManager class provides functionality for loading and managing
 * different textures, including cube maps and High Dynamic Range (HDR)
 * textures.
 * @note Take a read on: https://learnopengl.com/PBR/IBL/Diffuse-irradiance
 */
class TextureManager {
 public:
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
  void LoadFromEnvMap(Mgtt::Rendering::RenderTexturesContainer& container,
                      const std::vector<std::string>& texturePathes);

  /**
   * @brief Load an HDR texture from the given file path.
   *
   * This function loads an HDR texture from the specified file path and
   * associates it with the provided RenderTexturesContainer. It performs
   * necessary operations to make the texture available for use in rendering.
   *
   * @param container The RenderTexturesContainer to associate with the loaded
   * HDR texture.
   * @param texturePath The file path to the HDR texture.
   */
  void LoadFromHdr(Mgtt::Rendering::RenderTexturesContainer& container,
                   const std::string& texturePath);

  /**
   * @brief Load the BRDF Lookup Texture into the provided HDR texture
   * container.
   *
   * This function loads the BRDF Lookup Texture into the provided HDR texture
   * container. It performs necessary operations to make the BRDF texture
   * available for use in rendering.
   *
   * @param container The RenderTexturesContainer to associate with the loaded
   * BRDF texture.
   */
  void LoadBrdfLut(Mgtt::Rendering::RenderTexturesContainer& container);

  /**
   * @brief Clear the provided render textures container.
   *
   * This function clears the contents of the provided RenderTexturesContainer,
   * releasing any resources associated with the textures.
   *
   * @param container The RenderTexturesContainer to clear.
   */
  void Clear(Mgtt::Rendering::RenderTexturesContainer& container);

 private:
#ifdef __ANDROID__
  std::string logMessage;
#endif
  /**
   * @brief Check if any value in the given vector is greater than zero.
   *
   * The function iterates through the elements of the vector and returns true
   * if it finds at least one element greater than zero; otherwise, it returns
   * false.
   *
   * @param vec A const reference to a vector of unsigned integers to be
   * checked.
   * @return true if any value in the vector is greater than zero, false
   * otherwise.
   */
  bool HasValuesGreaterThanZero(const std::vector<unsigned int>& vec);

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
  void SetupCube(Mgtt::Rendering::RenderTexturesContainer& container);

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
  void SetupQuad(Mgtt::Rendering::RenderTexturesContainer& container);

  /**
   * @brief Generate the irradiance map for the provided HDR texture container.
   *
   * This function generates the irradiance map for the provided HDR texture
   * container. It performs necessary operations to make the irradiance map
   * available for use in rendering.
   *
   * @param container The RenderTexturesContainer to associate with the
   * generated irradiance map.
   */
  void GenerateIrradianceMap(
      Mgtt::Rendering::RenderTexturesContainer& container);
  ;
};
}  // namespace Mgtt::Rendering
