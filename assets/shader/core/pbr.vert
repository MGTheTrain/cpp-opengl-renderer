#version 330 core

layout (location = 0) in vec3 inVertexPosition;
layout (location = 1) in vec3 inVertexNormal;
layout (location = 2) in vec2 inVertexTextureCoordinates;

out vec3 outVertexNormal;
out vec3 outWorldPosition;
out vec2 VertexTextureCoordinates;

uniform mat4 model; 
uniform mat4 mvp;
uniform mat4 matrix;

void main()
{
	vec4 localVertexPosition;
    localVertexPosition = matrix * vec4(inVertexPosition, 1.0);

    mat4 modelMatrix = model * matrix; 
    outVertexNormal = mat3(modelMatrix) * inVertexNormal;
    outWorldPosition = mat3(modelMatrix) * inVertexPosition;

	outVertexTextureCoordinates = inVertexTextureCoordinates;
	
	vec3 normalizedVertexPosition = localVertexPosition.xyz / localVertexPosition.w;
	gl_Position = mvp * vec4(normalizedVertexPosition, 1.0);
}
