#ifndef _MBINSERIALIZER_H_
#define _MBINSERIALIZER_H_

#include <fstream>
#include <string>
#include <map>

namespace dae::serialization
{
    /// <summary>
    /// Low-level binary I/O utilities for MBIN format serialization
    /// </summary>

    /// <summary>
    /// Write binary data to file
    /// </summary>
    template<typename T>
    void WriteData(std::ofstream& file, const T& data)
    {
        file.write(reinterpret_cast<const char*>(&data), sizeof(T));
    }

    /// <summary>
    /// Read binary data from file
    /// </summary>
    template<typename T>
    void ReadData(std::ifstream& file, T& data)
    {
        file.read(reinterpret_cast<char*>(&data), sizeof(T));
    }

    /// <summary>
    /// Write string to file with length prefix
    /// </summary>
    void WriteString(std::ofstream& file, std::string_view str);

    /// <summary>
    /// Read string from file with length prefix
    /// </summary>
    std::string ReadString(std::ifstream& file);

    /// <summary>
    /// Write string map to file
    /// </summary>
    void WriteMap(std::ofstream& file, const std::map<std::string, std::string>& map);

    /// <summary>
    /// Read string map from file
    /// </summary>
    std::map<std::string, std::string> ReadMap(std::ifstream& file);
}

#endif
