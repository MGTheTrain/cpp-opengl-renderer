#version 300 es

precision highp int;
precision highp float;

// Essential parts prom: https://learnopengl.com/PBR/IBL/Specular-IBL

out vec4 fragmentColor;
in vec3 fragmentPosition;

uniform sampler2D equirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 v) {
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main() {		
    vec2 uv = SampleSphericalMap(normalize(fragmentPosition));
    vec3 color = texture(equirectangularMap, uv).rgb;
    
    fragmentColor = vec4(color, 1.0);
    // fragmentColor = vec4(1.0, 0.0, 0.0, 1.0);
}