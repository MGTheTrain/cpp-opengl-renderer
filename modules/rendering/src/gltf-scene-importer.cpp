#include <gltf-scene-importer.h>

/**
 * @brief Default constructor for the scene importer.
 */
Mgtt::Rendering::GltfSceneImporter::GltfSceneImporter() {
    
}

/**
 * @brief Recursively linearizes the scene hierarchy starting from the given node.
 *
 * This method flattens the hierarchical structure of the scene, storing nodes in a linearNodes vector.
 *
 * @param node The starting node to linearize.
 * @param scene The scene where the linearized nodes will be stored.
 */
void Mgtt::Rendering::GltfSceneImporter::Linearize(std::shared_ptr<Mgtt::Rendering::Node> node, Mgtt::Rendering::Scene&  scene) {
    scene.linearNodes.push_back(node);

    for (uint32_t i = 0; i < node->children.size(); i++) {
        this->Linearize(node->children[i], scene);
    }
}

/**
 * @brief Load the 3D scene from a specified file path.
 * 
 * This method overrides the corresponding method in the ISceneImporter interface.
 * It loads the 3D scene from the specified file path.
 * 
 * @param path The file path from which to load the 3D scene.
 * @return An instance of the loaded 3D scene.
 */
Mgtt::Rendering::Scene& Mgtt::Rendering::GltfSceneImporter::Load(const std::string& path) {

}

/**
 * @brief Clear the allocated resources in RAM and VRAM for the scene object.
 * 
 * This method is used to reset the internal state of the scene.
 * @param scene A unique pointer to the scene to clear.
 */
void Mgtt::Rendering::GltfSceneImporter::Clear(Mgtt::Rendering::Scene& scene) {
    for(auto & node: scene.nodes) {
        this->Linearize(node, scene);
    }

}