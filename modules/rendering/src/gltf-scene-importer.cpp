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
    size_t extpos = path.rfind('.', path.size());
    if (extpos != std::string::npos) {
        binary = (path.substr(extpos + 1, path.length() - extpos) == "glb");
    }

    std::string error;
    std::string warning;
    tinygltf::Model gltfModel;
    tinygltf::TinyGLTF gltfContext;
    bool fileLoaded = binary ? gltfContext.LoadBinaryFromFile(&gltfModel, &error, &warning, path.c_str()) : gltfContext.LoadASCIIFromFile(&gltfModel, &error, &warning, path.c_str());

    if (fileLoaded) {
        // assign values from gltfModel

        Mgtt::Rendering::Node node;
        scene.nodes.push_back(node);
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