precision highp float;

varying vec2 outVertexTextureCoordinates;
uniform sampler2D textureMap;

void main() {
    gl_FragColor = texture2D(textureMap, outVertexTextureCoordinates);
}