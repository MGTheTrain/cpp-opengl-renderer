#version 300 es

precision highp int;
precision highp float;

out vec4 fragmentColor;

in vec2 outVertexTextureCoordinates;
uniform sampler2D textureMap;

void main() {
	fragmentColor = texture(textureMap, outVertexTextureCoordinates);
}