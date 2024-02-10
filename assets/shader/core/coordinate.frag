out vec4 FragColor;

in vec2 texCoord;
uniform sampler2D textureMap;

void main() {
	FragColor = texture(textureMap, texCoord);
}