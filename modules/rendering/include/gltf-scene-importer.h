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
#include <tiny_gltf.h>

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
    };
}