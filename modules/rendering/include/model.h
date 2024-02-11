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

#include <GL/glew.h>
#include <string>
#include <vector>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <map>

namespace Mgtt::Rendering {
    // Forward declarations
    struct Scene;
    struct Node;
    struct Mesh;
    struct MeshPrimitive;
    struct Material;
    struct PbrMaterial;
    struct Texture;
    struct NormalTexture;
    struct EmissiveTexture;
    struct MetallicRoughnessTexture;
    struct OcclusionTexture;
    struct BaseColorTexture;

    /**
     * @brief Represents a 3D scene.
     */
    struct Scene {
        /**
         * @brief Constructor for the Scene structure.
         */
        Scene();

        /**
         * @brief Clear releases resources.
         */
        void Clear();
        std::string name;
        std::string path;
        glm::vec3 pos;
        glm::vec3 rot;
        double scale;
        glm::mat4 mvp;
        glm::mat4 matrix;
        std::map<std::string, Mgtt::Rendering::Texture> textureMap; // case in which we want to prevent loading the same texture into RAM which is time consuming
        std::vector<Node> nodes;
        std::vector<Node> linearNodes; 
        std::vector<Material> materials;

    private:
        /**
         * @brief Recursively linearizes the scene hierarchy starting from the given node.
         *
         * This method flattens the hierarchical structure of the scene, storing nodes in a linearNodes vector.
         *
         * @param node The starting node to linearize.
         */
        void Linearize(Mgtt::Rendering::Node& node);
    };

    /**
     * @brief Represents a node in the scene hierarchy.
     */
    struct Node {
        /**
         * @brief Constructor for the Node structure.
         */
        Node();

        /**
         * @brief Clear releases resources.
         */
        void Clear();

        /**
         * @brief Calculates the local transformation matrix of the node.
         * @return Local transformation matrix.
         */
        glm::mat4 LocalMatrix();

        /**
         * @brief Calculates the global transformation matrix of the node.
         * @return Global transformation matrix.
         */
        glm::mat4 GetGlobalMatrix();

        std::string name;
        std::shared_ptr<Node> parent;
        std::shared_ptr<Mesh> mesh;
        std::vector<Node> children;
        glm::vec3 pos;
        glm::quat rot;
        glm::vec3 scale;
        glm::mat4 mvp;
        glm::mat4 matrix;
        uint32_t index;
    };

    /**
     * @brief Represents a mesh in the scene.
     */
    struct Mesh {
        /**
         * @brief Constructor for the Mesh structure.
         */
        Mesh();

        /**
         * @brief Clear releases resources.
         */
        void Clear();

        std::string name;
        std::vector<MeshPrimitive> meshPrimitives;
        std::vector<unsigned int> indices;
        std::vector<glm::vec3> vertexPositionAttribs;
        std::vector<glm::vec3> vertexNormalAttribs;
        std::vector<glm::vec2> vertexTextureAttribs;
        std::vector<glm::ivec4> vertexJointAttribs;
        std::vector<glm::vec4> vertexWeightAttribs;
        glm::mat4 matrix;

        uint32_t vao;   
        uint32_t ebo; // On existing indices
        uint32_t pos; 
        uint32_t normal;
        uint32_t tex;
        //uint32_t joint; 
        //uint32_t weight;
    };

    
    /**
     * @brief Represents a primitive mesh in the scene.
     */
    struct MeshPrimitive {
        /**
         * @brief Constructor for the MeshPrimitive structure.
         */
        MeshPrimitive();

        /**
         * @brief Clear releases resources.
         */
        void Clear();

        std::string name;
        uint32_t firstIndex;  // required for glDrawElements(...)
        uint32_t indexCount;  // required for glDrawElements(...)
        uint32_t vertexCount; // required for glDrawElements(...) or glDrawArrays(...) 
        bool hasSkin;
        bool hasIndices;
        std::shared_ptr<PbrMaterial> pbrMaterial;
    };

    /**
     * @brief Represents a generic material.
     */
    struct Material {
        /**
         * @brief Constructor for the Material structure.
         */
        Material();

        std::string name;
    };

    enum class AlphaMode { NONE, OPAQE, MASK, BLEND };

    /**
     * @brief Represents a physically based rendering (PBR) material.
     */
    struct PbrMaterial : public Material {
        /**
         * @brief Constructor for the PbrMaterial structure.
         */
        PbrMaterial();

        /**
         * @brief Clear releases resources.
         */
        void Clear();

        std::unique_ptr<NormalTexture> normalTexture;
        std::unique_ptr<OcclusionTexture> occlusionTexture;
        std::unique_ptr<EmissiveTexture> emissiveTexture;
        std::unique_ptr<BaseColorTexture> baseColorTexture;
        std::unique_ptr<MetallicRoughnessTexture> metallicRoughnessTexture;

        float alphaCutoff;
        bool doubleSided;
        AlphaMode alphaMode;
    };

    /**
     * @brief Represents a generic texture.
     */
    struct Texture {
        /**
        * @brief Constructor for the Texture structure
        */
        Texture();

        /**
         * @brief Clear releases resources.
         *
         * This method clears the resources associated with the Texture object, freeing up memory.
         * It is recommended to call this method when the Texture is no longer needed.
         */
        void Clear();

        std::string name;
        uint32_t id;
        std::string path;
        int32_t width;
        int32_t height;
        int32_t nrComponents;
        unsigned char *data;
        uint32_t sizeInBytes;
    };

    /**
     * @brief Represents a normal map texture.
     */
    struct NormalTexture : public Texture {
        /**
         * @brief Constructor for the NormalTexture structure.
         */
        NormalTexture();
        float scale;
    };

    /**
     * @brief Represents an emissive map texture.
     */
    struct EmissiveTexture : public Texture {
        /**
         * @brief Constructor for the EmissiveTexture structure.
         */
        EmissiveTexture();
        glm::vec3 color;
    };

    /**
     * @brief Represents a metallic and roughness map texture.
     */
    struct MetallicRoughnessTexture : public Texture {
        /**
         * @brief Constructor for the MetallicRoughnessTexture structure.
         */
        MetallicRoughnessTexture();
        float metallicFactor;
        float roughnessFactor;
    };

    /**
     * @brief Represents an occlusion map texture.
     */
    struct OcclusionTexture : public Texture {
        /**
         * @brief Constructor for the OcclusionTexture structure.
         */
        OcclusionTexture();
        glm::vec3 color;
    };

    /**
     * @brief Represents a base color map texture.
     */
    struct BaseColorTexture : public Texture {
        /**
         * @brief Constructor for the BaseColorTexture structure.
         */
        BaseColorTexture();
        glm::vec4 color;
    };
}