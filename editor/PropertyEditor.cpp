#include "PropertyEditor.h"
#include <imgui.h>
#include <sstream>
#include <iomanip>
#include <cstdio>
#include <cstdlib>

namespace dae
{
    bool PropertyEditor::s_initialized = false;

    std::unordered_map<std::string, PropertyEditor::PropertyRenderer>& PropertyEditor::GetTypeRenderersMap()
    {
        static std::unordered_map<std::string, PropertyRenderer> typeRenderersMap;
        return typeRenderersMap;
    }

    void PropertyEditor::InitializeTypeRegistry()
    {
        if (s_initialized)
            return;

        auto& renderers = GetTypeRenderersMap();

        // Register default type renderers
        renderers["float"] = [](const std::string& label, std::string& value, const std::string& id)
        {
            return RenderFloatProperty(label, value, id);
        };

        renderers["int"] = [](const std::string& label, std::string& value, const std::string& id)
        {
            return RenderIntProperty(label, value, id);
        };

        renderers["bool"] = [](const std::string& label, std::string& value, const std::string& id)
        {
            return RenderBoolProperty(label, value, id);
        };

        renderers["std::string"] = [](const std::string& label, std::string& value, const std::string& id)
        {
            return RenderStringProperty(label, value, id);
        };

        renderers["SDL_Color"] = [](const std::string& label, std::string& value, const std::string& id)
        {
            return RenderColorProperty(label, value, id);
        };

        renderers["glm::vec2"] = [](const std::string& label, std::string& value, const std::string& id)
        {
            return RenderVec2Property(label, value, id);
        };

        renderers["uint8_t"] = [](const std::string& label, std::string& value, const std::string& id)
        {
            return RenderUInt8Property(label, value, id);
        };

        s_initialized = true;
    }

    void PropertyEditor::RegisterTypeRenderer(const std::string& typeName, PropertyRenderer renderer)
    {
        auto& renderers = GetTypeRenderersMap();
        renderers[typeName] = renderer;
    }

    bool PropertyEditor::RenderPropertyField(const PropertyMetadata& metadata, 
                                             std::string& value,
                                             const std::string& uniqueId)
    {
        // Ensure registry is initialized
        if (!s_initialized)
            InitializeTypeRegistry();

        const std::string& displayName = metadata.displayName.empty() ? metadata.name : metadata.displayName;

        // Look up the renderer for this type
        auto& renderers = GetTypeRenderersMap();
        auto it = renderers.find(metadata.type);
        bool changed = false;

        if (it != renderers.end())
        {
            // Use registered renderer
            changed = it->second(displayName, value, uniqueId);
        }
        else
        {
            // Fallback to string input for unknown types
            changed = RenderStringProperty(displayName, value, uniqueId);
        }

        // Display tooltip if available
        if (!metadata.tooltip.empty() && ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("%s", metadata.tooltip.c_str());
        }

        return changed;
    }

    bool PropertyEditor::RenderFloatProperty(const std::string& label, std::string& value, const std::string& uniqueId)
    {
        float floatValue = 0.0f;
        ParseFloat(value, floatValue);

        std::string inputLabel = label + "##" + uniqueId;
        if (ImGui::DragFloat(inputLabel.c_str(), &floatValue, 0.01f))
        {
            value = FloatToString(floatValue);
            return true;
        }
        return false;
    }

    bool PropertyEditor::RenderIntProperty(const std::string& label, std::string& value, const std::string& uniqueId)
    {
        int intValue = 0;
        ParseInt(value, intValue);

        std::string inputLabel = label + "##" + uniqueId;
        if (ImGui::DragInt(inputLabel.c_str(), &intValue, 1))
        {
            value = std::to_string(intValue);
            return true;
        }
        return false;
    }

    bool PropertyEditor::RenderBoolProperty(const std::string& label, std::string& value, const std::string& uniqueId)
    {
        bool boolValue = (value == "true" || value == "1" || value == "True");

        std::string inputLabel = label + "##" + uniqueId;
        if (ImGui::Checkbox(inputLabel.c_str(), &boolValue))
        {
            value = boolValue ? "true" : "false";
            return true;
        }
        return false;
    }

    bool PropertyEditor::RenderStringProperty(const std::string& label, std::string& value, const std::string& uniqueId)
    {
        char buffer[256];
        strcpy_s(buffer, sizeof(buffer), value.c_str());

        std::string inputLabel = label + "##" + uniqueId;
        if (ImGui::InputText(inputLabel.c_str(), buffer, sizeof(buffer)))
        {
            value = buffer;
            return true;
        }
        return false;
    }

    bool PropertyEditor::RenderColorProperty(const std::string& label, std::string& value, const std::string& uniqueId)
    {
        ImVec4 colorVec(1.0f, 1.0f, 1.0f, 1.0f);
        int r = 255, g = 255, b = 255, a = 255;
        
        if (!value.empty() && ParseColor(value, r, g, b, a))
        {
            colorVec = ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
        }

        std::string inputLabel = label + "##" + uniqueId;
        if (ImGui::ColorEdit4(inputLabel.c_str(), (float*)&colorVec))
        {
            r = static_cast<int>(colorVec.x * 255);
            g = static_cast<int>(colorVec.y * 255);
            b = static_cast<int>(colorVec.z * 255);
            a = static_cast<int>(colorVec.w * 255);
            value = ColorToString(r, g, b, a);
            return true;
        }
        return false;
    }

    bool PropertyEditor::RenderVec2Property(const std::string& label, std::string& value, const std::string& uniqueId)
    {
        float x = 0.0f, y = 0.0f;
        ParseVec2(value, x, y);

        float values[2] = { x, y };
        std::string inputLabel = label + "##" + uniqueId;
        if (ImGui::DragFloat2(inputLabel.c_str(), values, 0.01f))
        {
            value = Vec2ToString(values[0], values[1]);
            return true;
        }
        return false;
    }

    bool PropertyEditor::RenderUInt8Property(const std::string& label, std::string& value, const std::string& uniqueId)
    {
        int intValue = 0;
        if (!value.empty())
        {
            intValue = static_cast<int>(static_cast<unsigned char>(std::stoi(value)));
        }

        std::string inputLabel = label + "##" + uniqueId;
        if (ImGui::DragInt(inputLabel.c_str(), &intValue, 1, 0, 255))
        {
            intValue = (intValue < 0) ? 0 : (intValue > 255) ? 255 : intValue;
            value = std::to_string(static_cast<unsigned char>(intValue));
            return true;
        }
        return false;
    }

    bool PropertyEditor::ParseFloat(const std::string& str, float& outValue)
    {
        if (str.empty())
        {
            outValue = 0.0f;
            return true;
        }
        try
        {
            outValue = std::stof(str);
            return true;
        }
        catch (...)
        {
            outValue = 0.0f;
            return false;
        }
    }

    bool PropertyEditor::ParseInt(const std::string& str, int& outValue)
    {
        if (str.empty())
        {
            outValue = 0;
            return true;
        }
        try
        {
            outValue = std::stoi(str);
            return true;
        }
        catch (...)
        {
            outValue = 0;
            return false;
        }
    }

    bool PropertyEditor::ParseVec2(const std::string& str, float& x, float& y)
    {
        x = 0.0f;
        y = 0.0f;

        if (str.empty())
            return true;

        // Try parsing "x,y" format
        size_t commaPos = str.find(',');
        if (commaPos != std::string::npos)
        {
            try
            {
                x = std::stof(str.substr(0, commaPos));
                y = std::stof(str.substr(commaPos + 1));
                return true;
            }
            catch (...)
            {
                return false;
            }
        }

        // Try parsing "x y" format (glm::vec2 default)
        std::istringstream iss(str);
        if (iss >> x >> y)
            return true;

        return false;
    }

    bool PropertyEditor::ParseColor(const std::string& str, int& r, int& g, int& b, int& a)
    {
        r = 255;
        g = 255;
        b = 255;
        a = 255;

        if (str.empty())
            return true;

        int parsed = sscanf_s(str.c_str(), "%d,%d,%d,%d", &r, &g, &b, &a);
        return (parsed >= 3);
    }

    std::string PropertyEditor::FloatToString(float value)
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(6) << value;
        std::string result = oss.str();
        
        // Remove trailing zeros
        result.erase(result.find_last_not_of('0') + 1, std::string::npos);
        if (result.back() == '.')
            result.pop_back();
        
        return result;
    }

    std::string PropertyEditor::Vec2ToString(float x, float y)
    {
        char buffer[64];
        sprintf_s(buffer, sizeof(buffer), "%g,%g", x, y);
        return buffer;
    }

    std::string PropertyEditor::ColorToString(int r, int g, int b, int a)
    {
        char buffer[64];
        sprintf_s(buffer, sizeof(buffer), "%d,%d,%d,%d", r, g, b, a);
        return buffer;
    }
}
