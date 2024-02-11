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
#include "iscene-importer.h"  
#include <string>
#include <stdexcept>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT
#include <tiny_gltf.h>

#define GLTF_COMPONENT_TYPE_BYTE (5120)
#define GLTF_COMPONENT_TYPE_UNSIGNED_BYTE (5121)
#define GLTF_COMPONENT_TYPE_SHORT (5122)
#define GLTF_COMPONENT_TYPE_UNSIGNED_SHORT (5123)
#define GLTF_COMPONENT_TYPE_INT (5124)
#define GLTF_COMPONENT_TYPE_UNSIGNED_INT (5125)
#define GLTF_COMPONENT_TYPE_FLOAT (5126)
#define GLTF_COMPONENT_TYPE_DOUBLE (5130)

#define GLTF_PARAMETER_TYPE_BYTE (5120)
#define GLTF_PARAMETER_TYPE_UNSIGNED_BYTE (5121)
#define GLTF_PARAMETER_TYPE_SHORT (5122)
#define GLTF_PARAMETER_TYPE_UNSIGNED_SHORT (5123)
#define GLTF_PARAMETER_TYPE_INT (5124)
#define GLTF_PARAMETER_TYPE_UNSIGNED_INT (5125)
#define GLTF_PARAMETER_TYPE_FLOAT (5126)

#define GLTF_TYPE_VEC2 (2)
#define GLTF_TYPE_VEC3 (3)
#define GLTF_TYPE_VEC4 (4)
#define GLTF_TYPE_MAT2 (32 + 2)
#define GLTF_TYPE_MAT3 (32 + 3)
#define GLTF_TYPE_MAT4 (32 + 4)
#define GLTF_TYPE_SCALAR (64 + 1)
#define GLTF_TYPE_VECTOR (64 + 4)
#define GLTF_TYPE_MATRIX (64 + 16)

namespace Mgtt::Rendering {
    /**
     * @brief Implementation of the ISceneImporter interface for importing 3D scenes.
     * 
     * This class provides concrete implementation details for loading and clearing 3D scenes.
     */
    class GltfSceneImporter : public ISceneImporter {
    public:
        /**
         * @brief Default constructor for the scene importer.
         */
        GltfSceneImporter() {}

        /**
         * @brief Load the 3D scene from a specified file path.
         * 
         * This method overrides the corresponding method in the ISceneImporter interface.
         * It loads the 3D scene from the specified file path.
         * 
         * @param path The file path from which to load the 3D scene.
         * @return An instance of the loaded 3D scene.
         */
        Mgtt::Rendering::Scene& Load(const std::string& path) override;

        /**
         * @brief Clear the allocated resources in RAM and VRAM for the scene object.
         * 
         * This method is used to reset the internal state of the scene.
         * @param scene A unique pointer to the scene to clear.
         */
        void Clear(Mgtt::Rendering::Scene& scene) override;

    private:
        /**
         * @brief Load texture from the provided glTF model.
         *
         * This method loads texture from the given glTF model and updates the
         * internal representation of the 3D scene accordingly.
         *
         * @param scene A reference to the updated 3D scene after loading nodes.
         * @param gltfModel The glTF model containing node information.
         */
        void LoadTextures(Mgtt::Rendering::Scene& scene, tinygltf::Model& gltfModel);

        /**
         * @brief Load materials from the provided glTF model.
         *
         * This method loads materials from the given glTF model and updates the
         * internal representation of the 3D scene accordingly.
         *
         * @param scene A reference to the updated 3D scene after loading nodes.
         * @param gltfModel The glTF model containing node information.
         */
        void LoadMaterials(Mgtt::Rendering::Scene& scene, tinygltf::Model& gltfModel);

        /**
         * @brief Load nodes from the provided glTF model.
         *
         * This method loads nodes from the given glTF model and updates the
         * internal representation of the 3D scene accordingly.
         *
         * @param parent A shared pointer to the parent node in the 3D scene.
         * @param scene Reference to the updated 3D scene after loading nodes.
         * @param node Reference to the glTF node containing information.
         * @param nodeIndex Index of the current node in the glTF model.
         * @param model Reference to the glTF model containing node information.
         */
        void LoadNode(
            std::shared_ptr<Mgtt::Rendering::Node> parent, Mgtt::Rendering::Scene &scene,
            const tinygltf::Node &node, const uint32_t nodeIndex, const tinygltf::Model &model);

        /**
         * @brief Clear the resources associated with the Texture object, freeing up memory.
         *
         * This method releases resources associated with the provided Texture object, essentially freeing up memory.
         * It is recommended to call this method when the Texture is no longer needed.
         *
         * @param texture Reference to the Texture object whose resources are to be cleared.
         */
        void Clear(Mgtt::Rendering::Texture& texture);

        /**
         * @brief Load a texture from the specified file path and update the Texture object.
         *
         * This method loads a texture from the given file path and updates the provided Texture object with the loaded data.
         *
         * @param texturePath The file path to the texture.
         * @param texture Reference to the Texture object to be updated with the loaded data.
         */
        void Load(const std::string& texturePath, Mgtt::Rendering::Texture& texture);

        /**
         * @brief Clear the RAM resources associated with the Texture object.
         *
         * This method releases the memory resources in RAM associated with the Texture object, freeing up memory.
         * It is recommended to call this method when the Texture is no longer needed to avoid unnecessary memory usage.
         *
         * @param texture Reference to the Texture object for which RAM resources should be cleared.
         */
        void ClearRAM(Mgtt::Rendering::Texture& texture);
    };
}