out vec4 fragColor;

in vec2 texCoord;
uniform sampler2D textureMap;

void main() {
	fragColor = texture(textureMap, texCoord);
}