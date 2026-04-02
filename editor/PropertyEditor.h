#pragma once

#include <string>
#include <functional>
#include <unordered_map>
#include "../gencomponents/ComponentRegisterMaster.h"

namespace dae
{
    class PropertyEditor
    {
    public:
        using PropertyRenderer = std::function<bool(const std::string&, std::string&, const std::string&)>;

        // Initialize the type registry with default renderers
        static void InitializeTypeRegistry();

        // Render a property input field based on its type
        // Returns true if the value was modified
        static bool RenderPropertyField(const PropertyMetadata& metadata, 
                                       std::string& value,
                                       const std::string& uniqueId);

        // Register a custom renderer for a specific type
        static void RegisterTypeRenderer(const std::string& typeName, PropertyRenderer renderer);

    private:
        static std::unordered_map<std::string, PropertyRenderer>& GetTypeRenderersMap();
        static bool s_initialized;

        // Type-specific renderers
        static bool RenderFloatProperty(const std::string& label, std::string& value, const std::string& uniqueId);
        static bool RenderIntProperty(const std::string& label, std::string& value, const std::string& uniqueId);
        static bool RenderBoolProperty(const std::string& label, std::string& value, const std::string& uniqueId);
        static bool RenderStringProperty(const std::string& label, std::string& value, const std::string& uniqueId);
        static bool RenderColorProperty(const std::string& label, std::string& value, const std::string& uniqueId);
        static bool RenderVec2Property(const std::string& label, std::string& value, const std::string& uniqueId);
        static bool RenderUInt8Property(const std::string& label, std::string& value, const std::string& uniqueId);

        // Helper functions
        static bool ParseFloat(const std::string& str, float& outValue);
        static bool ParseInt(const std::string& str, int& outValue);
        static bool ParseVec2(const std::string& str, float& x, float& y);
        static std::string FloatToString(float value);
        static std::string Vec2ToString(float x, float y);
        static std::string ColorToString(int r, int g, int b, int a = 255);
        static bool ParseColor(const std::string& str, int& r, int& g, int& b, int& a);
    };
}
