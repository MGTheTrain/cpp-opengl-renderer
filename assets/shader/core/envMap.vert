#version 330 core

// Essential parts prom: https://learnopengl.com/PBR/IBL/Specular-IBL

layout (location = 0) in vec3 inVertexPosition;

out vec3 outVertexTextureCoordinates;

uniform mat4 projection;
uniform mat4 view;

void main() {
    outVertexTextureCoordinates = inVertexPosition;
    mat4 rotView = mat4(mat3(view));
    vec4 pos = projection * rotView * vec4(inVertexPosition, 1.0);
    gl_Position = pos.xyww;
}  