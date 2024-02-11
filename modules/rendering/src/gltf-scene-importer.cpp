#include <gltf-scene-importer.h>

/**
 * @brief Default constructor for the scene importer.
 */
Mgtt::Rendering::GltfSceneImporter::GltfSceneImporter() {
    
}

/**
 * @brief Destructor for the scene importer.
 */
Mgtt::Rendering::GltfSceneImporter::~GltfSceneImporter() {

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
std::unique_ptr<Mgtt::Rendering::Scene> Mgtt::Rendering::GltfSceneImporter::Load(const std::string& path) {

}

/**
 * @brief Clear the current state of the scene importer.
 * 
 * This method overrides the corresponding method in the ISceneImporter interface.
 * It is used to reset the internal state of the scene importer, freeing resources.
 */
void Mgtt::Rendering::GltfSceneImporter::Clear() {

}