#ifndef _COMPONENT_TYPE_MAP_H_
#define _COMPONENT_TYPE_MAP_H_

#include <Hash.h>
#include <map>
#include <string>

namespace dae
{
    // Component type hashes
    inline constexpr auto HASH_FPSCounterComponent = make_sdbm_hash("FPSCounterComponent");
    inline constexpr auto HASH_ObjectMoveComponent = make_sdbm_hash("ObjectMoveComponent");
    inline constexpr auto HASH_ScoreComponent = make_sdbm_hash("ScoreComponent");
    inline constexpr auto HASH_SpinnerComponent = make_sdbm_hash("SpinnerComponent");
    inline constexpr auto HASH_SwappableRenderComponent = make_sdbm_hash("SwappableRenderComponent");
    inline constexpr auto HASH_TextRenderComponent = make_sdbm_hash("TextRenderComponent");
    inline constexpr auto HASH_TextureRenderComponent = make_sdbm_hash("TextureRenderComponent");
    inline constexpr auto HASH_PlayerControllerComponent = make_sdbm_hash("PlayerControllerComponent");

    // Maps component type hash to string name
    inline const std::map<uint32_t, std::string> COMPONENT_TYPE_MAP{
        { HASH_FPSCounterComponent, "FPSCounterComponent" },
        { HASH_ObjectMoveComponent, "ObjectMoveComponent" },
        { HASH_ScoreComponent, "ScoreComponent" },
        { HASH_SpinnerComponent, "SpinnerComponent" },
        { HASH_SwappableRenderComponent, "SwappableRenderComponent" },
        { HASH_TextRenderComponent, "TextRenderComponent" },
        { HASH_TextureRenderComponent, "TextureRenderComponent" },
        { HASH_PlayerControllerComponent, "PlayerControllerComponent" }
    };

    inline std::string GetComponentTypeFromHash(uint32_t hash)
    {
        auto it = COMPONENT_TYPE_MAP.find(hash);
        if (it != COMPONENT_TYPE_MAP.end())
        {
            return it->second;
        }
        return "Unknown";
    }
}

#endif
