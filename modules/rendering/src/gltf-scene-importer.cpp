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
    Mgtt::Rendering::Scene mgttScene;
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
    bool fileLoaded = binary ? gltfContext.LoadBinaryFromFile(&gltfModel, &err, &warn, path.c_str()) : gltfContext.LoadASCIIFromFile(&gltfModel, &err, &warn, path.c_str());

    size_t vertexCount = 0;
    size_t indexCount = 0;
    if (fileLoaded) {
        this->LoadTextures(scene, gltfModel);
        this->LoadMaterials(scene, gltfModel);
        const tinygltf::Scene& scene = gltfModel.scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];
        for (size_t i = 0; i < scene.nodes.size(); i++) {
            const tinygltf::Node node = gltfModel.nodes[scene.nodes[i]];
            LoadNode(nullptr, mgttScene, node, scene.nodes[i], gltfModel);
        }
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
 * @brief Load texture from the provided glTF model.
 *
 * This method loads texture from the given glTF model and updates the
 * internal representation of the 3D scene accordingly.
 *
 * @param scene A reference to the updated 3D scene after loading nodes.
 * @param gltfModel The glTF model containing node information.
 */
void Mgtt::Rendering::GltfSceneImporter::LoadTextures(Mgtt::Rendering::Scene& scene, tinygltf::Model& gltfModel) {
    
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
 * @param parent A shared pointer to the parent node in the 3D scene.
 * @param scene Reference to the updated 3D scene after loading nodes.
 * @param node Reference to the glTF node containing information.
 * @param nodeIndex Index of the current node in the glTF model.
 * @param model Reference to the glTF model containing node information.
 */
void Mgtt::Rendering::GltfSceneImporter::LoadNode(
    std::shared_ptr<Mgtt::Rendering::Node> parent, Mgtt::Rendering::Scene &scene,
    const tinygltf::Node &node, const uint32_t nodeIndex, const tinygltf::Model &model) {
    std::shared_ptr<Mgtt::Rendering::Node> newNode =
      std::make_shared<Mgtt::Rendering::Node>();
    newNode->index = nodeIndex;
    newNode->parent = parent;
    newNode->name = node.name;

    glm::vec3 translation = (node.translation.size() == 3) ? glm::make_vec3(node.translation.data()) : glm::vec3(0.0f);
    newNode->pos = translation;

    glm::mat4 rotation = glm::mat4(1.0f);
    if (node.rotation.size() == 4) {
        glm::quat q = glm::make_quat(node.rotation.data());
        newNode->rot = glm::mat4(q);
    }
    glm::vec3 scale = (node.scale.size() == 3) ? glm::make_vec3(node.scale.data()) : glm::vec3(1.0f);
    newNode->scale = scale;

    if (node.matrix.size() == 16) {
        newNode->matrix = glm::make_mat4x4(node.matrix.data());
    }

    if (node.children.size() > 0) {
        for (uint32_t i = 0; i < node.children.size(); i++) {
        LoadNode(newNode, scene, model.nodes[node.children[i]], node.children[i], model);
        }
    }

    if (node.mesh > -1) {
        // assign mesh attributes
    }
}