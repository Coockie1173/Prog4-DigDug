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
#include <map>
#include <cstdio>
#include <glm/glm.hpp>

namespace dae
{
    bool SceneLoader::LoadSceneFromFile(
        const std::string& sceneFilePath,
        Scene& scene,
        std::string& errorMessage
    )
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

        //First pass: create all game objects with hierarchy
        for (const auto& objData : sceneData.gameObjects)
        {
            //Find parent if exists
            GameObject* parent = nullptr;
            if (objData.parentId != -1 && idToGameObjectMap.contains(objData.parentId))
            {
                parent = idToGameObjectMap[objData.parentId];
            }

            //Create game object
            auto gameObject = std::make_unique<GameObject>(
                objData.name,
                objData.worldPosition,
                parent
            );

            //Set debug flag if needed
            if (objData.isDebug)
            {
                //little redundant, may implement properly later
            }

            //Store raw pointer before transferring ownership
            GameObject* gameObjectPtr = gameObject.get();
            idToGameObjectMap[objData.id] = gameObjectPtr;

            //Add to scene
            scene.Add(std::move(gameObject));
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

        return true;
    }

    bool SceneLoader::CreateAndAddComponent(
        GameObject* gameObject,
        uint32_t componentTypeHash,
        const std::string&,
        const std::map<std::string, std::string>& properties,
        std::string& errorMessage
    )
    {
        if (!gameObject)
        {
            errorMessage = "GameObject is null";
            Debugger::GetInstance().LogError(errorMessage);
            return false;
        }

        //Log component creation and properties
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

        //Map component types to factory functions using hash
        switch (componentTypeHash)
        {
            case HASH_TextureRenderComponent:
            {
                //Get texture name property
                auto textureNameIt = properties.find("textureName");
                if (textureNameIt != properties.end())
                {
                    gameObject->AddComponent<TextureRenderComponent>(textureNameIt->second);
                    return true;
                }
                else
                {
                    errorMessage = std::format(
                        "TextureRenderComponent missing '{}' property",
                        "textureName"
                    );
                    Debugger::GetInstance().LogError(errorMessage);
                    return false;
                }
            }
            
            case HASH_TextRenderComponent:
            {
                //Get text, fontSize, and color properties
                auto textIt = properties.find("text");
                auto fontSizeIt = properties.find("fontSize");
                auto colorIt = properties.find("color");

                if (textIt != properties.end() && fontSizeIt != properties.end())
                {
                    try
                    {
                        auto fontSize = static_cast<uint8_t>(std::stoi(fontSizeIt->second));
                        auto font = ResourceManager::GetInstance().LoadFont("Lingua.otf", fontSize);
                        
                        //Parse color as "r,g,b,a" format (default: white)
                        uint8_t r = 255, g = 255, b = 255, a = 255;
                        if (colorIt != properties.end() && !colorIt->second.empty())
                        {
                            int parsed = sscanf_s(colorIt->second.c_str(), "%hhu,%hhu,%hhu,%hhu", &r, &g, &b, &a);
                            if (parsed < 3)
                            {
                                Debugger::GetInstance().LogDebug(std::format(
                                    "Invalid color format '{}', using white", 
                                    colorIt->second
                                ));
                            }
                        }
                        else
                            Debugger::GetInstance().LogDebug("color empty, using white default");
                        
                        SDL_Color color{ r, g, b, a };

                        gameObject->AddComponent<TextRenderComponent>(textIt->second, color, font);
                        return true;
                    }
                    catch (const std::exception& e)
                    {
                        errorMessage = std::format(
                            "Failed to create TextRenderComponent: {}",
                            e.what()
                        );
                        Debugger::GetInstance().LogError(errorMessage);
                        return false;
                    }
                }
                else
                {
                    errorMessage = std::format(
                        "TextRenderComponent missing required properties (text, fontSize)"
                    );
                    Debugger::GetInstance().LogError(errorMessage);
                    return false;
                }
            }
            
            case HASH_FPSCounterComponent:
            {
                gameObject->AddComponent<FPSCounterComponent>();
                return true;
            }
            
            case HASH_SpinnerComponent:
            {
                //Read spinner properties
                auto clockwiseIt = properties.find("spinClockwise");
                auto radiusIt = properties.find("spinRadius");
                auto speedIt = properties.find("spinSpeed");
                auto pointIt = properties.find("spinPoint");

                if (clockwiseIt != properties.end() && radiusIt != properties.end() && 
                    speedIt != properties.end() && pointIt != properties.end())
                {
                    try
                    {
                        bool clockwise = false;
                        float radius = 0.0f;
                        float speed = 0.0f;
                        glm::vec2 point(0.0f, 0.0f);

                        //Parse spinClockwise (default: true)
                        if (!clockwiseIt->second.empty())
                            clockwise = (clockwiseIt->second == "true" || clockwiseIt->second == "1");
                        else
                        {
                            clockwise = true;
                            Debugger::GetInstance().LogDebug("spinClockwise empty, using default (true)");
                        }

                        //Parse spinRadius (default: 50.0f)
                        if (!radiusIt->second.empty())
                            radius = std::stof(radiusIt->second);
                        else
                        {
                            radius = 50.0f;
                            Debugger::GetInstance().LogDebug("spinRadius empty, using default (50.0f)");
                        }
                        
                        //Parse spinSpeed (default: 1.0f)
                        if (!speedIt->second.empty())
                            speed = std::stof(speedIt->second);
                        else
                        {
                            speed = 1.0f;
                            Debugger::GetInstance().LogDebug("spinSpeed empty, using default (1.0f)");
                        }

                        //Parse spinPoint as "x,y" format (default: 400,300)
                        if (!pointIt->second.empty())
                        {
                            if (sscanf_s(pointIt->second.c_str(), "%f,%f", &point.x, &point.y) != 2)
                            {
                                point = glm::vec2(400.0f, 300.0f);
                                Debugger::GetInstance().LogWarning(std::format(
                                    "Failed to parse spinPoint: {}, using default", pointIt->second
                                ));
                            }
                        }
                        else
                        {
                            point = glm::vec2(400.0f, 300.0f);
                            Debugger::GetInstance().LogDebug("spinPoint empty, using default (400,300)");
                        }

                        gameObject->AddComponent<SpinnerComponent>(clockwise, radius, speed, point);
                        return true;
                    }
                    catch (const std::exception& e)
                    {
                        errorMessage = std::format(
                            "Failed to create SpinnerComponent: {}",
                            e.what()
                        );
                        Debugger::GetInstance().LogError(errorMessage);
                        return false;
                    }
                }
                else
                {
                    errorMessage = std::format(
                        "SpinnerComponent missing required properties"
                    );
                    Debugger::GetInstance().LogError(errorMessage);
                    return false;
                }
            }
            
            case HASH_ScoreComponent:
            {
                gameObject->AddComponent<ScoreComponent>();
                return true;
            }
            
            case HASH_ObjectMoveComponent:
            {
                gameObject->AddComponent<ObjectMoveComponent>();
                return true;
            }
            
            default:
            {
                //Unknown component type
                errorMessage = std::format(
                    "Unknown component type hash: 0x{:X}",
                    componentTypeHash
                );
                Debugger::GetInstance().LogWarning(errorMessage);
                //Return true to allow unknown components to be skipped
                return true;
            }
        }
    }
}
