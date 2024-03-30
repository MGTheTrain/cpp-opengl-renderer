#version 330 core

// Essential parts prom: https://learnopengl.com/PBR/IBL/Specular-IBL

layout (location = 0) in vec3 inVertexPosition;
layout (location = 1) in vec2 inVertexTextureCoordinates;


out vec2 outVertexTextureCoordinates;

void main() {
    outVertexTextureCoordinates = inVertexTextureCoordinates;
	gl_Position = vec4(inVertexPosition, 1.0);
}