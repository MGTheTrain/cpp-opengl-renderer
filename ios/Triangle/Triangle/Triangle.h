//
//  Triangle.hpp
//  Triangle
//
//  Created by Marvin Gajek on 23.03.24.
//

#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdio.h>
#include <OpenGLES/ES3/gl.h>
#include <string.h>

class Triangle {
public:
    Triangle();
    ~Triangle();

    void initOpenGL();
    void drawOpenGL();
    void clearOpenGL();
private:
    GLuint VBO;
    GLuint VAO;
    GLuint program;
    
    void compileShader(GLuint shader, const GLchar* source);
    void linkProgram(GLuint program, GLuint vertexShader, GLuint fragmentShader);
};

#endif
