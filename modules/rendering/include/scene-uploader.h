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
#include <result.h>
#include <scene.h>
#include <stb_image.h>
#include <texture.h>

namespace Mgtt::Rendering {

/**
 * @brief Uploads a CPU-side parsed scene to the GPU.
 *
 * Owns all GL allocation logic that was previously mixed into
 * GltfSceneImporter. Call after GltfSceneImporter::Load() has populated
 * the scene on the CPU side.
 */
class SceneUploader {
 public:
  SceneUploader() = default;
  ~SceneUploader() = default;

  SceneUploader(const SceneUploader&) = delete;
  SceneUploader& operator=(const SceneUploader&) = delete;
  SceneUploader(SceneUploader&&) = delete;
  SceneUploader& operator=(SceneUploader&&) = delete;

  /**
   * @brief Upload all textures and meshes in the scene to the GPU.
   *
   * @param scene Scene whose textureMap and nodes will be uploaded.
   * @return Ok on success, Err with a descriptive message on failure.
   */
  [[nodiscard]] Mgtt::Common::Result<void> Upload(
      Mgtt::Rendering::Scene& scene);

 private:
  /**
   * @brief Allocate a GL texture from already-loaded CPU image data.
   *
   * @param texture Texture whose data pointer and metadata are set.
   */
  void UploadTexture(Mgtt::Rendering::Texture& texture);

  /**
   * @brief Upload mesh vertex data to the GPU and configure VAO attributes.
   *
   * @param mesh     Mesh to upload.
   * @param shaderId Compiled shader program whose attribute locations are used.
   * @return Ok on success, Err if the mesh is in an invalid state.
   */
  [[nodiscard]] Mgtt::Common::Result<void> UploadMesh(
      std::shared_ptr<Mgtt::Rendering::Mesh>& mesh, uint32_t shaderId);

  /**
   * @brief Recursively upload meshes for a node and its children.
   *
   * @param node    Root of the subtree to upload.
   * @param shaderId Compiled shader program id.
   * @return Ok on success, Err if any mesh upload fails.
   */
  [[nodiscard]] Mgtt::Common::Result<void> UploadNode(
      const std::shared_ptr<Mgtt::Rendering::Node>& node, uint32_t shaderId);

  void PatchMaterialIds(
      const std::shared_ptr<Mgtt::Rendering::Node>& node,
      const std::map<std::string, Mgtt::Rendering::Texture>& textureMap);
};

}  // namespace Mgtt::Rendering
