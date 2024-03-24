//
//  triangle.cpp
//  Triangle
//
//  Created by Marvin Gajek on 23.03.24.
//

#include "Triangle.h"

GLuint VBO;
GLuint VAO;
GLuint program;

const GLchar* vertexShaderSource =
    "#version 300 es\n"
    "precision highp float;\n"
    "layout(location = 0) in vec3 position;\n"
    "void main() {\n"
    "    gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
    "}\n";

const GLchar* fragmentShaderSource =
    "#version 300 es\n"
    "precision highp float;\n"
    "out vec4 fragColor;\n"
    "void main() {\n"
    "    fragColor = vec4(0.0, 1.0, 0.0, 1.0); \n"
    "}\n";

Triangle::Triangle() {}

Triangle::~Triangle() {}

void Triangle::compileShader(GLuint shader, const GLchar* source) {
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        printf("ERROR::SHADER::COMPILATION_FAILED\n%s\n", infoLog);
    }
}

void Triangle::linkProgram(GLuint program, GLuint vertexShader, GLuint fragmentShader) {
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        printf("ERROR::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
    }
}

void Triangle::initOpenGL() {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    compileShader(vertexShader, vertexShaderSource);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    compileShader(fragmentShader, fragmentShaderSource);
    program = glCreateProgram();
    linkProgram(program, vertexShader, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    GLfloat vertices[] = {
        0.0f,  0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f
    };
    
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
}

void Triangle::drawOpenGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Triangle::clearOpenGL() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}
