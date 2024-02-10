#include <opengl-shader.h>


/**
 * @brief Constructor for the OpenGlShader class.
 *
 * @param vsPath Path to the vertex shader source file.
 * @param fsPath Path to the fragment shader source file.
 */
Mgtt::Rendering::OpenGlShader::OpenGlShader(const std::string& vsPath, const std::string& fsPath) {
    this->Compile(vsPath, fsPath);
}

/**
 * @brief Destructor for the OpenGL shader.
 */
Mgtt::Rendering::OpenGlShader::~OpenGlShader() {
    this->Clear();
}

/**
 * @brief Compile the OpenGL shader program from specified vertex and fragment shader files.
 *
 * This method overrides the corresponding method in the IShader interface.
 * It compiles the vertex and fragment shaders, linking them into a shader program.
 *
 * @param vsPath The file path to the vertex shader source code.
 * @param fsPath The file path to the fragment shader source code.
 */
void Mgtt::Rendering::OpenGlShader::Compile(const std::string& vsPath, const std::string& fsPath) {
    this->Clear();

    if (vsPath.size() == 0) {
        throw std::runtime_error("OPENGL SHADER ALLOCATOR ERROR: Empty vertex path: " + vsPath);
    }
    if (fsPath.size() == 0) {
        throw std::runtime_error("OPENGL SHADER ALLOCATOR ERROR: Empty fragment path: " + fsPath);
    }
    std::string vsCode;
    std::string fsCode;
    std::ifstream vsFile;
    std::ifstream fsFile;
    vsFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fsFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try  {
        vsFile.open(vsPath.c_str());
        fsFile.open(fsPath.c_str());
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vsFile.rdbuf();
        fShaderStream << fsFile.rdbuf();		
        vsFile.close();
        fsFile.close();
        vsCode = vShaderStream.str();
        fsCode = fShaderStream.str();			
    }
    catch (std::ifstream::failure& ex)
    {
        std::string errorMsg = "SHADER ERROR: " + std::string(ex.what());
        throw std::runtime_error(errorMsg.c_str());
        return;
    }
    const char* vShaderCode = vsCode.c_str();
    const char * fShaderCode = fsCode.c_str();
    unsigned int vs, fragment;
    vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vShaderCode, NULL);
    glCompileShader(vs);
    this->CheckCompileErrors(vs, "VERTEX");
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    this->CheckCompileErrors(fragment, "FRAGMENT");
    this->id = glCreateProgram();
    glAttachShader(this->id, vs);
    glAttachShader(this->id, fragment);
    glLinkProgram(this->id);
    this->CheckCompileErrors(this->id, "PROGRAM");
    glDeleteShader(vs);
    glDeleteShader(fragment);
}

/**
 * @brief Delete the shader program.
 */
void Mgtt::Rendering::OpenGlShader::Clear() {
    if (this->id > 0) {
        glDeleteProgram(this->id);
        this->id = 0;
    }
}

/**
 * @brief Get the ID of the shader program.
 *
 * @return The ID of the shader program.
 */
unsigned int& Mgtt::Rendering::OpenGlShader::GetProgramId(){
    return this->id;
}

/**
 * @brief Activate the shader program.
 */
void Mgtt::Rendering::OpenGlShader::Use() const {
    glUseProgram(this->id); 
}

/**
 * @brief Set a boolean uniform value in the shader program.
 *
 * @param name The name of the boolean uniform.
 * @param value The boolean value to set.
 */
void Mgtt::Rendering::OpenGlShader::SetBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(this->id, name.c_str()), (int)value); 
}

/**
 * @brief Set an integer uniform value in the shader program.
 *
 * @param name The name of the integer uniform.
 * @param value The integer value to set.
 */
void Mgtt::Rendering::OpenGlShader::SetInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(this->id, name.c_str()), value); 
}

/**
 * @brief Set a floating-point uniform value in the shader program.
 *
 * @param name The name of the floating-point uniform.
 * @param value The floating-point value to set.
 */
void Mgtt::Rendering::OpenGlShader::SetFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(this->id, name.c_str()), value); 
}

/**
 * @brief Set a 2D vector uniform value in the shader program.
 *
 * @param name The name of the 2D vector uniform.
 * @param value The glm::vec2 value to set.
 */
void Mgtt::Rendering::OpenGlShader::SetVec2(const std::string& name, const glm::vec2& value) const {
    glUniform2fv(glGetUniformLocation(this->id, name.c_str()), 1, &value[0]); 
}

/**
 * @brief Set a 2D vector uniform value in the shader program.
 *
 * @param name The name of the 2D vector uniform.
 * @param x The x-component of the vector.
 * @param y The y-component of the vector.
 */
void Mgtt::Rendering::OpenGlShader::SetVec2(const std::string& name, float x, float y) const {
    glUniform2f(glGetUniformLocation(this->id, name.c_str()), x, y); 
}

/**
 * @brief Set a 3D vector uniform value in the shader program.
 *
 * @param name The name of the 3D vector uniform.
 * @param value The glm::vec3 value to set.
 */
void Mgtt::Rendering::OpenGlShader::SetVec3(const std::string& name, const glm::vec3& value) const {
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
void Mgtt::Rendering::OpenGlShader::SetVec3(const std::string& name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(this->id, name.c_str()), x, y, z); 
}

/**
 * @brief Set a 4D vector uniform value in the shader program.
 *
 * @param name The name of the 4D vector uniform.
 * @param value The glm::vec4 value to set.
 */
void Mgtt::Rendering::OpenGlShader::SetVec4(const std::string& name, const glm::vec4& value) const {
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
void Mgtt::Rendering::OpenGlShader::SetVec4(const std::string& name, float x, float y, float z, float w) const {
    glUniform4f(glGetUniformLocation(this->id, name.c_str()), x, y, z, w); 
}

/**
 * @brief Set a 2x2 matrix uniform value in the shader program.
 *
 * @param name The name of the 2x2 matrix uniform.
 * @param mat The glm::mat2 value to set.
 */
void Mgtt::Rendering::OpenGlShader::SetMat2(const std::string& name, const glm::mat2& mat) const {
    glUniformMatrix2fv(glGetUniformLocation(this->id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

/**
 * @brief Set a 3x3 matrix uniform value in the shader program.
 *
 * @param name The name of the 3x3 matrix uniform.
 * @param mat The glm::mat3 value to set.
 */
void Mgtt::Rendering::OpenGlShader::SetMat3(const std::string& name, const glm::mat3& mat) const {
    glUniformMatrix3fv(glGetUniformLocation(this->id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

/**
 * @brief Set a 4x4 matrix uniform value in the shader program.
 *
 * @param name The name of the 4x4 matrix uniform.
 * @param mat The glm::mat4 value to set.
 */
void Mgtt::Rendering::OpenGlShader::SetMat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(this->id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

/**
 * @brief Check for compile and linking errors in the shader.
 *
 * @param shader Shader object (vertex or fragment).
 * @param type Type of shader (vertex or fragment).
 */
void Mgtt::Rendering::OpenGlShader::CheckCompileErrors(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            throw std::runtime_error("SHADER COMPILATION ERROR: Type: " + type + "\n" + infoLog + "\n#####################################################################################");
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            throw std::runtime_error("PROGRAM LINKING ERROR: Type: " + type + "\n" + infoLog + "\n#####################################################################################");
        }
    }
}