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

    struct Material {
        Material();
        std::string name;
    };

    struct PbrMaterial : public Material {
        PbrMaterial();
        struct NormalTexture;
        struct OcclusionTexture;
        struct EmissiveTexture;
        struct BaseColorTexture;
        struct MetallicRoughnessTexture;
    };

    struct Texture {
        Texture();
        std::string name;
    };

    struct NormalTexture : public Texture {
        NormalTexture();
        float scale;
    };

    struct EmissiveTexture : public Texture {
        EmissiveTexture();
        float color;
    };

    struct MetallicRoughnessTexture : public Texture {
        MetallicRoughnessTexture();
        float metallicFactor;
        float roughnessFactor;
    };

    struct OcclusionTexture : public Texture {
        OcclusionTexture();
        glm::vec3 color;
    };

    struct BaseColorTexture : public Texture {
        BaseColorTexture();
        glm::vec4 color;
    };
}  // namespace Mgtt::Rendering
