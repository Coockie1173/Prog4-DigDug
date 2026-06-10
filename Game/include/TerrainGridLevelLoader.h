#ifndef _TERRAINLOADER_H_
#define _TERRAINLOADER_H_

#include <cstdint>
#include <vector>
#include <glm/glm.hpp>
#include <string>

namespace dae
{
    enum class EnemyType : uint8_t { Pooka = 0, Fygar = 1 }; //can be translated to a bool that says "can spit fire" or nah
    struct EnemySpawnData
    {
        EnemyType  type;
        glm::ivec2 cell;
    };

    struct TerrainData
    {
        std::vector<glm::ivec2> carvedCells;
        std::vector<std::pair<glm::ivec2, glm::ivec2>> connections;
        std::vector<EnemySpawnData> enemySpawns;
    };

    class ITerrainDeserializeHelper
    {
    public:
        virtual void PassData(bool CanAttack, std::string SpriteDir) = 0;
    };

    class TerrainLoader
    {
    public:
        static TerrainData Read(const std::string& path);
    };
};

#endif