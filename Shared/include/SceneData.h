#ifndef _SCENEDATA_H_
#define _SCENEDATA_H_

#include <vector>
#include <map>
#include <memory>
#include <glm/glm.hpp>
#include <ComponentData.h>
#include <InputBindingData.h>

namespace dae
{
    /// <summary>
    /// Deserialized game object data structure
    /// </summary>
    struct GameObjectData
    {
        uint32_t id;
        std::string name;
        int32_t parentId;  // -1 if no parent
        glm::vec2 worldPosition;
        bool isDebug;
        std::vector<std::unique_ptr<ComponentInstance>> components;

        GameObjectData()
            : id(0), parentId(-1), worldPosition(0.0f, 0.0f), isDebug(false)
        {
        }
    };

    /// <summary>
    /// Complete scene data structure deserialized from MBIN file
    /// </summary>
    struct SceneData
    {
        std::map<std::string, InputBinding> inputBindings;
        std::vector<GameObjectData> gameObjects;
    };
}

#endif
