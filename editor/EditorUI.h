#pragma once

#include <imgui.h>
#include "../gencomponents/GameObject_Barebones.h"
#include "EditorScene.h"

class EditorUI
{
public:
    void RenderSceneGraphPanel(EditorScene& scene, dae::GameObject_Barebones*& selectedObject, 
                              dae::GameObject_Barebones*& deleteTarget);
    void RenderPropertiesPanel(dae::GameObject_Barebones* selectedObject);
    void RenderDialogs(EditorScene& scene, dae::GameObject_Barebones*& selectedObject,
                      dae::GameObject_Barebones*& deleteTarget);

private:
    void DrawTreeNode(dae::GameObject_Barebones* obj, dae::GameObject_Barebones*& selectedObject,
                     dae::GameObject_Barebones*& draggedObject);
    static char m_nameBuffer[256];
    static bool m_openAddDialog;
    static bool m_openDeleteDialog;
};
