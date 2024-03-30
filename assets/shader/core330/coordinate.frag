#version 330 core

out vec4 fragmentColor;

in vec2 outVertexTextureCoordinates;
uniform sampler2D textureMap;

void main() {
	fragmentColor = texture(textureMap, outVertexTextureCoordinates);
}