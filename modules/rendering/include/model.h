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
#include <opengl-shader.h>

namespace Mgtt::Rendering {
    // Forward declarations
    struct Scene;
    struct Node;
    struct Mesh;
    struct MeshPrimitive;
    struct Material;
    struct TextureBase;
    struct Texture;
    struct NormalTexture;
    struct EmissiveTexture;
    struct MetallicRoughnessTexture;
    struct OcclusionTexture;
    struct BaseColorTexture;
    struct PbrMaterial;
    struct AABB;
    struct RenderTexturesContainer;
    struct HdrTextureContainer;
    struct CubeMapContainer;

    /**
     * @brief Represents a collidable aabb
     */
    struct AABB {
        /**
         * @brief Constructor for the AABB structure.
         */
        AABB();
        glm::vec3 min;
        glm::vec3 max;
        glm::vec3 center;

        /**
         * @brief CalculateBoundingBox calculates the bounding box of an object transformed by the given matrix.
         *
         * This function takes a 4x4 transformation matrix and calculates the bounding box
         * of an object after being transformed by the matrix. The resulting bounding box
         * can be used for various purposes, such as collision detection or rendering optimizations.
         *
         * @param m A 4x4 transformation matrix representing the object's transformation.
         *
         * @return void This function does not return a value. The bounding box information is typically
         *              stored or used internally within the calling code.
         *
         * @note The function assumes that the object's original bounding box is defined in its local space.
         *       The resulting bounding box is in the same coordinate space as the transformed object.
         *
         * @see glm::mat4 - The type of the transformation matrix.
         */
        void CalculateBoundingBox(const glm::mat4& m);
    };

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
        std::vector< std::shared_ptr<Node>> nodes;
        std::vector< std::shared_ptr<Node>> linearNodes;
        std::vector<PbrMaterial> materials;
        AABB aabb;
        OpenGlShader shader;
    private:
        /**
         * @brief Recursively linearizes the scene hierarchy starting from the given node.
         *
         * This method flattens the hierarchical structure of the scene, storing nodes in a linearNodes vector.
         *
         * @param node The starting node to linearize.
         */
        void Linearize(std::shared_ptr<Mgtt::Rendering::Node> node);
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
        std::vector<std::shared_ptr<Node>> children;
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
        uint32_t ebo; 
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
        AABB aabb;
    };

    /**
     * @brief Represents a generic texture base.
     */
    struct TextureBase {
        /**
        * @brief Constructor for the TextureBase structure
        */
        TextureBase();
        
        std::string name;
        std::string path;
        int32_t width;
        int32_t height;
        int32_t nrComponents;
        unsigned char* data;
        uint32_t sizeInBytes;
    };

    /**
     * @brief Represents a generic texture.
     */
    struct Texture: public TextureBase {
        /**
        * @brief Constructor for the Texture structure
        */
        Texture();

        /**
        * @brief Copy Constructor for the Texture structure
        *
        * @param texture The texture to be associated with this structure.
        */
        Texture(const Texture& texture);

        /**
         * @brief Clear releases resources.
         *
         * This method clears the resources associated with the Texture object, freeing up memory.
         * It is recommended to call this method when the Texture is no longer needed.
         */
        void Clear();

        uint32_t id;
    };

    /**
     * @brief Represents a normal map texture.
     */
    struct NormalTexture : public Texture {
        /**
         * @brief Constructor for the NormalTexture structure.
         */
        NormalTexture();

        /**
        * @brief Constructor for the NormalTexture structure.
        *
        * @param texture The normal texture to be associated with this structure.
        * @param scale The scale factor applied to the normal texture.
        */
        NormalTexture(const Texture& texture, const float& scale);

        float scale;
    };

    /**
     * @brief Represents an emissive map texture.
     */
    struct EmissiveTexture : public Texture {
        /**
         * @brief Constructor for the Emissive Texture structure.
         */
        EmissiveTexture();

        /**
        * @brief Constructor for the Emissive Texture structure.
        *
        * @param texture The emissive texture to be associated with this structure.
        * @param scale The emissive color applied to the normal texture.
        */
        EmissiveTexture(const Texture& texture, const glm::vec3& color);

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

        /**
        * @brief Constructor for the Metallic roughness texture.
        *
        * @param texture The Metallic roughness texture to be associated with this structure.
        * @param metallicFactor The metallic factor applied to the Metallic roughness texture.
        * @param roughnnessFactor The roughness factor applied to the Metallic roughness texture.
        */
        MetallicRoughnessTexture(const Texture& texture, const float& metallicFactor, const float& roughnessFactor);

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

        /**
        * @brief Constructor for the Occlusion Texture structure.
        *
        * @param texture The occlusion texture to be associated with this structure.
        * @param scale The occlusion color applied to the occlusion texture.
        */
        OcclusionTexture(const Texture& texture, const float& color);

        float strength;
    };

    /**
     * @brief Represents a base color map texture.
     */
    struct BaseColorTexture : public Texture {
        /**
         * @brief Constructor for the BaseColorTexture structure.
         */
        BaseColorTexture();

        /**
        * @brief Constructor for the BaseColor Texture structure.
        *
        * @param texture The base color texture to be associated with this structure.
        * @param scale The base color applied to the base color texture.
        */
        BaseColorTexture(const Texture& texture, const glm::vec4& color);

        glm::vec4 color;
    };

    /**
     * @brief Represents a generic material.
     */
    struct Material {
        /**
         * @brief Constructor for the Material structure.
         */
        Material();

        /**
         * @brief Virtual destructor for the Material structure.
         */
        virtual ~Material() {}

        std::string name;
    };

    enum class AlphaMode { NONE, OPAQ, MASK, BLEND };

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

        std::string name;
        NormalTexture normalTexture;
        OcclusionTexture occlusionTexture;
        EmissiveTexture emissiveTexture;
        BaseColorTexture baseColorTexture;
        MetallicRoughnessTexture metallicRoughnessTexture;

        float alphaCutoff;
        bool doubleSided;
        AlphaMode alphaMode;
    };

    /**
     * @brief Container for managing rendering-related textures and resources.
     * 
     * The RenderTexturesContainer structure encapsulates various texture IDs and rendering
     * resources used in a graphics rendering pipeline.
     */
    struct RenderTexturesContainer {
        /**
         * @brief Constructor for the RenderTexturesContainer structure.
         */
        RenderTexturesContainer();

        /**
         * @brief Constructor for the RenderTexturesContainer structure.
         * 
         * @param eq2CubeMapShaderPathes The equirectangular to cube map vertex and fragment shader path
         * @param eq2CubeMapShaderPathes The brdf lut vertex and fragment shader path
         * @param envMapShaderPathes The env map vertex and fragment shader path
         */
        RenderTexturesContainer(
            const std::pair<std::string, std::string>& eq2CubeMapShaderPathes, 
            const std::pair<std::string, std::string>& brdfLutShaderPathes,
            const std::pair<std::string, std::string>& envMapShaderPathes);

        /**
         * @brief Clear releases resources.
         *
         * This method clears the resources associated with the RenderTextureContainer object, freeing up memory.
         * It is recommended to call this method when the RenderTextureContainer is no longer needed.
         */
        void Clear();

        uint32_t cubeMapTextureId;      
        uint32_t irradianceMapTextureId; 
        uint32_t brdfLutTextureId;       
        uint32_t hdrTextureId;          
        uint32_t fboId;                  
        uint32_t rboId;                  
        uint32_t envMapVao;              
        uint32_t envMapVbo;              
        uint32_t brdfQuadVao;            
        uint32_t brdfQuadVbo;            
        std::vector<TextureBase> textures; // @note This vector can be used to store either a single HDR texture or multiple cube map textures.

        Mgtt::Rendering::OpenGlShader eq2CubeMapShader; // equirectangular to cubemap shader
        Mgtt::Rendering::OpenGlShader brdfLutShader; // brdf lut shader
        Mgtt::Rendering::OpenGlShader envMapShader; // env map shader
    };
}