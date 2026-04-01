#include "EditorUI.h"
#include <string>
#include <vector>

char EditorUI::m_nameBuffer[256] = "GameObject";
bool EditorUI::m_openAddDialog = false;
bool EditorUI::m_openDeleteDialog = false;

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
