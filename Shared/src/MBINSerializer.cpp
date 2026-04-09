#include "MBINSerializer.h"

namespace dae::serialization
{
    void WriteString(std::ofstream& file, std::string_view str)
    {
        auto length = static_cast<uint32_t>(str.length());
        WriteData(file, length);
        if (length > 0)
        {
            file.write(str.data(), length);
        }
    }

    std::string ReadString(std::ifstream& file)
    {
        uint32_t length = 0;
        ReadData(file, length);
        if (length > 0)
        {
            std::string buffer(length, '\0');
            file.read(buffer.data(), length);
            return buffer;
        }
        return "";
    }

    void WriteMap(std::ofstream& file, const std::map<std::string, std::string>& map)
    {
        auto size = static_cast<uint32_t>(map.size());
        WriteData(file, size);
        for (const auto& [key, value] : map)
        {
            WriteString(file, value);
            WriteString(file, key);
        }
    }

    std::map<std::string, std::string> ReadMap(std::ifstream& file)
    {
        std::map<std::string, std::string> result;
        uint32_t size = 0;
        ReadData(file, size);
        for (uint32_t i = 0; i < size; ++i)
        {
            auto value = ReadString(file);
            auto key = ReadString(file);
            result.emplace(key, value);
        }
        return result;
    }
}
