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

namespace Mgtt::Rendering {

OpenGlShader::OpenGlShader(
    const std::pair<std::string_view, std::string_view>& shaderPaths) {
  if (auto result = Compile(shaderPaths); result.err()) {
    throw std::runtime_error("Failed to compile shader: " + result.error());
  }
}

OpenGlShader::~OpenGlShader() noexcept { Clear(); }

OpenGlShader::OpenGlShader(OpenGlShader&& other) noexcept
    : id_(std::exchange(other.id_, 0)) {}

OpenGlShader& OpenGlShader::operator=(OpenGlShader&& other) noexcept {
  if (this != &other) {
    Clear();
    id_ = std::exchange(other.id_, 0);
  }
  return *this;
}

uint32_t OpenGlShader::GetProgramId() const noexcept { return id_; }

Mgtt::Common::Result<void> OpenGlShader::Compile(
    const std::pair<std::string_view, std::string_view>& shaderPaths) {
  Clear();

  if (shaderPaths.first.empty()) {
    return Mgtt::Common::Result<void>::Err("Empty vertex shader path");
  }
  if (shaderPaths.second.empty()) {
    return Mgtt::Common::Result<void>::Err("Empty fragment shader path");
  }

  // read source files
  auto readFile =
      [](std::string_view path) -> Mgtt::Common::Result<std::string> {
    std::ifstream file{std::string(path)};
    if (!file.is_open()) {
      return Mgtt::Common::Result<std::string>::Err(
          "Could not open shader file: " + std::string(path));
    }
    std::ostringstream stream;
    stream << file.rdbuf();
    return Mgtt::Common::Result<std::string>::Ok(stream.str());
  };

  auto vsResult = readFile(shaderPaths.first);
  if (vsResult.err()) {
    return Mgtt::Common::Result<void>::Err(vsResult.error());
  }

  auto fsResult = readFile(shaderPaths.second);
  if (fsResult.err()) {
    return Mgtt::Common::Result<void>::Err(fsResult.error());
  }

  const std::string& vsCode = vsResult.value();
  const std::string& fsCode = fsResult.value();
  const char* vsSrc = vsCode.c_str();
  const char* fsSrc = fsCode.c_str();

  // compile vertex shader
  GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vsSrc, nullptr);
  glCompileShader(vertex);
  if (auto result = CheckCompileErrors(vertex, false); result.err()) {
    glDeleteShader(vertex);
    return result;
  }

  // compile fragment shader
  GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fsSrc, nullptr);
  glCompileShader(fragment);
  if (auto result = CheckCompileErrors(fragment, false); result.err()) {
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return result;
  }

  // link program
  id_ = glCreateProgram();
  glAttachShader(id_, vertex);
  glAttachShader(id_, fragment);
  glLinkProgram(id_);
  glDeleteShader(vertex);
  glDeleteShader(fragment);

  if (auto result = CheckCompileErrors(id_, true); result.err()) {
    Clear();
    return result;
  }

  std::cout << "Shader program compiled from " << shaderPaths.first << " and "
            << shaderPaths.second << '\n';
  return Mgtt::Common::Result<void>::Ok();
}

void OpenGlShader::Clear() noexcept {
  if (id_ > 0) {
    glDeleteProgram(id_);
    std::cout << "Deleted shader program " << id_ << '\n';
    id_ = 0;
  }
}

[[nodiscard]] Mgtt::Common::Result<void> OpenGlShader::Use() const noexcept {
  if (id_ == 0) {
    return Mgtt::Common::Result<void>::Err("Shader program not compiled");
  }
  glUseProgram(id_);
  return Mgtt::Common::Result<void>::Ok();
}

void OpenGlShader::SetBool(std::string_view name, bool value) const {
  glUniform1i(glGetUniformLocation(id_, std::string{name}.c_str()),
              static_cast<int>(value));
}
void OpenGlShader::SetInt(std::string_view name, int32_t value) const {
  glUniform1i(glGetUniformLocation(id_, std::string{name}.c_str()), value);
}
void OpenGlShader::SetFloat(std::string_view name, float value) const {
  glUniform1f(glGetUniformLocation(id_, std::string{name}.c_str()), value);
}
void OpenGlShader::SetVec2(std::string_view name, const glm::vec2& vec) const {
  glUniform2fv(glGetUniformLocation(id_, std::string{name}.c_str()), 1,
               &vec[0]);
}
void OpenGlShader::SetVec2(std::string_view name, float xVal,
                           float yVal) const {
  glUniform2f(glGetUniformLocation(id_, std::string{name}.c_str()), xVal, yVal);
}
void OpenGlShader::SetVec3(std::string_view name, const glm::vec3& vec) const {
  glUniform3fv(glGetUniformLocation(id_, std::string{name}.c_str()), 1,
               &vec[0]);
}
void OpenGlShader::SetVec3(std::string_view name, float xVal, float yVal,
                           float zVal) const {
  glUniform3f(glGetUniformLocation(id_, std::string{name}.c_str()), xVal, yVal,
              zVal);
}
void OpenGlShader::SetVec4(std::string_view name, const glm::vec4& vec) const {
  glUniform4fv(glGetUniformLocation(id_, std::string{name}.c_str()), 1,
               &vec[0]);
}
void OpenGlShader::SetVec4(std::string_view name, float xVal, float yVal,
                           float zVal, float wVal) const {
  glUniform4f(glGetUniformLocation(id_, std::string{name}.c_str()), xVal, yVal,
              zVal, wVal);
}
void OpenGlShader::SetMat2(std::string_view name, const glm::mat2& mat) const {
  glUniformMatrix2fv(glGetUniformLocation(id_, std::string{name}.c_str()), 1,
                     GL_FALSE, &mat[0][0]);
}
void OpenGlShader::SetMat3(std::string_view name, const glm::mat3& mat) const {
  glUniformMatrix3fv(glGetUniformLocation(id_, std::string{name}.c_str()), 1,
                     GL_FALSE, &mat[0][0]);
}
void OpenGlShader::SetMat4(std::string_view name, const glm::mat4& mat) const {
  glUniformMatrix4fv(glGetUniformLocation(id_, std::string{name}.c_str()), 1,
                     GL_FALSE, &mat[0][0]);
}

Mgtt::Common::Result<void> OpenGlShader::CheckCompileErrors(GLuint object,
                                                            bool isProgram) {
  GLint success = 0;
  GLchar log[1024];
  if (isProgram) {
    glGetProgramiv(object, GL_LINK_STATUS, &success);
    if (success == 0) {
      glGetProgramInfoLog(object, sizeof(log), nullptr, log);
      return Mgtt::Common::Result<void>::Err(
          std::string("Program linking error:\n") + log);
    }
  } else {
    glGetShaderiv(object, GL_COMPILE_STATUS, &success);
    if (success == 0) {
      glGetShaderInfoLog(object, sizeof(log), nullptr, log);
      return Mgtt::Common::Result<void>::Err(
          std::string("Shader compilation error:\n") + log);
    }
  }
  return Mgtt::Common::Result<void>::Ok();
}

}  // namespace Mgtt::Rendering
