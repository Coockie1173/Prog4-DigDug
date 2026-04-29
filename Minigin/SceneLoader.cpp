#include "SceneLoader.h"
#include "GameObject.h"
#include "ResourceManager.h"
#include "TextObject.h"
#include "Scene.h"
#include "Debugger.h"
#include "ComponentFactoryRegistry.h"

#include <SceneDeserializer.h>
#include <SceneData.h>
#include "InputManager.h"
#include <format>
#include <sstream>


namespace dae
{
    bool SceneLoader::LoadSceneFromFile(const std::string& sceneFilePath, Scene& scene, std::string& errorMessage)
    {
        //Deserialize the MBIN file
        auto sceneData = SceneDeserializer::DeserializeFromFile(sceneFilePath, errorMessage);
        if (!sceneData)
        {
            errorMessage = std::format(
                "Failed to deserialize scene: {}",
                errorMessage
            );
            Debugger::GetInstance().LogError(errorMessage);
            return false;
        }

        //Add scene data to the game scene
        return AddSceneDataToScene(*sceneData, scene, errorMessage);
    }

    bool SceneLoader::AddSceneDataToScene(const SceneData& sceneData, Scene& scene, std::string& errorMessage)
    {
        //Register input bindings (editor stores action names + device/key). Commands will be attached at runtime by game code.
        for (const auto& [actionName, binding] : sceneData.inputBindings)
        {
            if (binding.deviceType == InputDeviceType::Keyboard)
            {
                dae::InputManager::GetInstance().BindKey(binding.keyCode, dae::InputManager::InputType::Down, binding.actionName);
            }
            else
            {
                dae::InputManager::GetInstance().BindButton(binding.gamepadIndex, binding.gamepadButton, dae::InputManager::InputType::Down, binding.actionName);
            }
        }
        //Map deserialized object IDs to created GameObject pointers
        std::map<uint32_t, GameObject*> idToGameObjectMap;
        //Store created GameObjects before adding to scene
        std::vector<std::unique_ptr<GameObject>> gameObjects;
        //Store parent relationships to apply after all objects are created
        std::vector<std::pair<GameObject*, uint32_t>> parentRelationships;

        //First pass: create all game objects WITHOUT hierarchy
        for (const auto& objData : sceneData.gameObjects)
        {
            //Create game object with no parent initially
            auto gameObject = std::make_unique<GameObject>(
                objData.name,
                objData.worldPosition
            );

            //Set debug flag if needed
            if (objData.isDebug)
            {
                //little redundant, may implement properly later
            }

            //Store raw pointer before transferring ownership
            GameObject* gameObjectPtr = gameObject.get();
            idToGameObjectMap[objData.id] = gameObjectPtr;

            //Store parent relationship for later
            if (objData.parentId != -1)
            {
                parentRelationships.push_back({gameObjectPtr, objData.parentId});
            }

            //Store the unique_ptr for later
            gameObjects.push_back(std::move(gameObject));
        }

        //Establish parent-child relationships
            for (const auto& [child, parentId] : parentRelationships)
            {
                if (idToGameObjectMap.find(parentId) != idToGameObjectMap.end())
            {
                GameObject* parent = idToGameObjectMap[parentId];
                //Calculate local position relative to parent
                glm::vec2 localPosition = child->GetWorldPosition() - parent->GetWorldPosition();
                child->SetLocalPosition(localPosition);
                child->SetParent(parent, false);
            }
        }


        //Second pass: add components to game objects
            for (const auto& objData : sceneData.gameObjects)
            {
                if (idToGameObjectMap.find(objData.id) == idToGameObjectMap.end())
            {
                continue;  //Should not happen, but safety check
            }

            GameObject* gameObject = idToGameObjectMap[objData.id];

            for (const auto& componentData : objData.components)
            {
                if (componentData)
                {
                    if (!CreateAndAddComponent(
                        gameObject,
                        componentData->componentTypeHash,
                        componentData->componentType,
                        componentData->properties,
                        errorMessage
                    ))
                    {
                        errorMessage = std::format(
                            "Failed to create component '{}' of type '{}' for object '{}': {}",
                            componentData->componentName,
                            componentData->componentType,
                            objData.name,
                            errorMessage
                        );
                        Debugger::GetInstance().LogError(errorMessage);
                        return false;
                    }
                }
            }
        }

        //Third pass: add all game objects to the scene
        for (auto& gameObject : gameObjects)
        {
            scene.Add(std::move(gameObject));
        }

        return true;
    }

    bool SceneLoader::CreateAndAddComponent(GameObject* gameObject, uint32_t componentTypeHash, const std::string& componentType, const std::map<std::string, std::string>& properties, std::string& errorMessage)
    {
        if (!gameObject)
        {
            errorMessage = "GameObject is null";
            Debugger::GetInstance().LogError(errorMessage);
            return false;
        }

        auto* component = CreateComponentFromHash(componentTypeHash, gameObject);
        if (!component)
        {
            errorMessage = std::format("Unknown or unregistered component type hash: 0x{:X} ('{}')", componentTypeHash, componentType);
            Debugger::GetInstance().LogWarning(errorMessage);
            return false;
        }

        if (!component->Deserialize(properties, errorMessage))
        {
            errorMessage = std::format("Failed to deserialize component: {}", errorMessage);
            Debugger::GetInstance().LogError(errorMessage);
            return false;
        }

        return true;
    }
}
