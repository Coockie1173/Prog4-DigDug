#include <TerrainGridLevelLoader.h>
#include <fstream>
#include <stdexcept>

namespace dae
{
    namespace
    {
        template<typename T>
        T ReadT(std::istream& in)
        {
            T v{};
            in.read(reinterpret_cast<char*>(&v), sizeof(T));
            return v;
        }
    }

    TerrainData TerrainLoader::Read(const std::string& path)
    {
        std::ifstream in(path, std::ios::binary);
        if (!in)
            throw std::runtime_error("TerrainLoader: cannot open: " + path);

        TerrainData data;

        const auto carvedCount = ReadT<uint16_t>(in);
        data.carvedCells.reserve(carvedCount);
        for (uint16_t i = 0; i < carvedCount; ++i)
        {
            const auto x = ReadT<uint16_t>(in);
            const auto y = ReadT<uint16_t>(in);
            data.carvedCells.push_back({ static_cast<int>(x), static_cast<int>(y) });
        }

        const auto connCount = ReadT<uint16_t>(in);
        data.connections.reserve(connCount);
        for (uint16_t i = 0; i < connCount; ++i)
        {
            const auto fx = ReadT<uint16_t>(in);
            const auto fy = ReadT<uint16_t>(in);
            const auto tx = ReadT<uint16_t>(in);
            const auto ty = ReadT<uint16_t>(in);
            data.connections.push_back({ { static_cast<int>(fx), static_cast<int>(fy) },
                                         { static_cast<int>(tx), static_cast<int>(ty) } });
        }

        const auto spawnCount = ReadT<uint16_t>(in);
        data.enemySpawns.reserve(spawnCount);
        for (uint16_t i = 0; i < spawnCount; ++i)
        {
            EnemySpawnData s;
            s.type = static_cast<EnemyType>(ReadT<uint8_t>(in));
            s.cell.x = static_cast<int>(ReadT<uint16_t>(in));
            s.cell.y = static_cast<int>(ReadT<uint16_t>(in));
            data.enemySpawns.push_back(s);
        }

        if (!in)
            throw std::runtime_error("TerrainLoader: truncated file: " + path);

        return data;
    }
};