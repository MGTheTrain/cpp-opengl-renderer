#include <gltf-scene-importer.h>


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
    scene.~Scene();
}