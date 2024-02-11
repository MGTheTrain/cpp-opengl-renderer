#pragma once
#include <vector>


namespace Mgtt::Rendering {
    struct GltfNode;

    enum GltfTextureWrapType {
        TEXTURE_WRAP_REPEAT,
        TEXTURE_WRAP_CLAMP_TO_EDGE,
        TEXTURE_WRAP_MIRRORED_REPEAT
    };

    enum class GltfType { ASCII, BINARY };

    struct GltfAnimationChannel {
        GltfAnimationChannel();
        int32_t sampler;
        int32_t target_node;
        std::string target_path;
    };

    struct GltfAnimationSampler {
        GltfAnimationSampler();
        int32_t input;
        int32_t output;
        std::string interpolation;
    };

    struct GltfAnimation {
        std::string name;
        std::vector<GltfAnimationChannel> channels;
        std::vector<GltfAnimationSampler> samplers;
    };

    struct GltfSkin {
    GltfSkin();
        std::string name;
        int32_t inverseBindMatrices;
        int32_t skeleton;        // The index of the node used as a skeleton root
        std::vector<int> joints; // Indices of skeleton nodes
    };

    struct GltfSampler {
        std::string name;
        int32_t minFilter =-1;
        int32_t magFilter =
            -1; 
        int32_t wrapS = GltfTextureWrapType::TEXTURE_WRAP_REPEAT; 
        int32_t wrapT = GltfTextureWrapType::TEXTURE_WRAP_REPEAT; 
        int32_t wrapR = GltfTextureWrapType::TEXTURE_WRAP_REPEAT;
    };

    struct GltfImage {
        GltfImage();
        std::string name;
        int32_t width;
        int32_t height;
        int32_t component;
        std::vector<unsigned char> image;
        int32_t bufferView;   
        std::string mimeType; 
        std::string uri;      
    };

    struct GltfTexture {
        GltfTexture();
        std::string name;
        int32_t sampler;
        int32_t source;
    };

    struct GltfTextureInfo {
        GltfTextureInfo();
        int32_t index;
        int32_t texCoord; 
    };

    struct GltfNormalTextureInfo {
        GltfNormalTextureInfo();
        int32_t index;
        int32_t texCoord;
        double scale; 
    };

    struct GltfOcclusionTextureInfo {
        GltfOcclusionTextureInfo();
        int32_t index;
        int32_t texCoord; 
        double strength;  
    };

    struct GltfPbrMetallicRoughness {
        GltfPbrMetallicRoughness();
        double metallicFactor;  
        double roughnessFactor; 
        glm::vec4 baseColor;    
        GltfTextureInfo baseColorTexture;
        GltfTextureInfo metallicRoughnessTexture;
    };

    struct GltfSpecularGlossiness {
        GltfSpecularGlossiness();
        glm::vec4 diffuseColor;  
        glm::vec3 specularColor; 
        float glosinessFactor;   
        GltfTextureInfo diffuseTexture;
        GltfTextureInfo specularGlossinessTexture;
    };

    struct GltfMaterial {
        GltfMaterial();
        std::string name;
        glm::vec3 emissiveColor; 
        std::string alphaMode;   
        double alphaCutoff;      
        bool doubleSided;        

        GltfPbrMetallicRoughness pbrMetallicRoughness;
        GltfNormalTextureInfo normalTexture;
        GltfOcclusionTextureInfo occlusionTexture;
        GltfTextureInfo emissiveTexture;
        GltfSpecularGlossiness specularGlossiness;
        // std::map<> extensionMap;
    };

    struct GltfBufferView {
        GltfBufferView();
        std::string name;
        int32_t buffer;
        size_t byteOffset;
        size_t byteLength;
        size_t byteStride;
        int32_t target;
    };

    struct GltfAccessor {
        GltfAccessor();
        std::string name;
        int32_t bufferView; 
        size_t byteOffset; 
        int32_t componentType;
        size_t count;
        int32_t type;

        glm::vec3 minValues;
        glm::vec3 maxValues;
    };

    struct GltfPrimitive {
        GltfPrimitive();
        std::map<std::string, int> attributes; 
        int32_t material; 
        int32_t indices;  
        int32_t mode;
        std::vector<std::map<std::string, int>> targets; 
    };

    struct GltfMesh {
        GltfMesh();
        std::string name;
        std::vector<GltfPrimitive> primitives;
        glm::vec4 weights; 
    };

    struct GltfNode {
        GltfNode();
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

    struct GltfBuffer {
        GltfBuffer();
        int32_t byteLength;
        std::vector<unsigned char> data;
        std::string uri; 
    };

    struct GltfAsset {
        std::string version;
        std::string generator;
        std::string minVersion;
        std::string copyright;
    };

    struct GltfScene {
        std::string name;
        std::vector<int> nodes;
    };
}