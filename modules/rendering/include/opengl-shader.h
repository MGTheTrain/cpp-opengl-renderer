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
#include <ishader.h>

#include <fstream>
#include <glm/glm.hpp>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

namespace Mgtt::Rendering {

/**
 * @brief OpenGL implementation of IShader.
 *
 * @note Essential parts from:
 * https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/shader_m.h
 */
class OpenGlShader : public IShader {
 public:
  OpenGlShader() noexcept;
  explicit OpenGlShader(
      const std::pair<std::string_view, std::string_view>& shaderPaths);
  ~OpenGlShader() override { Clear(); }

  OpenGlShader(const OpenGlShader&) = delete;
  OpenGlShader& operator=(const OpenGlShader&) = delete;

  OpenGlShader(OpenGlShader&& other) noexcept
      : id_(std::exchange(other.id_, 0)) {}
  OpenGlShader& operator=(OpenGlShader&& other) noexcept {
    if (this != &other) {
      Clear();
      id_ = std::exchange(other.id_, 0);
    }
    return *this;
  }

  /**
   * @brief Compile and link vertex + fragment shaders into a program.
   *
   * @param shaderPaths Pair of (vertex path, fragment path).
   * @return Ok on success, Err with a descriptive message on failure.
   */
  [[nodiscard]] Mgtt::Common::Result<void> Compile(
      const std::pair<std::string_view, std::string_view>& shaderPaths)
      override;

  /**
   * @brief Delete the GL program and reset id to 0.
   */
  void Clear() override;

  /**
   * @brief Return the GL program id.
   */
  [[nodiscard]] uint32_t GetProgramId() const noexcept { return id_; }

  /**
   * @brief Bind this shader program for subsequent draw calls.
   */
  void Use() const noexcept;

  void SetBool(std::string_view name, bool value) const;
  void SetInt(std::string_view name, int32_t value) const;
  void SetFloat(std::string_view name, float value) const;
  void SetVec2(std::string_view name, const glm::vec2& v) const;
  void SetVec2(std::string_view name, float x, float y) const;
  void SetVec3(std::string_view name, const glm::vec3& v) const;
  void SetVec3(std::string_view name, float x, float y, float z) const;
  void SetVec4(std::string_view name, const glm::vec4& v) const;
  void SetVec4(std::string_view name, float x, float y, float z, float w) const;
  void SetMat2(std::string_view name, const glm::mat2& m) const;
  void SetMat3(std::string_view name, const glm::mat3& m) const;
  void SetMat4(std::string_view name, const glm::mat4& m) const;

 private:
  /**
   * @brief Query the GL info log and return an error string on failure.
   *
   * @param object  Shader or program object id.
   * @param isProgram  True when checking a linked program, false for a shader.
   * @return Ok when compilation/linking succeeded, Err with the info log.
   */
  [[nodiscard]] static Mgtt::Common::Result<void> CheckCompileErrors(
      GLuint object, bool isProgram);

  uint32_t id_{0};
};

}  // namespace Mgtt::Rendering
