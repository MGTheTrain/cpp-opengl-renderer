#include <gltf-scene-importer.h>


/**
 * @brief Load the 3D scene from a specified file path.
 * 
 * This method overrides the corresponding method in the ISceneImporter interface.
 * It loads the 3D scene from the specified file path.
 * 
 * @param path The file path from which to load the 3D scene.
 * @return An instance of the loaded 3D scene.
 * 
 * @note This implementation is inspired by the Vulkan glTF PBR example:
 * https://github.com/SaschaWillems/Vulkan-glTF-PBR/blob/master/base/VulkanglTFModel.cpp
 */
Mgtt::Rendering::Scene& Mgtt::Rendering::GltfSceneImporter::Load(const std::string& path) {
    Mgtt::Rendering::Scene scene;
    scene.path = path;

    bool hasGltfSuffix = (path.substr(path.size() - 5, 5) == ".GLTF" ||
                            path.substr(path.size() - 5, 5) == ".gltf" ||
                            path.substr(path.size() - 4, 4) == ".GLB" ||
                            path.substr(path.size() - 4, 4) == ".glb");

    if (!hasGltfSuffix) {
        throw std::runtime_error("GLTF IMPORTER ERROR: No proper suffix for: " + path);
    }

    bool binary = false;
    size_t suffix = path.rfind('.', path.size());
    if (suffix != std::string::npos) {
        binary = (path.substr(suffix + 1, path.length() - suffix) == "glb");
    }

    std::string err;
    std::string warn;
    tinygltf::Model gltfModel;
    tinygltf::TinyGLTF gltfContext;
    bool fileLoaded = binary ? gltfContext.LoadBinaryFromFile(&gltfModel, &err, &warn, path.c_str()) : gltfContext.LoadASCIIFromFile(&gltfModel, &error, &warning, path.c_str());


    size_t vertexCount = 0;
    size_t indexCount = 0;
    if (fileLoaded) {
        this->LoadTextures(scene, gltfModel);
        this->LoadMaterials(scene, gltfModel);
        this->LoadNodes(scene, gltfModel);

        // // assign values from gltfModel
        // Mgtt::Rendering::Node node;
        // scene.nodes.push_back(node);
    }
    else {
        throw std::runtime_error("GLTF IMPORTER ERROR: Could not load file: " + path);
    }

    return scene;
}

/**
 * @brief Clear the allocated resources in RAM and VRAM for the scene object.
 * 
 * This method is used to reset the internal state of the scene.
 * @param scene A unique pointer to the scene to clear.
 */
void Mgtt::Rendering::GltfSceneImporter::Clear(Mgtt::Rendering::Scene& scene) {
    scene.Clear();
}

/**
 * @brief Load materials from the provided glTF model.
 *
 * This method loads materials from the given glTF model and updates the
 * internal representation of the 3D scene accordingly.
 *
 * @param scene A reference to the updated 3D scene after loading nodes.
 * @param gltfModel The glTF model containing node information.
 */
void Mgtt::Rendering::GltfSceneImporter::LoadMaterials(Mgtt::Rendering::Scene& scene, tinygltf::Model& gltfModel) {

}

/**
 * @brief Load nodes from the provided glTF model.
 *
 * This method loads nodes from the given glTF model and updates the
 * internal representation of the 3D scene accordingly.
 *
 * @param scene A reference to the updated 3D scene after loading nodes.
 * @param gltfModel The glTF model containing node information.
 */
void Mgtt::Rendering::GltfSceneImporter::LoadNodes(Mgtt::Rendering::Scene& scene, tinygltf::Model& gltfModel) {

}