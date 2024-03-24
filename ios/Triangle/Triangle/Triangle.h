//
//  Triangle.hpp
//  Triangle
//
//  Created by Marvin Gajek on 23.03.24.
//

#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdio.h>
#include <string.h>

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#if defined(TARGET_OS_IOS) || defined(TARGET_IPHONE_SIMULATOR)
#include <OpenGLES/ES3/gl.h>
#endif
/**
 * @brief Class representing a triangle in OpenGL.
 */
class Triangle {
public:
    /**
     * @brief Constructor for the Triangle class.
     */
    Triangle();
    
    /**
     * @brief Destructor for the Triangle class.
     */
    ~Triangle();

    /**
     * @brief Initializes OpenGL resources required to render the triangle.
     */
    void init();
    
    /**
     * @brief Draws the triangle using OpenGL.
     */
    void draw();
    
    /**
     * @brief Clears OpenGL resources when they are no longer needed.
     */
    void clear();

private:
    GLuint VBO;     /**< Vertex Buffer Object: Stores vertex data. */
    GLuint VAO;     /**< Vertex Array Object: Describes vertex data format. */
    GLuint program; /**< Shader Program: Defines how vertices are transformed and colored. */
    
    /**
     * @brief Compiles a shader from its source code.
     * @param shader The shader object to compile.
     * @param source The source code of the shader.
     */
    void compileShader(GLuint shader, const GLchar* source);
    
    /**
     * @brief Links vertex and fragment shaders into a shader program.
     * @param program The shader program object.
     * @param vertexShader The vertex shader object.
     * @param fragmentShader The fragment shader object.
     */
    void linkProgram(GLuint program, GLuint vertexShader, GLuint fragmentShader);
};

#endif
