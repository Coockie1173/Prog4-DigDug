#ifndef _SCENELOADER_H_
#define _SCENELOADER_H_

#include <string>
#include <memory>
#include <map>
#include "Scene.h"

namespace dae
{
    struct SceneData;

    /// <summary>
    /// Loads deserialized scene data into the game engine
    /// </summary>
    class SceneLoader
    {
    public:
        /// <summary>
        /// Load a scene from an MBIN file into the provided Scene
        /// </summary>
        /// <param name="sceneFilePath">Path to the MBIN scene file</param>
        /// <param name="scene">The scene to populate with game objects</param>
        /// <param name="errorMessage">Out parameter for error message if loading fails</param>
        /// <returns>True if successful, false otherwise</returns>
        static bool LoadSceneFromFile(const std::string& sceneFilePath, Scene& scene, std::string& errorMessage);

    private:
        SceneLoader() = delete;
        SceneLoader(const SceneLoader&) = delete;
        SceneLoader& operator=(const SceneLoader&) = delete;

        /// <summary>
        /// Add a deserialized scene hierarchy to the game Scene
        /// </summary>
        static bool AddSceneDataToScene(const SceneData& sceneData, Scene& scene, std::string& errorMessage);

        /// <summary>
        /// Create and add a component to a game object based on component type hash
        /// </summary>
        static bool CreateAndAddComponent(GameObject* gameObject, uint32_t componentTypeHash, const std::string& componentType, const std::map<std::string, std::string>& properties, std::string& errorMessage);
    };
}

#endif
