#pragma once
#include <vector>
#include <string>
#include <map>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

namespace Mgtt::Rendering::Gltf {
    struct Node;

    enum GltfTextureWrapType {
        TEXTURE_WRAP_REPEAT,
        TEXTURE_WRAP_CLAMP_TO_EDGE,
        TEXTURE_WRAP_MIRRORED_REPEAT
    };

    enum class GltfType { ASCII, BINARY };

    struct AnimationChannel {
        AnimationChannel();
        int32_t sampler;
        int32_t target_node;
        std::string target_path;
    };

    struct AnimationSampler {
        AnimationSampler();
        int32_t input;
        int32_t output;
        std::string interpolation;
    };

    struct Animation {
        std::string name;
        std::vector<AnimationChannel> channels;
        std::vector<AnimationSampler> samplers;
    };

    struct Skin {
        Skin();
        std::string name;
        int32_t inverseBindMatrices;
        int32_t skeleton; 
        std::vector<int> joints;
    };

    struct Sampler {
        std::string name;
        int32_t minFilter =-1;
        int32_t magFilter = -1; 
        int32_t wrapS = GltfTextureWrapType::TEXTURE_WRAP_REPEAT; 
        int32_t wrapT = GltfTextureWrapType::TEXTURE_WRAP_REPEAT; 
        int32_t wrapR = GltfTextureWrapType::TEXTURE_WRAP_REPEAT;
    };

    struct Image {
        Image();
        std::string name;
        int32_t width;
        int32_t height;
        int32_t component;
        std::vector<unsigned char> image;
        int32_t bufferView;   
        std::string mimeType; 
        std::string uri;      
    };

    struct Texture {
        Texture();
        std::string name;
        int32_t sampler;
        int32_t source;
    };

    struct TextureInfo {
        TextureInfo();
        int32_t index;
        int32_t texCoord; 
    };

    struct NormalTextureInfo {
        NormalTextureInfo();
        int32_t index;
        int32_t texCoord;
        double scale; 
    };

    struct OcclusionTextureInfo {
        OcclusionTextureInfo();
        int32_t index;
        int32_t texCoord; 
        double strength;  
    };

    struct PbrMetallicRoughness {
        PbrMetallicRoughness();
        double metallicFactor;  
        double roughnessFactor; 
        glm::vec4 baseColor;    
        TextureInfo baseColorTexture;
        TextureInfo metallicRoughnessTexture;
    };

    struct SpecularGlossiness {
        SpecularGlossiness();
        glm::vec4 diffuseColor;  
        glm::vec3 specularColor; 
        float glosinessFactor;   
        TextureInfo diffuseTexture;
        TextureInfo specularGlossinessTexture;
    };

    struct Material {
        Material();
        std::string name;
        glm::vec3 emissiveColor; 
        std::string alphaMode;   
        double alphaCutoff;      
        bool doubleSided;        

        PbrMetallicRoughness pbrMetallicRoughness;
        NormalTextureInfo normalTexture;
        OcclusionTextureInfo occlusionTexture;
        TextureInfo emissiveTexture;
        SpecularGlossiness specularGlossiness;
        // std::map<> extensionMap;
    };

    struct BufferView {
        BufferView();
        std::string name;
        int32_t buffer;
        size_t byteOffset;
        size_t byteLength;
        size_t byteStride;
        int32_t target;
    };

    struct Accessor {
        Accessor();
        std::string name;
        int32_t bufferView; 
        size_t byteOffset; 
        int32_t componentType;
        size_t count;
        int32_t type;

        glm::vec3 minValues;
        glm::vec3 maxValues;
    };

    struct Primitive {
        Primitive();
        std::map<std::string, int> attributes; 
        int32_t material; 
        int32_t indices;  
        int32_t mode;
        std::vector<std::map<std::string, int>> targets; 
    };

    struct Mesh {
        Mesh();
        std::string name;
        std::vector<Primitive> primitives;
        glm::vec4 weights; 
    };

    struct Node {
        Node();
        std::string name;
        int32_t skin;
        int32_t mesh;
        std::vector<int> children;
        glm::quat rotation;
        glm::vec3 scale;   
        glm::vec3 translation;
        glm::mat4 matrix;  
        glm::vec4 weights; 
    };

    struct Buffer {
        Buffer();
        int32_t byteLength;
        std::vector<unsigned char> data;
        std::string uri; 
    };

    struct Asset {
        std::string version;
        std::string generator;
        std::string minVersion;
        std::string copyright;
    };

    struct Scene {
        std::string name;
        std::vector<int> nodes;
    };
}