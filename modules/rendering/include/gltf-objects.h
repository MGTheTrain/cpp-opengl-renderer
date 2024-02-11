// The MIT License
//
// Copyright (c) 2024 MGTheTrain
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// Maintainers:
// - MGTheTrain 
//
// Contributors:
// - TBD

#pragma once
#include <vector>
#include <string>
#include <map>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

namespace Mgtt::Rendering::Gltf {
     /**
     * @brief Represents a node in the GLTF structure.
     */
    struct Node;

    /**
     * @brief Enumerates the possible texture wrap types in GLTF.
     */
    enum GltfTextureWrapType {
        TEXTURE_WRAP_REPEAT,              /**< Repeat texture wrapping mode. */
        TEXTURE_WRAP_CLAMP_TO_EDGE,      /**< Clamp to edge texture wrapping mode. */
        TEXTURE_WRAP_MIRRORED_REPEAT      /**< Mirrored repeat texture wrapping mode. */
    };

    /**
     * @brief Enumerates the GLTF types (ASCII or BINARY).
     */
    enum class GltfType { ASCII, BINARY };

    /**
     * @brief Represents a channel in GLTF animation.
     */
    struct AnimationChannel {
        /**
         * @brief Default constructor for AnimationChannel.
         */
         AnimationChannel(): sampler(-1), target_node(-1) {}
        int32_t sampler;                  /**< Index of the animation sampler. */
        int32_t target_node;              /**< Index of the target node in the animation. */
        std::string target_path;          /**< Target property path of the animation channel. */
    };

    /**
     * @brief Represents a sampler in GLTF animation.
     */
    struct AnimationSampler {
        /**
         * @brief Default constructor for AnimationSampler.
         */
        AnimationSampler(): input(-1), output(-1) {}
        int32_t input;                    /**< Index of the input accessor containing keyframe times. */
        int32_t output;                   /**< Index of the output accessor containing keyframe data. */
        std::string interpolation;        /**< Interpolation algorithm for the sampler. */
    };

    /**
     * @brief Represents an animation in GLTF.
     */
    struct Animation {
        /**
         * @brief Represents a GLTF animation.
         */
        Animation(): name("") {}
        std::string name;                  /**< Name of the animation. */
        /**
         * @brief Animation channels.
         */
        std::vector<AnimationChannel> channels;
        /**
         * @brief Animation samplers.
         */
        std::vector<AnimationSampler> samplers;
    };

    /**
     * @brief Represents a skin in GLTF.
     */
    struct Skin {
        /**
         * @brief Default constructor for Skin.
         */
        Skin(): inverseBindMatrices(-1), skeleton(-1) {}
        std::string name;                  /**< Name of the skin. */
        int32_t inverseBindMatrices;      /**< Index of the accessor containing inverse bind matrices. */
        int32_t skeleton;                  /**< Index of the skeleton node. */
        /**
         * @brief Indices of joints in the skin.
         */
        std::vector<int> joints;
    };

    /**
     * @brief Represents a texture sampler in GLTF.
     */
    struct Sampler {
        Sampler(): minFilter(-1), magFilter(-1), wrapS(GltfTextureWrapType::TEXTURE_WRAP_REPEAT),
              wrapT(GltfTextureWrapType::TEXTURE_WRAP_REPEAT), wrapR(GltfTextureWrapType::TEXTURE_WRAP_REPEAT) {}
        std::string name;                  /**< Name of the sampler. */
        int32_t minFilter = -1;            /**< Minification filter. */
        int32_t magFilter = -1;            /**< Magnification filter. */
        int32_t wrapS = GltfTextureWrapType::TEXTURE_WRAP_REPEAT;   /**< Texture wrap mode (S axis). */
        int32_t wrapT = GltfTextureWrapType::TEXTURE_WRAP_REPEAT;   /**< Texture wrap mode (T axis). */
        int32_t wrapR = GltfTextureWrapType::TEXTURE_WRAP_REPEAT;   /**< Texture wrap mode (R axis). */
    };

     /**
     * @brief Represents an image in GLTF.
     */
    struct Image {
        /**
         * @brief Default constructor for Image.
         */
        Image(): width(0), height(0), component(0), bufferView(-1) {}
        std::string name;                  /**< Name of the image. */
        int32_t width;                     /**< Width of the image. */
        int32_t height;                    /**< Height of the image. */
        int32_t component;                 /**< Number of components in the image. */
        std::vector<unsigned char> image;  /**< Image data. */
        int32_t bufferView;                /**< Index of the buffer view containing the image data. */
        std::string mimeType;              /**< MIME type of the image. */
        std::string uri;                   /**< URI of the image. */
    };

    /**
     * @brief Represents a texture in GLTF.
     */
    struct Texture {
        /**
         * @brief Default constructor for Texture.
         */
        Texture(): sampler(-1), source(-1) {}
        std::string name;                  /**< Name of the texture. */
        int32_t sampler;                   /**< Index of the texture sampler. */
        int32_t source;                    /**< Index of the texture image source. */
    };

    /**
     * @brief Represents texture information in GLTF.
     */
    struct TextureInfo {
        /**
         * @brief Default constructor for TextureInfo.
         */
        TextureInfo(): index(-1), texCoord(0) {}
        int32_t index;                     /**< Index of the texture. */
        int32_t texCoord;                  /**< Texture coordinate set. */
    };

    /**
     * @brief Represents normal texture information in GLTF.
     */
    struct NormalTextureInfo {
        /**
         * @brief Default constructor for NormalTextureInfo.
         */
        NormalTextureInfo(): index(-1), texCoord(0), scale(1.0) {}
        int32_t index;                     /**< Index of the normal texture. */
        int32_t texCoord;                  /**< Texture coordinate set. */
        double scale;                      /**< Scaling factor for the normal texture. */
    };

    /**
     * @brief Represents occlusion texture information in GLTF.
     */
    struct OcclusionTextureInfo {
        /**
         * @brief Default constructor for OcclusionTextureInfo.
         */
        OcclusionTextureInfo(): index(-1), texCoord(0), strength(1.0) {}

        int32_t index;                     /**< Index of the occlusion texture. */
        int32_t texCoord;                  /**< Texture coordinate set. */
        double strength;                   /**< Strength of the occlusion effect. */
    };

    /**
     * @brief Represents the PBR metallic roughness in GLTF.
     */
    struct PbrMetallicRoughness {
        /**
         * @brief Default constructor for PbrMetallicRoughness.
         */
        PbrMetallicRoughness(): metallicFactor(1.0), roughnessFactor(1.0), baseColor(glm::vec4(1.0)), metallicRoughnessTexture(), baseColorTexture() {}
        double metallicFactor;             /**< Factor for metallic property. */
        double roughnessFactor;            /**< Factor for roughness property. */
        glm::vec4 baseColor;               /**< Base color factor. */
        TextureInfo baseColorTexture;      /**< Texture information for the base color. */
        TextureInfo metallicRoughnessTexture;  /**< Texture information for metallic and roughness properties. */
    };

    /**
     * @brief Represents the specular-glossiness properties in GLTF.
     */
    struct SpecularGlossiness {
        /**
         * @brief Default constructor for SpecularGlossiness.
         */
        SpecularGlossiness(): diffuseColor(glm::vec4(1.0)), specularColor(glm::vec3(1.0)), 
            glosinessFactor(1.0), diffuseTexture(), specularGlossinessTexture() {}
        glm::vec4 diffuseColor;            /**< Diffuse color factor. */
        glm::vec3 specularColor;           /**< Specular color factor. */
        float glosinessFactor;             /**< Glossiness factor. */
        TextureInfo diffuseTexture;        /**< Texture information for diffuse color. */
        TextureInfo specularGlossinessTexture;  /**< Texture information for specular color and glossiness. */
    };

    /**
     * @brief Represents a material in GLTF.
     */
    struct Material {
        /**
         * @brief Default constructor for Material.
         */
        Material(): alphaCutoff(0.5), doubleSided(false), pbrMetallicRoughness(), normalTexture(), occlusionTexture(), emissiveTexture(), specularGlossiness() {}
        std::string name;                  /**< Name of the material. */
        glm::vec3 emissiveColor;           /**< Emissive color factor. */
        std::string alphaMode;             /**< Alpha rendering mode. */
        double alphaCutoff;                /**< Alpha cutoff threshold. */
        bool doubleSided;                  /**< Indicates if the material is double-sided. */
        PbrMetallicRoughness pbrMetallicRoughness;   /**< PBR metallic roughness properties. */
        NormalTextureInfo normalTexture;   /**< Normal texture information. */
        OcclusionTextureInfo occlusionTexture;     /**< Occlusion texture information. */
        TextureInfo emissiveTexture;       /**< Texture information for emissive color. */
        SpecularGlossiness specularGlossiness;     /**< Specular-glossiness properties. */
        // std::map<> extensionMap;         /**< Extension-specific properties (commented out). */
    };

    /**
     * @brief Represents a buffer view in GLTF.
     */
    struct BufferView {
        /**
         * @brief Default constructor for BufferView.
         */
        BufferView(): buffer(-1), byteOffset(0), byteLength(0), byteStride(0), target(-1) {}
        std::string name;                  /**< Name of the buffer view. */
        int32_t buffer;                    /**< Index of the buffer. */
        size_t byteOffset;                 /**< Byte offset into the buffer. */
        size_t byteLength;                 /**< Length of the buffer view in bytes. */
        size_t byteStride;                 /**< Stride of the buffer view. */
        int32_t target;                    /**< The target that the GPU buffer should be bound to. */
    };

    /**
     * @brief Represents an accessor in GLTF.
     */
    struct Accessor {
        /**
         * @brief Default constructor for Accessor.
         */
         Accessor(): bufferView(-1), byteOffset(0), componentType(0), count(0), type(0), minValues(glm::vec3(0.0)), maxValues(glm::vec3(0.0)) {}
        std::string name;                  /**< Name of the accessor. */
        int32_t bufferView;                /**< Index of the buffer view. */
        size_t byteOffset;                 /**< Byte offset into the buffer view. */
        int32_t componentType;             /**< Component type of the accessor's data. */
        size_t count;                      /**< Number of elements in the accessor. */
        int32_t type;                      /**< The data type of components in the accessor. */
        glm::vec3 minValues;               /**< Minimum bounds of the attribute data. */
        glm::vec3 maxValues;               /**< Maximum bounds of the attribute data. */
    };

    /**
     * @brief Represents a primitive in GLTF.
     */
    struct Primitive {
        /**
         * @brief Default constructor for Primitive.
         */
        Primitive(): material(-1), indices(-1), mode(4) {}

        /**
         * @brief Attribute mappings for the primitive.
         */
        std::map<std::string, int> attributes;
        int32_t material;                  /**< Index of the material to apply to the primitive. */
        int32_t indices;                   /**< Index of the accessor containing the primitive indices. */
        int32_t mode;                      /**< The type of primitive to render. */
        /**
         * @brief Target attributes for morph targets.
         */
        std::vector<std::map<std::string, int>> targets;
    };

    /**
     * @brief Represents a mesh in GLTF.
     */
    struct Mesh {
        /**
         * @brief Default constructor for Mesh.
         */
        Mesh(): weights(glm::vec4(0.0)) {}
        std::string name;                  /**< Name of the mesh. */
        /**
         * @brief Primitives composing the mesh.
         */
        std::vector<Primitive> primitives;
        glm::vec4 weights;                  /**< Morph target weights. */
    };

    /**
     * @brief Represents a node in GLTF.
     */
    struct Node {
        /**
         * @brief Default constructor for Node.
         */
        Node(): skin(-1), mesh(-1), rotation(glm::quat(1.0, 0.0, 0.0, 0.0)), scale(glm::vec3(1.0)),
              translation(glm::vec3(0.0)), matrix(glm::mat4(1.0)), weights(glm::vec4(0.0)) {}
        std::string name;                  /**< Name of the node. */
        int32_t skin;                      /**< Index of the skin. */
        int32_t mesh;                      /**< Index of the mesh. */
        /**
         * @brief Indices of child nodes.
         */
        std::vector<int> children;
        glm::quat rotation;                 /**< Rotation of the node in quaternion form. */
        glm::vec3 scale;                    /**< Scaling factor of the node. */
        glm::vec3 translation;              /**< Translation of the node. */
        glm::mat4 matrix;                   /**< Transformation matrix of the node. */
        glm::vec4 weights;                  /**< Morph target weights. */
    };

    /**
     * @brief Represents a buffer in GLTF.
     */
    struct Buffer {
        /**
         * @brief Default constructor for Buffer.
         */
        Buffer(): byteLength(0) {}
        int32_t byteLength;                 /**< Length of the buffer in bytes. */
        std::vector<unsigned char> data;    /**< Buffer data. */
        std::string uri;                    /**< URI of the external buffer. */
    };

    /**
     * @brief Represents asset information in GLTF.
     */
    struct Asset {
        Asset(): version(""), generator(""), minVersion(""), copyright("") {}
        std::string version;                /**< GLTF version. */
        std::string generator;              /**< Software that generated the GLTF. */
        std::string minVersion;             /**< Minimum supported GLTF version. */
        std::string copyright;              /**< Copyright information. */
    };

    /**
     * @brief Represents a scene in GLTF.
     */
    struct Scene {
        Scene(): gltfType(GltfType::ASCII) {}
        std::string name;                   /**< Name of the scene. */
        /**
         * @brief Indices of nodes in the scene.
         */
        std::vector<int> nodes;
        GltfType gltfType;                  /**< GLTF type (ASCII or BINARY). */
    };
}