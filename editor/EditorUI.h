#pragma once

#include <imgui.h>
#include "../gencomponents/GameObject_Barebones.h"
#include "EditorComponentData.h"
#include "EditorScene.h"
#include <memory>
#include <string>
#include <map>

class EditorUI
{
public:
    void RenderSceneGraphPanel(EditorScene& scene, dae::GameObject_Barebones*& selectedObject, 
                              dae::GameObject_Barebones*& deleteTarget);
    void RenderPropertiesPanel(dae::GameObject_Barebones* selectedObject);
    void RenderComponentsPanel(EditorScene& scene, dae::GameObject_Barebones* selectedObject);
    void RenderDialogs(EditorScene& scene, dae::GameObject_Barebones*& selectedObject,
                      dae::GameObject_Barebones*& deleteTarget);

private:
    void DrawTreeNode(dae::GameObject_Barebones* obj, dae::GameObject_Barebones*& selectedObject,
                     dae::GameObject_Barebones*& draggedObject);
    static char m_nameBuffer[256];
    static bool m_openAddDialog;
    static bool m_openDeleteDialog;
    static bool m_openAddComponentDialog;
    static std::string m_selectedComponentType;
    static int m_selectedComponentIndex;
};
