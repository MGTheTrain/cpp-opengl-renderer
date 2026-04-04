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

#include <scene-uploader.h>
#include <utils.h>

#include <iostream>
#include <map>
#include <string>

namespace Mgtt::Rendering {

Mgtt::Common::Result<void> SceneUploader::Upload(
    Mgtt::Rendering::Scene& scene) {
  // Upload textures to GPU and record their GL ids in the textureMap
  for (auto& [uri, texture] : scene.textureMap) {
    UploadTexture(texture);
  }

  // Patch material texture ids from the map back into each primitive's
  // PbrMaterial. LoadMaterials copies textures by value, so the id written
  // into the map during upload is not reflected in the material copies.
  for (auto& node : scene.nodes) {
    PatchMaterialIds(node, scene.textureMap);
  }

  // Upload meshes
  for (auto& node : scene.nodes) {
    if (auto r = UploadNode(node, scene.shader.GetProgramId()); r.err()) {
      return r;
    }
  }

  std::cout << "Scene uploaded to GPU: " << scene.path << '\n';
  return Mgtt::Common::Result<void>::Ok();
}

void SceneUploader::UploadTexture(Mgtt::Rendering::Texture& texture) {
  if (texture.data == nullptr) {
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

  stbi_image_free(texture.data);
  texture.data = nullptr;
}

Mgtt::Common::Result<void> SceneUploader::UploadMesh(
    std::shared_ptr<Mgtt::Rendering::Mesh>& mesh, uint32_t shaderId) {
  if (mesh == nullptr) {
    return Mgtt::Common::Result<void>::Ok();
  }
  if (HasValuesGreaterThanZero(
          {mesh->ebo, mesh->pos, mesh->normal, mesh->tex})) {
    return Mgtt::Common::Result<void>::Err(
        "Mesh GL buffers must be 0 before UploadMesh (already uploaded?)");
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

  auto uploadAttrib = [&](uint32_t buf, auto& attribs, const char* attrName,
                          GLint components) {
    using T = typename std::decay_t<decltype(attribs)>::value_type;
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(attribs.size() * sizeof(T)),
                 attribs.data(), GL_STATIC_DRAW);
    const uint32_t kLoc = glGetAttribLocation(shaderId, attrName);
    glEnableVertexAttribArray(kLoc);
    glVertexAttribPointer(kLoc, components, GL_FLOAT, GL_FALSE, sizeof(T),
                          nullptr);
  };

  uploadAttrib(mesh->pos, mesh->vertexPositionAttribs, "inVertexPosition", 3);
  uploadAttrib(mesh->normal, mesh->vertexNormalAttribs, "inVertexNormal", 3);
  uploadAttrib(mesh->tex, mesh->vertexTextureAttribs,
               "inVertexTextureCoordinates", 2);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
  glBindVertexArray(0);

  return Mgtt::Common::Result<void>::Ok();
}

Mgtt::Common::Result<void> SceneUploader::UploadNode(
    const std::shared_ptr<Mgtt::Rendering::Node>& node, uint32_t shaderId) {
  if (node->mesh != nullptr) {
    if (auto r = UploadMesh(node->mesh, shaderId); r.err()) {
      return r;
    }
  }
  for (auto& child : node->children) {
    if (auto r = UploadNode(child, shaderId); r.err()) {
      return r;
    }
  }
  return Mgtt::Common::Result<void>::Ok();
}

void SceneUploader::PatchMaterialIds(
    const std::shared_ptr<Mgtt::Rendering::Node>& node,
    const std::map<std::string, Mgtt::Rendering::Texture>& textureMap) {
  auto patchTex = [&](Mgtt::Rendering::Texture& tex) {
    if (tex.id == 0 && !tex.path.empty()) {
      // path is "folder/uri" — find the matching map entry by path suffix
      for (const auto& [uri, mapTex] : textureMap) {
        if (tex.path == mapTex.path) {
          tex.id = mapTex.id;
          break;
        }
      }
    }
  };

  if (node->mesh != nullptr) {
    for (auto& prim : node->mesh->meshPrimitives) {
      auto& mat = prim.pbrMaterial;
      patchTex(mat.baseColorTexture);
      patchTex(mat.metallicRoughnessTexture);
      patchTex(mat.normalTexture);
      patchTex(mat.emissiveTexture);
      patchTex(mat.occlusionTexture);
    }
  }

  for (const auto& child : node->children) {
    PatchMaterialIds(child, textureMap);
  }
}

}  // namespace Mgtt::Rendering
