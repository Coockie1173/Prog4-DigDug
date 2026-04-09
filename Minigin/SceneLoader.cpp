#include "SceneLoader.h"
#include "GameObject.h"
#include "ResourceManager.h"
#include "TextObject.h"
#include "Scene.h"
#include "Debugger.h"

#include <SceneDeserializer.h>
#include <SceneData.h>
#include <ComponentTypeMap.h>

// Component includes
#include "Components/TextureRendererComponent.h"
#include "Components/TextRendererComponent.h"
#include "Components/FPSCounterComponent.h"
#include "Components/SpinnerComponent.h"
#include "Components/ScoreComponent.h"
#include "Components/ObjectMoveComponent.h"

#include <format>
#include <functional>
#include <map>
#include <sstream>
#include <glm/glm.hpp>

namespace dae
{
    // Anonymous namespace provides internal linkage for g_componentFactories.
    // This prevents ODR violations and signals the map is private implementation detail.
    // Doing it like this also just allows us to plop standard data in before calling deserialize
    namespace
    {
        using ComponentFactory = std::function<Component* (GameObject*)>;

        static const std::map<uint32_t, ComponentFactory> g_componentFactories{
            {
                HASH_TextureRenderComponent,
                [](GameObject* obj) { return obj->AddComponent<TextureRenderComponent>(""); }
            },
            {
                HASH_TextRenderComponent,
                [](GameObject* obj)
                {
                    auto font = ResourceManager::GetInstance().LoadFont("Lingua.otf", 24);
                    return obj->AddComponent<TextRenderComponent>("", SDL_Color{255, 255, 255, 255}, font);
                }
            },
            {
                HASH_FPSCounterComponent,
                [](GameObject* obj) { return obj->AddComponent<FPSCounterComponent>(); }
            },
            {
                HASH_SpinnerComponent,
                [](GameObject* obj) { return obj->AddComponent<SpinnerComponent>(true, 50.0f, 1.0f, glm::vec2(400.0f, 300.0f)); }
            },
            {
                HASH_ScoreComponent,
                [](GameObject* obj) { return obj->AddComponent<ScoreComponent>(); }
            },
            {
                HASH_ObjectMoveComponent,
                [](GameObject* obj) { return obj->AddComponent<ObjectMoveComponent>(); }
            }
        };
    }

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
            if (idToGameObjectMap.contains(parentId))
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
            if (!idToGameObjectMap.contains(objData.id))
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

    bool SceneLoader::CreateAndAddComponent(GameObject* gameObject, uint32_t componentTypeHash, const std::string&, const std::map<std::string, std::string>& properties, std::string& errorMessage)
    {
        if (!gameObject)
        {
            errorMessage = "GameObject is null";
            Debugger::GetInstance().LogError(errorMessage);
            return false;
        }

        Debugger::GetInstance().LogDebug(std::format(
            "[SceneLoader] CreateAndAddComponent hash={}, properties count={}",
            componentTypeHash, properties.size()
        ));
        for (const auto& [key, value] : properties)
        {
            Debugger::GetInstance().LogDebug(std::format(
                "    Property: {} = {}", key, value
            ));
        }

        auto factoryIt = g_componentFactories.find(componentTypeHash);
        if (factoryIt == g_componentFactories.end())
        {
            errorMessage = std::format("Unknown component type hash: 0x{:X}", componentTypeHash);
            Debugger::GetInstance().LogWarning(errorMessage);
            return true;
        }

        Component* component = factoryIt->second(gameObject);
        if (!component)
        {
            errorMessage = "Failed to create component";
            Debugger::GetInstance().LogError(errorMessage);
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
