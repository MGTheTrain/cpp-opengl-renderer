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

#include <aabb.h>
#include <node.h>
#include <opengl-shader.h>
#include <texture.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace Mgtt::Rendering {

struct Node;
struct Mesh;
struct MeshPrimitive;
struct PbrMaterial;

/**
 * @brief Represents a 3D scene.
 */
struct Scene {
  Scene() = default;
  ~Scene() noexcept;

  Scene(const Scene&) = delete;
  Scene& operator=(const Scene&) = delete;
  Scene(Scene&&) noexcept;
  Scene& operator=(Scene&&) noexcept;

  /**
   * @brief Release all RAM and VRAM resources held by the scene.
   */
  void Clear();

  std::string name;
  std::string path;
  glm::vec3 pos{0.0f};
  glm::vec3 rot{0.0f};
  double scale{1.0};
  glm::mat4 mvp{1.0f};
  glm::mat4 matrix{1.0f};

  // Prevents loading the same texture into RAM more than once
  std::map<std::string, Mgtt::Rendering::Texture> textureMap;

  std::vector<std::shared_ptr<Mgtt::Rendering::Node>> nodes;
  std::vector<std::shared_ptr<Mgtt::Rendering::Node>> linearNodes;
  std::vector<Mgtt::Rendering::PbrMaterial> materials;

  Mgtt::Rendering::AABB aabb;
  Mgtt::Rendering::OpenGlShader shader;

 private:
  /**
   * @brief Recursively flatten the scene hierarchy into linearNodes.
   *
   * @param node Root of the subtree to linearize.
   */
  void Linearize(const std::shared_ptr<Mgtt::Rendering::Node>& node);
};

}  // namespace Mgtt::Rendering
