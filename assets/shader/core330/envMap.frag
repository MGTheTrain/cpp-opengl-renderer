#version 330 core

// Essential parts prom: https://learnopengl.com/PBR/IBL/Specular-IBL

out vec4 fragmentColor;

in vec3 outVertexTextureCoordinates;

uniform samplerCube envMap;

void main() {    
    fragmentColor = texture(envMap, outVertexTextureCoordinates);
}