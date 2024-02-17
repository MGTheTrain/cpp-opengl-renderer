#version 330 core

// Essential parts from: https://github.com/SaschaWillems/Vulkan-glTF-PBR

out vec4 fragmentColor;

in vec3 outVertexNormal;
in vec3 outWorldPosition;
in vec2 outVertexTextureCoordinates;

uniform vec3 lightPosition;
uniform vec3 cameraPosition;

uniform	vec4 baseColorFactor;
uniform vec4 emissiveFactor;
uniform vec4 occlusionFactor;
uniform vec4 diffuseFactor;
uniform vec4 specularFactor;

uniform	float metallicFactor;
uniform	float roughnessFactor;
uniform	bool alphaMaskSet;
uniform	float alphaMaskCutoff;

// texture maps
uniform sampler2D colorMap;
uniform sampler2D physicalDescriptorMap;
uniform sampler2D normalMap;
uniform sampler2D aoMap;
uniform sampler2D emissiveMap;

// booleans enabling or disabling tetures
uniform	bool baseColorTextureSet;
uniform	bool physicalDescriptorTextureSet;
uniform	bool normalTextureSet;
uniform	bool occlusionTextureSet;
uniform	bool emissiveTextureSet;

// sampler cube
uniform samplerCube samplerEnvMap;
uniform samplerCube samplerIrradianceMap;

// brdf 
uniform sampler2D samplerBrdfLut;

// ibl 
uniform	float scaleIblAmbient;
uniform float prefilteredCubeMipLevels;

// constants
const vec3 dielectric = vec3(0.04);
const float PI = 3.14;
// const float epsilon = 1e-6;
const float epsilon = 0.1;


vec3 GetNormal(){
    vec3 tangentNormal = texture(normalMap, outVertexTextureCoordinates).xyz * 2.0 - 1.0;

    vec3 q1 = dFdx(outWorldPosition);
    vec3 q2 = dFdy(outWorldPosition);
    vec2 st1 = dFdx(outVertexTextureCoordinates);
    vec2 st2 = dFdy(outVertexTextureCoordinates);

    vec3 N = normalize(outVertexNormal);
    vec3 T = normalize(q1 * st2.t - q2 * st1.t);
    vec3 B = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);  
}

// The following equation models the Fresnel reflectance term of the spec equation (aka F())
// Implementation of fresnel from [4], Equation 15
vec3 SpecularReflection(vec3 reflectance0, vec3 reflectance90, float VdotH)
{
	return reflectance0 + 
        (reflectance90 - reflectance0) * 
        pow(clamp(1.0 - VdotH, 0.0, 1.0), 5.0);
}

// This calculates the specular geometric attenuation (aka G()),
// where rougher material will reflect less light back to the viewer.
// This implementation is based on [1] Equation 4, and we adopt their modifications to
// alphaRoughness as input as originally proposed in [2].
float GeometricOcclusion(float NdotL, float NdotV, float alphaRoughness)
{
	float r = alphaRoughness;

	float attenuationL = 
        2.0 * NdotL / (NdotL + sqrt(r * r + (1.0 - r * r) * (NdotL * NdotL)));
	float attenuationV = 
        2.0 * NdotV / (NdotV + sqrt(r * r + (1.0 - r * r) * (NdotV * NdotV)));

	return attenuationL * attenuationV;
}


// The following equation(s) model the distribution of microfacet normals across the area being drawn (aka D())
// Implementation from "Average Irregularity Representation of a Roughened Surface for Ray Reflection" by T. S. Trowbridge, and K. P. Reitz
// Follows the distribution function recommended in the SIGGRAPH 2013 course notes from EPIC Games [1], Equation 3.
float MicrofacetDistribution(float alphaRoughness, float NdotH)
{
	float roughnessSq = 
        alphaRoughness * alphaRoughness;
	float f = (NdotH * roughnessSq - NdotH) * NdotH + 1.0;
	return roughnessSq / (PI * f * f);
}

// Gets metallic factor from specular glossiness workflow inputs 
float ConvertSpecularToMetallic(vec3 diffuse, vec3 specular, float maxSpecular) {
	float perceivedDiffuse = sqrt(
        0.299 * diffuse.r * diffuse.r + 
        0.587 * diffuse.g * diffuse.g + 
        0.114 * diffuse.b * diffuse.b);
	float perceivedSpecular =  sqrt(
        0.299 * specular.r * specular.r + 
        0.587 * specular.g * specular.g + 
        0.114 * specular.b * specular.b);
	if (perceivedSpecular < 0.04) {
		return 0.0;
	}
	float a = 0.04;
	float b = perceivedDiffuse * 
        (1.0 - maxSpecular) / (1.0 - 0.04) + 
        perceivedSpecular - 2.0 * 0.04;
	float c = 0.04 - perceivedSpecular;
	float D = max(b * b - 4.0 * a * c, 0.0);
	return clamp((-b + sqrt(D)) / (2.0 * a), 0.0, 1.0);
}

// Calculation of the lighting contribution from an optional Image Based Light source.
// Precomputed Environment Maps are required uniform inputs and are computed as outlined in [1].
// See our README.md on Environment Maps [3] for additional discussion.
vec3 GetIBLContribution(
	float perceptualRoughness,
	float NdotV,
	vec3 specularColor,
	vec3 diffuseColor, 
	vec3 n,
	vec3 reflection) {

	float lod = perceptualRoughness;

	vec3 envLight = textureLod(samplerEnvMap, reflection, lod).rgb;

	// vec2 brdfTest = vec2(NdotV, 1.0 - perceptualRoughness); // false
	vec2 brdfTest = texture(samplerBrdfLut, vec2(NdotV, 1.0 - perceptualRoughness)).rg;
	vec3 specular = envLight * 
		(specularColor * brdfTest.x + brdfTest.y) * scaleIblAmbient; 

	vec3 irradianceLight = 
		textureLod(samplerIrradianceMap, reflection, lod).rgb;
	vec3 diffuse = 
		irradianceLight * diffuseColor * scaleIblAmbient;

	return specular + diffuse;
	// return specular;
	// return diffuse;
}


void main() {    
    float perceptualRoughness;
	float metallic;
	vec3 diffuseColor;
	vec4 baseColor;

    perceptualRoughness = roughnessFactor;
    metallic = metallicFactor;

    if (physicalDescriptorTextureSet) {
        vec4 mrSample = texture(physicalDescriptorMap, outVertexTextureCoordinates);
        perceptualRoughness = mrSample.g * perceptualRoughness;
    } 

    if (baseColorTextureSet) {
        baseColor = texture(colorMap, outVertexTextureCoordinates) * baseColorFactor;
    } else {
        baseColor = baseColorFactor;
    }

    if (alphaMaskSet) {
        if (baseColor.a < alphaMaskCutoff) {
            discard;
        }
    }

    // diffuse color
    diffuseColor = baseColor.rgb * (vec3(1.0) - dielectric);
	diffuseColor *= 1.0 - metallic;

    // base color
    vec3 specularColor = mix(dielectric, baseColor.rgb, metallic);

    // fragmentColor = vec4(specularColor, 1.0);

	// Compute reflectance.
	float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);

    float reflectance90 = clamp(reflectance * 25.0, 0.0, 1.0);
	vec3 specularEnvironmentR0 = specularColor.rgb;
	vec3 specularEnvironmentR90 = vec3(1.0, 1.0, 1.0) * reflectance90;

    // per fragment/pixel vectors 
    // calculate for each fragment/pixel light direction and view direction vectors
    vec3 norm = (normalTextureSet) ? GetNormal() : normalize(outVertexNormal);
    vec3 viewDirection = normalize(cameraPosition - outWorldPosition);
    vec3 lightDirection = normalize(lightPosition - outWorldPosition);
    vec3 halfVector = normalize(lightDirection + viewDirection); // half vector between lightDirection and viewDirection
	
    float NdotL = clamp(dot(norm, lightDirection), 0.001, 1.0);
	float NdotV = clamp(dot(norm, viewDirection), 0.001, 1.0);
	float NdotH = clamp(dot(norm, halfVector), 0.0, 1.0);
	float LdotH = clamp(dot(lightDirection, halfVector), 0.0, 1.0);
    float VdotH = clamp(dot(viewDirection, halfVector), 0.0, 1.0);
    //vec3 reflection = -reflect(viewDirection, norm);
	vec3 reflection = -reflect(norm, viewDirection);
	reflection.y *= -1.0; 

    vec3 F = SpecularReflection(specularEnvironmentR0, specularEnvironmentR90, VdotH);
    float alphaRoughness = perceptualRoughness * perceptualRoughness;
	float G = GeometricOcclusion(NdotL, NdotV, alphaRoughness);
    // fragmentColor.rgb = vec3(G);
    // fragmentColor.a = 1.0;

    float D = MicrofacetDistribution(alphaRoughness, NdotH);

    // fragmentColor.rgb = vec3(D);
    // fragmentColor.a = 1.0;

    // Calculation of analytical lighting contribution
	vec3 diffuseContrib = (1.0 - F) * (diffuseColor / PI);

    // fragmentColor.rgb = diffuseContrib;
    // fragmentColor.a = 1.0;

    vec3 specContrib = F * G * D / (4.0 * NdotL * NdotV);
    // vec3 specContrib = F * G * D / max(epsilon, (4.0 * NdotL * NdotV));
    // fragmentColor.rgb = specContrib;
    // fragmentColor.a = 1.0;

    // vec3 color = diffuseContrib + specContrib;

	// Important: NdotL * vec3(1.0) as factor
	vec3 color = NdotL * vec3(1.0) * (diffuseContrib + specContrib);

    // fragmentColor = vec4(color, 1.0); 

	// check environment maps
	// fragmentColor = vec4(texture(samplerIrradianceMap, vec3(outVertexTextureCoordinates.xy, 1.0)).rgb, 1.0); 
	// fragmentColor = vec4(texture(samplerEnvMap, vec3(outVertexTextureCoordinates.xy, 1.0)).rgb, 1.0); 

    // get IBL contribution
	color += GetIBLContribution(
				perceptualRoughness,
				NdotV,
				specularColor,
				diffuseColor, 
				norm,
				reflection); 

	if (occlusionTextureSet) {
		float ao = texture(aoMap, outVertexTextureCoordinates).r;
		color = mix(color, color * ao, occlusionFactor.rgb);
	}
	else{
		color = mix(color, color + 0.001, occlusionFactor.rgb);
	}

	if (emissiveTextureSet) {
		vec3 emissive = texture(emissiveMap, outVertexTextureCoordinates).rgb;
		
		emissive = emissive * emissiveFactor.rgb;
		color += emissive;
	}
	else{
		color += emissiveFactor.rgb;
	}

    // color = SRGBtoLINEAR(vec4(color, baseColorFactor.a));	
    fragmentColor = vec4(color, baseColorFactor.a);	

	// diplay occlusion effect
	// float ao = texture(aoMap, outVertexTextureCoordinates).r;
	// color = mix(color, color * ao, occlusionFactor.rgb);
	// fragmentColor = vec4(color, 1.0); 

    // display textures   
    // base color 
    // fragmentColor = vec4(texture(colorMap, outVertexTextureCoordinates).rgb, 1.0);

    // physical descriptor map
    // fragmentColor = vec4(texture(physicalDescriptorMap, outVertexTextureCoordinates).rgb, 1.0);

    // normal map
    // fragmentColor = vec4(texture(normalMap, outVertexTextureCoordinates).rgb, 1.0);
}