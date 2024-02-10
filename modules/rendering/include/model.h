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

#include <string>
#include <vector>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Mgtt::Rendering {
    // Forward declarations
    struct Mesh;
    struct Material;

    /**
     * @brief Represents a 3D scene.
     */
    struct Scene {
        /**
         * @brief @brief Constructor for the Scene structure.
         */
        Scene();
        std::string name;
        std::string path;
        glm::vec3 pos;
        glm::vec3 rot;
        double scale;
        glm::mat4 mvp;
        glm::mat4 matrix;
    };

    /**
     * @brief Represents a node in the scene hierarchy.
     */
    struct Node {
        /**
         * @brief @brief Constructor for the Node structure.
         */
        Node();

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
        std::vector<std::shared_ptr<Node>> children;
        glm::vec3 pos;
        glm::quat rot;
        glm::vec3 scale;
        glm::mat4 mvp;
        glm::mat4 matrix;
        uint32_t index;
    };

    /**
     * @brief Represents a primitive mesh in the scene.
     */
    struct MeshPrimitive {
        /**
         * @brief @brief Constructor for the MeshPrimitive structure.
         */
        MeshPrimitive();
        std::string name;
        uint32_t firstIndex;  // required for glDrawElements(...)
        uint32_t indexCount;  // required for glDrawElements(...)
        uint32_t vertexCount; // required for glDrawElements(...) or glDrawArrays(...) 
        bool hasSkin;
        bool hasIndices;
        std::shared_ptr<Material> material;
    };

    /**
     * @brief Represents a mesh in the scene.
     */
    struct Mesh {
        /**
         * @brief @brief Constructor for the Mesh structure.
         */
        Mesh();
        std::string name;
        std::vector<MeshPrimitive> meshPrimitives;
        std::vector<unsigned int> indices;
        std::vector<glm::vec3> vertexPositionAttribs;
        std::vector<glm::vec3> vertexNormalAttribs;
        std::vector<glm::vec2> vertexTextureAttribs;
        std::vector<glm::ivec4> vertexJointAttribs;
        std::vector<glm::vec4> vertexWeightAttribs;
        glm::mat4 matrix;
    };

    /**
     * @brief Represents a generic material.
     */
    struct Material {
        /**
         * @brief @brief Constructor for the Material structure.
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
         * @brief @brief Constructor for the PbrMaterial structure.
         */
        PbrMaterial();
        struct NormalTexture;
        struct OcclusionTexture;
        struct EmissiveTexture;
        struct BaseColorTexture;
        struct MetallicRoughnessTexture;

        float alphaCutoff;
        bool doubleSided;
        AlphaMode alphaMode;
    };

    /**
     * @brief Represents a generic texture.
     */
    struct Texture {
        /**
         * @brief @brief Constructor for the Texture structure.
         */
        Texture();
        std::string name;
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
         * @brief @brief Constructor for the NormalTexture structure.
         */
        NormalTexture();
        float scale;
    };

    /**
     * @brief Represents an emissive map texture.
     */
    struct EmissiveTexture : public Texture {
        /**
         * @brief @brief Constructor for the EmissiveTexture structure.
         */
        EmissiveTexture();
        float color;
    };

    /**
     * @brief Represents a metallic and roughness map texture.
     */
    struct MetallicRoughnessTexture : public Texture {#
        /**
         * @brief @brief Constructor for the MetallicRoughnessTexture structure.
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
         * @brief @brief Constructor for the OcclusionTexture structure.
         */
        OcclusionTexture();
        glm::vec3 color;
    };

    /**
     * @brief Represents a base color map texture.
     */
    struct BaseColorTexture : public Texture {
        /**
         * @brief @brief Constructor for the BaseColorTexture structure.
         */
        BaseColorTexture();
        glm::vec4 color;
    };
}