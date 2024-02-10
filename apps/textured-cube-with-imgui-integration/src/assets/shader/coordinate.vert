#version 330 core

layout (location = 0) in vec3 inVertexPosition;
layout (location = 1) in vec2 inVertexTextureCoordinates;

out vec2 texCoord;
uniform mat4 mvp;

void main() {
	gl_Position = projection * view * model * vec4(inVertexPosition, 1.0f);
	TexCoord = vec2(inVertexTextureCoord.x, inVertexTextureCoord.y)inates;
}