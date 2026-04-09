#ifndef _SCENEDESERIALIZER_H_
#define _SCENEDESERIALIZER_H_

#include <string>
#include <memory>
#include <SceneData.h>

namespace dae
{
    /// <summary>
    /// Deserializes MBIN binary scene files into SceneData structures
    /// </summary>
    class SceneDeserializer
    {
    public:
        /// <summary>
        /// Deserialize a scene from an MBIN file
        /// </summary>
        /// <param name="filepath">Path to the MBIN file</param>
        /// <param name="errorMessage">Out parameter for error message if deserialization fails</param>
        /// <returns>Deserialized scene data, or nullptr if failed</returns>
        static std::unique_ptr<SceneData> DeserializeFromFile(
            const std::string& filepath,
            std::string& errorMessage
        );

    private:
        SceneDeserializer() = delete;
        SceneDeserializer(const SceneDeserializer&) = delete;
        SceneDeserializer& operator=(const SceneDeserializer&) = delete;
    };
}

#endif
