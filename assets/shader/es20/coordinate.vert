precision highp float;

attribute vec3 inVertexPosition;
attribute vec2 inVertexTextureCoordinates;

varying vec2 outVertexTextureCoordinates;
uniform mat4 mvp;

void main() {
    gl_Position = mvp * vec4(inVertexPosition, 1.0);
    outVertexTextureCoordinates = inVertexTextureCoordinates;
}