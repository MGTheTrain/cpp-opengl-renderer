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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <string>
#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include <aabb.h>
#include <node.h>
#include <opengl-shader.h>
#include <texture.h>

#include <glm/gtx/quaternion.hpp>
#include <map>

namespace Mgtt::Rendering {
struct Node;
struct Mesh;
struct MeshPrimitive;
struct PbrMaterial;
/**
 * @brief Represents a 3D scene.
 */
struct Scene {
  /**
   * @brief Constructor for the Scene struct.
   */
  Scene();

  ~Scene() {}

  /**
   * @brief Clear releases resources.
   */
  void Clear();
  std::string name;
  std::string path;
  glm::vec3 pos;
  glm::vec3 rot;
  double scale;
  glm::mat4 mvp;
  glm::mat4 matrix;
  std::map<std::string, Mgtt::Rendering::Texture>
      textureMap;  // case in which we want to prevent loading the same texture
                   // into RAM which is time consuming
  std::vector<std::shared_ptr<Mgtt::Rendering::Node>> nodes;
  std::vector<std::shared_ptr<Mgtt::Rendering::Node>> linearNodes;
  std::vector<Mgtt::Rendering::PbrMaterial> materials;
  Mgtt::Rendering::AABB aabb;
  Mgtt::Rendering::OpenGlShader shader;

 private:
  /**
   * @brief Recursively linearizes the scene hierarchy starting from the given
   * node.
   *
   * This method flattens the hierarchical struct of the scene, storing nodes in
   * a linearNodes vector.
   *
   * @param node The starting node to linearize.
   */
  void Linearize(std::shared_ptr<Mgtt::Rendering::Node> node);
};
}  // namespace Mgtt::Rendering
