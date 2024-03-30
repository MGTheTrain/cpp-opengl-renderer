#version 330 core

// Essential parts prom: https://learnopengl.com/PBR/IBL/Specular-IBL

layout (location = 0) in vec3 inVertexPosition;

out vec3 fragmentPosition;

uniform mat4 projection;
uniform mat4 view;

void main() {
    fragmentPosition = inVertexPosition;  
    gl_Position =  projection * view * vec4(fragmentPosition, 1.0);
}