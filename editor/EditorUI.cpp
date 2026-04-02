#include "EditorUI.h"
#include <string>
#include <vector>
#include "gencomponents/ComponentRegisterMaster.h"
#include "PropertyEditor.h"

char EditorUI::m_nameBuffer[256] = "GameObject";
bool EditorUI::m_openAddDialog = false;
bool EditorUI::m_openDeleteDialog = false;
bool EditorUI::m_openAddComponentDialog = false;
std::string EditorUI::m_selectedComponentType = "";
int EditorUI::m_selectedComponentIndex = -1;

// Initialize the property editor registry on first use
namespace {
    struct PropertyEditorInitializer {
        PropertyEditorInitializer() {
            dae::PropertyEditor::InitializeTypeRegistry();
        }
    };
    static PropertyEditorInitializer s_initPropertyEditor;
}

void EditorUI::RenderSceneGraphPanel(EditorScene& scene, dae::GameObject_Barebones*& selectedObject,
                                     dae::GameObject_Barebones*& deleteTarget)
{
    ImGui::Begin("Scene Graph");
    if (ImGui::Button("Add GameObject"))
    {
        m_openAddDialog = true;
    }

    ImGui::SameLine();
    if (ImGui::Button("Remove") && selectedObject)
    {
        deleteTarget = selectedObject;
        m_openDeleteDialog = true;
    }

    ImGui::Separator();

    auto roots = scene.GetRootObjects();
    static dae::GameObject_Barebones* draggedObject = nullptr;

    for (auto root : roots)
    {
        DrawTreeNode(root, selectedObject, draggedObject);
    }

    ImGui::End();
}

void EditorUI::RenderDialogs(EditorScene& scene, dae::GameObject_Barebones*& selectedObject,
                            dae::GameObject_Barebones*& deleteTarget)
{
    if (m_openAddDialog)
    {
        ImGui::OpenPopup("Add GameObject##dialog");
        m_openAddDialog = false;
    }

    if (m_openDeleteDialog)
    {
        ImGui::OpenPopup("Delete GameObject##confirm");
        m_openDeleteDialog = false;
    }

    // Add GameObject Dialog
    if (ImGui::BeginPopupModal("Add GameObject##dialog", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("GameObject Name:");
        ImGui::InputText("##name", m_nameBuffer, IM_ARRAYSIZE(m_nameBuffer));

        if (ImGui::Button("Create", ImVec2(120, 0)))
        {
            scene.CreateGameObject(m_nameBuffer, selectedObject);
            strcpy_s(m_nameBuffer, IM_ARRAYSIZE(m_nameBuffer), "GameObject");
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            strcpy_s(m_nameBuffer, IM_ARRAYSIZE(m_nameBuffer), "GameObject");
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    // Delete GameObject Dialog
    if (ImGui::BeginPopupModal("Delete GameObject##confirm", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Are you sure you want to delete '%s'?", deleteTarget ? deleteTarget->GetObjectName().c_str() : "");
        ImGui::Text("This will also delete all of its children.");

        if (ImGui::Button("Delete", ImVec2(120, 0)))
        {
            if (deleteTarget)
            {
                auto childrenCopy = deleteTarget->GetChildren();
                for (auto child : childrenCopy)
                {
                    scene.DeleteGameObject(child);
                }
                scene.DeleteGameObject(deleteTarget);
                if (selectedObject == deleteTarget)
                    selectedObject = nullptr;
                deleteTarget = nullptr;
            }
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            deleteTarget = nullptr;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    // Add Component Dialog
    if (m_openAddComponentDialog)
    {
        ImGui::OpenPopup("Add Component##dialog");
        m_openAddComponentDialog = false;
    }

    if (ImGui::BeginPopupModal("Add Component##dialog", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        const auto& allMetadata = dae::GetAllComponentMetadata();

        if (allMetadata.empty())
        {
            ImGui::TextDisabled("No components available");
        }
        else
        {
            ImGui::Text("Select Component Type:");
            ImGui::Separator();

            std::vector<std::string> validComponents;
            for (size_t i = 0; i < allMetadata.size(); ++i)
            {
                if (allMetadata[i].componentName != "Component")
                {
                    validComponents.push_back(allMetadata[i].componentName);
                }
            }

            const char* previewValue = m_selectedComponentType.empty() ? "Select a component" : m_selectedComponentType.c_str();
            if (ImGui::BeginCombo("##component_combo", previewValue))
            {
                for (size_t i = 0; i < validComponents.size(); ++i)
                {
                    bool isSelected = m_selectedComponentType == validComponents[i];
                    if (ImGui::Selectable(validComponents[i].c_str(), isSelected))
                    {
                        m_selectedComponentType = validComponents[i];
                        m_selectedComponentIndex = static_cast<int>(i);
                    }
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            ImGui::Separator();

            if (ImGui::Button("Add", ImVec2(120, 0)) && selectedObject && !m_selectedComponentType.empty())
            {
                dae::ComponentInstance newComponent;
                newComponent.componentType = m_selectedComponentType;
                newComponent.componentName = m_selectedComponentType + "_0";

                // Find metadata for this component and initialize properties
                for (const auto& meta : allMetadata)
                {
                    if (meta.componentName == m_selectedComponentType)
                    {
                        for (const auto& prop : meta.properties)
                        {
                            newComponent.properties[prop.name] = "";
                        }
                        break;
                    }
                }

                auto handle = std::make_unique<dae::ComponentInstance>(std::move(newComponent));
                 const auto* handlePtr = handle.get();
                 selectedObject->AddComponent(std::move(handle));
                 scene.RegisterComponentType(selectedObject, handlePtr, m_selectedComponentType);
                m_selectedComponentType = "";
                m_selectedComponentIndex = -1;
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();
            if (ImGui::Button("Cancel##comp", ImVec2(120, 0)))
            {
                m_selectedComponentType = "";
                m_selectedComponentIndex = -1;
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::EndPopup();
    }
}

void EditorUI::RenderPropertiesPanel(dae::GameObject_Barebones* selectedObject)
{
    ImGui::Begin("Properties");
    if (selectedObject)
    {
        ImGui::Text("GameObject ID: %d", selectedObject->GetId());
        ImGui::Separator();

            std::string name = selectedObject->GetObjectName();
            std::vector<char> nameBuffer(name.begin(), name.end());
            nameBuffer.resize(256);

            if (ImGui::InputText("Name##prop", nameBuffer.data(), nameBuffer.size()))
            {
                selectedObject->SetObjectName(std::string(nameBuffer.data()));
            }

            bool isDebug = selectedObject->GetIsDebugData();
            if (ImGui::Checkbox("Is Debug Data##prop", &isDebug))
            {
                selectedObject->SetIsDebugData(isDebug);
            }

            ImGui::Separator();
            ImGui::TextUnformatted("Transform:");

            glm::vec2 worldPos = selectedObject->GetWorldPosition();
            float worldPosArray[2] = { worldPos.x, worldPos.y };
            if (ImGui::DragFloat2("World Position##prop", worldPosArray, 0.1f))
            {
                glm::vec2 newWorldPos(worldPosArray[0], worldPosArray[1]);
                selectedObject->SetWorldPosition(newWorldPos);

                if (selectedObject->GetParent())
                {
                    glm::vec2 parentWorldPos = selectedObject->GetParent()->GetWorldPosition();
                    glm::vec2 newLocalPos = newWorldPos - parentWorldPos;
                    selectedObject->SetLocalPosition(newLocalPos);
                }
                else
                {
                    selectedObject->SetLocalPosition(newWorldPos);
                }
            }

            glm::vec2 localPos = selectedObject->GetLocalPosition();
            float localPosArray[2] = { localPos.x, localPos.y };
            ImGui::BeginDisabled();
            ImGui::DragFloat2("Local Position##prop", localPosArray, 0.1f);
            ImGui::EndDisabled();

            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Auto-calculated from World Position and Parent");
            }

            ImGui::Separator();
            ImGui::TextUnformatted("Hierarchy:");

            if (selectedObject->GetParent())
            {
                ImGui::Text("Parent: %s (ID: %d)",
                    selectedObject->GetParent()->GetObjectName().c_str(),
                    selectedObject->GetParent()->GetId());

                if (ImGui::Button("Unparent##action", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
                {
                    selectedObject->GetParent()->RemoveChild(selectedObject);
                }
            }
            else
            {
                ImGui::TextDisabled("No Parent (Root Object)");
            }

        ImGui::Text("Children: %zu", selectedObject->GetChildren().size());
        for (auto child : selectedObject->GetChildren())
        {
            ImGui::BulletText("%s (ID: %d)", child->GetObjectName().c_str(), child->GetId());
        }
    }
    else
    {
        ImGui::TextDisabled("No GameObject selected");
    }

    ImGui::End();
}

void EditorUI::RenderComponentsPanel(EditorScene& scene, dae::GameObject_Barebones* selectedObject)
{
    ImGui::Begin("Components");
    if (selectedObject)
    {
        if (ImGui::Button("Add Component", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
        {
            m_openAddComponentDialog = true;
            m_selectedComponentType = "";
            m_selectedComponentIndex = -1;
        }

        ImGui::Separator();

        const auto& components = selectedObject->GetComponents();
        const auto& allMetadata = dae::GetAllComponentMetadata();

        if (components.empty())
        {
            ImGui::TextDisabled("No components attached");
        }
        else
        {
            ImGui::Text("Attached Components (%zu):", components.size());
            ImGui::Separator();

            for (size_t i = 0; i < components.size(); ++i)
            {
                const dae::ComponentHandle& compHandle = components[i];
                const auto& comp = compHandle;
                if (!comp)
                {
                    continue;
                }

                ImGui::PushID(static_cast<int>(i));

                if (ImGui::CollapsingHeader(comp->componentType.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                {
                    // Find metadata for this component type
                    const dae::ComponentMetadata* metadata = nullptr;
                    for (const auto& meta : allMetadata)
                    {
                        if (meta.componentName == comp->componentType)
                        {
                            metadata = &meta;
                            break;
                        }
                    }

                    // Display and edit properties
                    if (metadata)
                    {
                        for (const auto& propMeta : metadata->properties)
                        {
                            auto it = comp->properties.find(propMeta.name);
                            if (it == comp->properties.end())
                            {
                                comp->properties[propMeta.name] = "";
                            }

                            std::string propertyKey = comp->componentType + "_" + propMeta.name + "_" + std::to_string(i);

                            // Use PropertyEditor to render type-specific input fields
                            dae::PropertyEditor::RenderPropertyField(propMeta, comp->properties[propMeta.name], propertyKey);
                        }

                        // Display preview for TextRenderComponent
                        if (comp->componentType == "TextRenderComponent")
                        {
                            ImGui::Separator();
                            ImGui::TextUnformatted("Preview:");

                            auto textIt = comp->properties.find("text");
                            auto colorIt = comp->properties.find("color");

                            if (textIt != comp->properties.end() && !textIt->second.empty())
                            {
                                ImVec4 previewColor(1.0f, 1.0f, 1.0f, 1.0f);
                                if (colorIt != comp->properties.end() && !colorIt->second.empty())
                                {
                                    int r = 255, g = 255, b = 255, a = 255;
                                    sscanf_s(colorIt->second.c_str(), "%d,%d,%d,%d", &r, &g, &b, &a);
                                    previewColor = ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
                                }
                                ImGui::TextColored(previewColor, "%s", textIt->second.c_str());
                            }
                            else
                            {
                                ImGui::TextDisabled("(No text set)");
                            }
                        }

                        // Display preview for TextureRenderComponent
                        if (comp->componentType == "TextureRenderComponent")
                        {
                            ImGui::Separator();
                            ImGui::TextUnformatted("Preview:");

                            auto textureIt = comp->properties.find("textureName");
                            if (textureIt != comp->properties.end() && !textureIt->second.empty())
                            {
                                ImGui::Text("Texture: %s", textureIt->second.c_str());
                                ImGui::TextDisabled("(Texture preview not available in editor)");
                            }
                            else
                            {
                                ImGui::TextDisabled("(No texture set)");
                            }
                        }
                    }

                    ImGui::Spacing();
                     if (ImGui::Button("Remove Component##remove", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
                     {
                         scene.UnregisterComponentType(selectedObject, comp.get());
                         selectedObject->RemoveComponent(comp.get());
                         // Break out of the loop since we modified the vector
                         ImGui::PopID();
                         break;
                     }
                }

                ImGui::PopID();
            }
        }
    }
    else
    {
        ImGui::TextDisabled("No GameObject selected");
    }

    ImGui::End();
}

void EditorUI::DrawTreeNode(dae::GameObject_Barebones* obj, dae::GameObject_Barebones*& selectedObject,
                           dae::GameObject_Barebones*& draggedObject)
{
    bool isSelected = obj == selectedObject;
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
    if (isSelected)
        flags |= ImGuiTreeNodeFlags_Selected;
    if (obj->GetChildren().empty())
        flags |= ImGuiTreeNodeFlags_Leaf;

    std::string displayName = obj->GetObjectName() + " (ID: " + std::to_string(obj->GetId()) + ")";
    bool nodeOpen = ImGui::TreeNodeEx(displayName.c_str(), flags);

    if (ImGui::IsItemClicked())
        selectedObject = obj;

    if (ImGui::BeginDragDropSource())
    {
        draggedObject = obj;
        ImGui::SetDragDropPayload("GAMEOBJECT", &draggedObject, sizeof(dae::GameObject_Barebones*));
        ImGui::Text("Move: %s", displayName.c_str());
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT"))
        {
            dae::GameObject_Barebones* dropped = *(dae::GameObject_Barebones**)payload->Data;
            if (dropped != obj && dropped->GetParent() != obj)
            {
                obj->AddChild(dropped);
            }
        }
        ImGui::EndDragDropTarget();
    }

    if (nodeOpen)
    {
        for (auto child : obj->GetChildren())
        {
            DrawTreeNode(child, selectedObject, draggedObject);
        }
        ImGui::TreePop();
    }
}
