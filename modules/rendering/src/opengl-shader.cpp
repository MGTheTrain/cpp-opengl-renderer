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

#include <opengl-shader.h>

#include <iostream>
#include <string>

namespace Mgtt::Rendering {

OpenGlShader::OpenGlShader() noexcept : id_(0) {}

OpenGlShader::OpenGlShader(
    const std::pair<std::string_view, std::string_view>& shaderPaths)
    : id_(0) {
  if (auto r = Compile(shaderPaths); r.err()) {
    std::cerr << r.error() << '\n';
  }
}

Mgtt::Common::Result<void> OpenGlShader::Compile(
    const std::pair<std::string_view, std::string_view>& shaderPaths) {
  Clear();

  if (shaderPaths.first.empty())
    return Mgtt::Common::Result<void>::Err("Empty vertex shader path");
  if (shaderPaths.second.empty())
    return Mgtt::Common::Result<void>::Err("Empty fragment shader path");

  // --- read source files ---
  auto readFile =
      [](std::string_view path) -> Mgtt::Common::Result<std::string> {
    std::ifstream file{std::string(path)};  // braces avoid the vexing parse
    if (!file.is_open())
      return Mgtt::Common::Result<std::string>::Err(
          "Could not open shader file: " + std::string(path));
    std::ostringstream ss;
    ss << file.rdbuf();
    return Mgtt::Common::Result<std::string>::Ok(ss.str());
  };

  auto vsResult = readFile(shaderPaths.first);
  if (vsResult.err()) return Mgtt::Common::Result<void>::Err(vsResult.error());

  auto fsResult = readFile(shaderPaths.second);
  if (fsResult.err()) return Mgtt::Common::Result<void>::Err(fsResult.error());

  const std::string& vsCode = vsResult.value();
  const std::string& fsCode = fsResult.value();
  const char* vsSrc = vsCode.c_str();
  const char* fsSrc = fsCode.c_str();

  // --- compile vertex shader ---
  GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vsSrc, nullptr);
  glCompileShader(vertex);
  if (auto r = CheckCompileErrors(vertex, false); r.err()) {
    glDeleteShader(vertex);
    return r;
  }

  // --- compile fragment shader ---
  GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fsSrc, nullptr);
  glCompileShader(fragment);
  if (auto r = CheckCompileErrors(fragment, false); r.err()) {
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return r;
  }

  // --- link program ---
  id_ = glCreateProgram();
  glAttachShader(id_, vertex);
  glAttachShader(id_, fragment);
  glLinkProgram(id_);
  glDeleteShader(vertex);
  glDeleteShader(fragment);

  if (auto r = CheckCompileErrors(id_, true); r.err()) {
    Clear();
    return r;
  }

  std::cout << "Shader program compiled from " << shaderPaths.first << " and "
            << shaderPaths.second << '\n';
  return Mgtt::Common::Result<void>::Ok();
}

void OpenGlShader::Clear() {
  if (id_ > 0) {
    glDeleteProgram(id_);
    std::cout << "Deleted shader program " << id_ << '\n';
    id_ = 0;
  }
}

void OpenGlShader::Use() const noexcept { glUseProgram(id_); }

void OpenGlShader::SetBool(std::string_view name, bool value) const {
  glUniform1i(glGetUniformLocation(id_, name.data()), static_cast<int>(value));
}
void OpenGlShader::SetInt(std::string_view name, int32_t value) const {
  glUniform1i(glGetUniformLocation(id_, name.data()), value);
}
void OpenGlShader::SetFloat(std::string_view name, float value) const {
  glUniform1f(glGetUniformLocation(id_, name.data()), value);
}
void OpenGlShader::SetVec2(std::string_view name, const glm::vec2& v) const {
  glUniform2fv(glGetUniformLocation(id_, name.data()), 1, &v[0]);
}
void OpenGlShader::SetVec2(std::string_view name, float x, float y) const {
  glUniform2f(glGetUniformLocation(id_, name.data()), x, y);
}
void OpenGlShader::SetVec3(std::string_view name, const glm::vec3& v) const {
  glUniform3fv(glGetUniformLocation(id_, name.data()), 1, &v[0]);
}
void OpenGlShader::SetVec3(std::string_view name, float x, float y,
                           float z) const {
  glUniform3f(glGetUniformLocation(id_, name.data()), x, y, z);
}
void OpenGlShader::SetVec4(std::string_view name, const glm::vec4& v) const {
  glUniform4fv(glGetUniformLocation(id_, name.data()), 1, &v[0]);
}
void OpenGlShader::SetVec4(std::string_view name, float x, float y, float z,
                           float w) const {
  glUniform4f(glGetUniformLocation(id_, name.data()), x, y, z, w);
}
void OpenGlShader::SetMat2(std::string_view name, const glm::mat2& m) const {
  glUniformMatrix2fv(glGetUniformLocation(id_, name.data()), 1, GL_FALSE,
                     &m[0][0]);
}
void OpenGlShader::SetMat3(std::string_view name, const glm::mat3& m) const {
  glUniformMatrix3fv(glGetUniformLocation(id_, name.data()), 1, GL_FALSE,
                     &m[0][0]);
}
void OpenGlShader::SetMat4(std::string_view name, const glm::mat4& m) const {
  glUniformMatrix4fv(glGetUniformLocation(id_, name.data()), 1, GL_FALSE,
                     &m[0][0]);
}

Mgtt::Common::Result<void> OpenGlShader::CheckCompileErrors(GLuint object,
                                                            bool isProgram) {
  GLint success = 0;
  GLchar log[1024];
  if (isProgram) {
    glGetProgramiv(object, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(object, sizeof(log), nullptr, log);
      return Mgtt::Common::Result<void>::Err(
          std::string("Program linking error:\n") + log);
    }
  } else {
    glGetShaderiv(object, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(object, sizeof(log), nullptr, log);
      return Mgtt::Common::Result<void>::Err(
          std::string("Shader compilation error:\n") + log);
    }
  }
  return Mgtt::Common::Result<void>::Ok();
}

}  // namespace Mgtt::Rendering
