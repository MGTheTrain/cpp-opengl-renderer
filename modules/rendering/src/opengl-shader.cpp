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

#include <opengl-shader.h>

/**
 * @brief Default Constructor for the OpenGlShader class.
 *
 */
Mgtt::Rendering::OpenGlShader::OpenGlShader() { this->id = 0; }

/**
 * @brief Constructor for the OpenGlShader class.
 *
 * @param shaderPaths The vertex and fragment shader pathes
 */
Mgtt::Rendering::OpenGlShader::OpenGlShader(
    const std::pair<std::string, std::string> shaderPaths) {
  this->id = 0;
  this->Compile(shaderPaths);
}

/**
 * @brief Compile the shader program from specified vertex and fragment shader
 * files.
 *
 * This method compiles the vertex and fragment shaders, linking them into a
 * shader program.
 *
 * @param shaderPaths The vertex and fragment shader pathes
 */
void Mgtt::Rendering::OpenGlShader::Compile(
    const std::pair<std::string, std::string>& shaderPaths) {
  this->Clear();

  if (shaderPaths.first.size() == 0) {
    throw std::runtime_error("Empty vertex path: " + shaderPaths.first);
  }
  if (shaderPaths.second.size() == 0) {
    throw std::runtime_error("Empty fragment path: " + shaderPaths.second);
  }
  std::string vsCode;
  std::string fsCode;
  std::ifstream vsFile;
  std::ifstream fsFile;
  vsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    vsFile.open(shaderPaths.first.c_str());
    fsFile.open(shaderPaths.second.c_str());
    std::stringstream vShaderStream, fShaderStream;
    vShaderStream << vsFile.rdbuf();
    fShaderStream << fsFile.rdbuf();
    vsFile.close();
    fsFile.close();
    vsCode = vShaderStream.str();
    fsCode = fShaderStream.str();
  } catch (std::ifstream::failure& ex) {
    if (vsFile.is_open()) {
      vsFile.close();
    }
    if (fsFile.is_open()) {
      fsFile.close();
    }
    std::string errorMsg = "Either vertex shader file " + shaderPaths.first +
                           " or fragment shader file " + shaderPaths.second +
                           "missing";
    std::cerr << errorMsg.c_str() << std::endl;
    return;
  }
  const char* vShaderCode = vsCode.c_str();
  const char* fShaderCode = fsCode.c_str();
  uint32_t vertex, fragment;
  try {
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    this->CheckCompileErrors(vertex, "VERTEX");
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    this->CheckCompileErrors(fragment, "FRAGMENT");
    this->id = glCreateProgram();
    glAttachShader(this->id, vertex);
    glAttachShader(this->id, fragment);
    glLinkProgram(this->id);
    this->CheckCompileErrors(this->id, "PROGRAM");
    glDeleteShader(vertex);
    glDeleteShader(fragment);
  } catch (const std::runtime_error& ex) {
    if (vertex > 0) {
      glDeleteShader(vertex);
      vertex = 0;
    }
    if (fragment > 0) {
      glDeleteShader(fragment);
      fragment = 0;
    }
    this->Clear();
    std::cerr << ex.what() << std::endl;
  }
  std::cout << "Shader program allocated with vertex shader and fragment "
               "shader compiled from "
            << shaderPaths.first << " and " << shaderPaths.second << std::endl;
}

/**
 * @brief Delete the shader program.
 */
void Mgtt::Rendering::OpenGlShader::Clear() {
  if (this->id > 0) {
    glDeleteProgram(this->id);
    std::cout << "Deleted program with id " << this->id << std::endl;
    this->id = 0;
  }
}

/**
 * @brief Get the ID of the shader program.
 *
 * @return The ID of the shader program.
 */
uint32_t& Mgtt::Rendering::OpenGlShader::GetProgramId() { return this->id; }

/**
 * @brief Activate the shader program.
 */
void Mgtt::Rendering::OpenGlShader::Use() const { glUseProgram(this->id); }

/**
 * @brief Set a boolean uniform value in the shader program.
 *
 * @param name The name of the boolean uniform.
 * @param value The boolean value to set.
 */
void Mgtt::Rendering::OpenGlShader::SetBool(const std::string& name,
                                            bool value) const {
  glUniform1i(glGetUniformLocation(this->id, name.c_str()),
              static_cast<int>(value));
}

/**
 * @brief Set an integer uniform value in the shader program.
 *
 * @param name The name of the integer uniform.
 * @param value The integer value to set.
 */
void Mgtt::Rendering::OpenGlShader::SetInt(const std::string& name,
                                           int32_t value) const {
  glUniform1i(glGetUniformLocation(this->id, name.c_str()), value);
}

/**
 * @brief Set a floating-point uniform value in the shader program.
 *
 * @param name The name of the floating-point uniform.
 * @param value The floating-point value to set.
 */
void Mgtt::Rendering::OpenGlShader::SetFloat(const std::string& name,
                                             float value) const {
  glUniform1f(glGetUniformLocation(this->id, name.c_str()), value);
}

/**
 * @brief Set a 2D vector uniform value in the shader program.
 *
 * @param name The name of the 2D vector uniform.
 * @param value The glm::vec2 value to set.
 */
void Mgtt::Rendering::OpenGlShader::SetVec2(const std::string& name,
                                            const glm::vec2& value) const {
  glUniform2fv(glGetUniformLocation(this->id, name.c_str()), 1, &value[0]);
}

/**
 * @brief Set a 2D vector uniform value in the shader program.
 *
 * @param name The name of the 2D vector uniform.
 * @param x The x-component of the vector.
 * @param y The y-component of the vector.
 */
void Mgtt::Rendering::OpenGlShader::SetVec2(const std::string& name, float x,
                                            float y) const {
  glUniform2f(glGetUniformLocation(this->id, name.c_str()), x, y);
}

/**
 * @brief Set a 3D vector uniform value in the shader program.
 *
 * @param name The name of the 3D vector uniform.
 * @param value The glm::vec3 value to set.
 */
void Mgtt::Rendering::OpenGlShader::SetVec3(const std::string& name,
                                            const glm::vec3& value) const {
  glUniform3fv(glGetUniformLocation(this->id, name.c_str()), 1, &value[0]);
}

/**
 * @brief Set a 3D vector uniform value in the shader program.
 *
 * @param name The name of the 3D vector uniform.
 * @param x The x-component of the vector.
 * @param y The y-component of the vector.
 * @param z The z-component of the vector.
 */
void Mgtt::Rendering::OpenGlShader::SetVec3(const std::string& name, float x,
                                            float y, float z) const {
  glUniform3f(glGetUniformLocation(this->id, name.c_str()), x, y, z);
}

/**
 * @brief Set a 4D vector uniform value in the shader program.
 *
 * @param name The name of the 4D vector uniform.
 * @param value The glm::vec4 value to set.
 */
void Mgtt::Rendering::OpenGlShader::SetVec4(const std::string& name,
                                            const glm::vec4& value) const {
  glUniform4fv(glGetUniformLocation(this->id, name.c_str()), 1, &value[0]);
}

/**
 * @brief Set a 4D vector uniform value in the shader program.
 *
 * @param name The name of the 4D vector uniform.
 * @param x The x-component of the vector.
 * @param y The y-component of the vector.
 * @param z The z-component of the vector.
 * @param w The w-component of the vector.
 */
void Mgtt::Rendering::OpenGlShader::SetVec4(const std::string& name, float x,
                                            float y, float z, float w) const {
  glUniform4f(glGetUniformLocation(this->id, name.c_str()), x, y, z, w);
}

/**
 * @brief Set a 2x2 matrix uniform value in the shader program.
 *
 * @param name The name of the 2x2 matrix uniform.
 * @param mat The glm::mat2 value to set.
 */
void Mgtt::Rendering::OpenGlShader::SetMat2(const std::string& name,
                                            const glm::mat2& mat) const {
  glUniformMatrix2fv(glGetUniformLocation(this->id, name.c_str()), 1, GL_FALSE,
                     &mat[0][0]);
}

/**
 * @brief Set a 3x3 matrix uniform value in the shader program.
 *
 * @param name The name of the 3x3 matrix uniform.
 * @param mat The glm::mat3 value to set.
 */
void Mgtt::Rendering::OpenGlShader::SetMat3(const std::string& name,
                                            const glm::mat3& mat) const {
  glUniformMatrix3fv(glGetUniformLocation(this->id, name.c_str()), 1, GL_FALSE,
                     &mat[0][0]);
}

/**
 * @brief Set a 4x4 matrix uniform value in the shader program.
 *
 * @param name The name of the 4x4 matrix uniform.
 * @param mat The glm::mat4 value to set.
 */
void Mgtt::Rendering::OpenGlShader::SetMat4(const std::string& name,
                                            const glm::mat4& mat) const {
  glUniformMatrix4fv(glGetUniformLocation(this->id, name.c_str()), 1, GL_FALSE,
                     &mat[0][0]);
}

/**
 * @brief Check for compile and linking errors in the shader.
 *
 * @param shader Shader object (vertex or fragment).
 * @param type Type of shader (vertex or fragment).
 */
void Mgtt::Rendering::OpenGlShader::CheckCompileErrors(GLuint shader,
                                                       std::string type) {
  GLint success;
  GLchar infoLog[1024];
  if (type != "PROGRAM") {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(shader, 1024, NULL, infoLog);
      std::string errorMsg = "Shader compilation error: Type: " + type + "\n" +
                             infoLog +
                             "\n###############################################"
                             "######################################";
      throw std::runtime_error(errorMsg);
    }
  } else {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shader, 1024, NULL, infoLog);
      std::string errorMsg = "Program linking error: Type: " + type + "\n" +
                             infoLog +
                             "\n###############################################"
                             "######################################";
      throw std::runtime_error(errorMsg);
    }
  }
}
