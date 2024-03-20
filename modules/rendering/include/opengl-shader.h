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
#if defined(__EMSCRIPTEN__ ) || defined(__ANDROID__) 
#include <GLES3/gl3.h>
#else
#include <GL/glew.h>
#endif>
#include <ishader.h>

#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace Mgtt::Rendering {
/**
 * @brief Implementation of the IShader interface for OpenGL shaders.
 *
 * This class provides concrete implementation details for compiling and
 * managing vertex and fragment shaders within an OpenGL rendering context.
 * @note Essential parts from:
 * https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/shader_m.h
 */
class OpenGlShader : public IShader {
 public:
  /**
   * @brief Default Constructor for the OpenGlShader class.
   *
   */
  OpenGlShader();

  /**
   * @brief Constructor for the OpenGlShader class.
   *
   * @param shaderPathes The vertex and fragment shader pathes
   */
  explicit OpenGlShader(const std::pair<std::string, std::string> shaderPathes);

  /**
   * @brief Compile the shader program from specified vertex and fragment shader
   * files.
   *
   * This method compiles the vertex and fragment shaders, linking them into a
   * shader program.
   *
   * @param shaderPathes The vertex and fragment shader pathes
   */
  void Compile(const std::pair<std::string, std::string> shaderPathes) override;

  /**
   * @brief Delete the shader program.
   */
  void Clear() override;

  /**
   * @brief Get the ID of the shader program.
   *
   * @return The ID of the shader program.
   */
  unsigned int& GetProgramId();

  /**
   * @brief Activate the shader program.
   */
  void Use() const;

  /**
   * @brief Set a boolean uniform value in the shader program.
   *
   * @param name The name of the boolean uniform.
   * @param value The boolean value to set.
   */
  void SetBool(const std::string& name, bool value) const;

  /**
   * @brief Set an integer uniform value in the shader program.
   *
   * @param name The name of the integer uniform.
   * @param value The integer value to set.
   */
  void SetInt(const std::string& name, int value) const;

  /**
   * @brief Set a floating-point uniform value in the shader program.
   *
   * @param name The name of the floating-point uniform.
   * @param value The floating-point value to set.
   */
  void SetFloat(const std::string& name, float value) const;

  /**
   * @brief Set a 2D vector uniform value in the shader program.
   *
   * @param name The name of the 2D vector uniform.
   * @param value The glm::vec2 value to set.
   */
  void SetVec2(const std::string& name, const glm::vec2& value) const;

  /**
   * @brief Set a 2D vector uniform value in the shader program.
   *
   * @param name The name of the 2D vector uniform.
   * @param x The x-component of the vector.
   * @param y The y-component of the vector.
   */
  void SetVec2(const std::string& name, float x, float y) const;

  /**
   * @brief Set a 3D vector uniform value in the shader program.
   *
   * @param name The name of the 3D vector uniform.
   * @param value The glm::vec3 value to set.
   */
  void SetVec3(const std::string& name, const glm::vec3& value) const;

  /**
   * @brief Set a 3D vector uniform value in the shader program.
   *
   * @param name The name of the 3D vector uniform.
   * @param x The x-component of the vector.
   * @param y The y-component of the vector.
   * @param z The z-component of the vector.
   */
  void SetVec3(const std::string& name, float x, float y, float z) const;

  /**
   * @brief Set a 4D vector uniform value in the shader program.
   *
   * @param name The name of the 4D vector uniform.
   * @param value The glm::vec4 value to set.
   */
  void SetVec4(const std::string& name, const glm::vec4& value) const;

  /**
   * @brief Set a 4D vector uniform value in the shader program.
   *
   * @param name The name of the 4D vector uniform.
   * @param x The x-component of the vector.
   * @param y The y-component of the vector.
   * @param z The z-component of the vector.
   * @param w The w-component of the vector.
   */
  void SetVec4(const std::string& name, float x, float y, float z,
               float w) const;

  /**
   * @brief Set a 2x2 matrix uniform value in the shader program.
   *
   * @param name The name of the 2x2 matrix uniform.
   * @param mat The glm::mat2 value to set.
   */
  void SetMat2(const std::string& name, const glm::mat2& mat) const;

  /**
   * @brief Set a 3x3 matrix uniform value in the shader program.
   *
   * @param name The name of the 3x3 matrix uniform.
   * @param mat The glm::mat3 value to set.
   */
  void SetMat3(const std::string& name, const glm::mat3& mat) const;

  /**
   * @brief Set a 4x4 matrix uniform value in the shader program.
   *
   * @param name The name of the 4x4 matrix uniform.
   * @param mat The glm::mat4 value to set.
   */
  void SetMat4(const std::string& name, const glm::mat4& mat) const;

 private:
  /**
   * @brief Check for compile and linking errors in the shader.
   *
   * @param shader Shader object (vertex or fragment).
   * @param type Type of shader (vertex or fragment).
   */
  void CheckCompileErrors(GLuint shader, std::string type);

  unsigned int id;  // the OpenGL program id
};
}  // namespace Mgtt::Rendering
