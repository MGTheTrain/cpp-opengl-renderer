#version 330 core

layout (location = 0) in vec3 inVertexPosition;
layout (location = 1) in vec2 inVertexTextureCoordinates;

out vec2 outVertexTextureCoordinates;
uniform mat4 mvp;

void main() {
	gl_Position = mvp * vec4(inVertexPosition, 1.0f);
	outVertexTextureCoordinates = inVertexTextureCoordinates;
}