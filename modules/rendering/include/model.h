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
        Node();
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
        Material();
        std::string name;
    };

    /**
     * @brief Represents a physically based rendering (PBR) material.
     */
    struct PbrMaterial : public Material {
        PbrMaterial();
        struct NormalTexture;
        struct OcclusionTexture;
        struct EmissiveTexture;
        struct BaseColorTexture;
        struct MetallicRoughnessTexture;
    };

    /**
     * @brief Represents a generic texture.
     */
    struct Texture {
        Texture();
        std::string name;
    };

    /**
     * @brief Represents a normal map texture.
     */
    struct NormalTexture : public Texture {
        NormalTexture();
        float scale;
    };

    /**
     * @brief Represents an emissive map texture.
     */
    struct EmissiveTexture : public Texture {
        EmissiveTexture();
        float color;
    };

    /**
     * @brief Represents a metallic and roughness map texture.
     */
    struct MetallicRoughnessTexture : public Texture {
        MetallicRoughnessTexture();
        float metallicFactor;
        float roughnessFactor;
    };

    /**
     * @brief Represents an occlusion map texture.
     */
    struct OcclusionTexture : public Texture {
        OcclusionTexture();
        glm::vec3 color;
    };

    /**
     * @brief Represents a base color map texture.
     */
    struct BaseColorTexture : public Texture {
        BaseColorTexture();
        glm::vec4 color;
    };

}