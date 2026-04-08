#pragma once

#include <imgui.h>
#include "../gencomponents/GameObject_Barebones.h"
#include "EditorComponentData.h"
#include "EditorScene.h"
#include "InputBindingEditor.h"
#include "EditorSerialization.h"
#include <memory>
#include <string>
#include <map>

class EditorUI
{
public:
    EditorUI();

    void RenderMenuBar(EditorScene& scene);
    void RenderSceneGraphPanel(EditorScene& scene, dae::GameObject_Barebones*& selectedObject, 
                              dae::GameObject_Barebones*& deleteTarget);
    void RenderPropertiesPanel(dae::GameObject_Barebones* selectedObject);
    void RenderComponentsPanel(EditorScene& scene, dae::GameObject_Barebones* selectedObject);
    void RenderDialogs(EditorScene& scene, dae::GameObject_Barebones*& selectedObject,
                      dae::GameObject_Barebones*& deleteTarget);

    InputBindingEditor& GetInputBindingEditor() { return m_inputBindingEditor; }
    EditorSerialization& GetSerialization() { return m_serialization; }

    bool IsSceneGraphVisible() const { return m_showSceneGraph; }
    bool IsPropertiesVisible() const { return m_showProperties; }
    bool IsComponentsVisible() const { return m_showComponents; }
    bool IsInputBindingEditorVisible() const { return m_showInputBindingEditor; }
    bool IsMenuBarVisible() const { return m_showMenuBar; }
    void ToggleMenuBar() { m_showMenuBar = !m_showMenuBar; }

private:
    void DrawTreeNode(dae::GameObject_Barebones* obj, dae::GameObject_Barebones*& selectedObject,
                     dae::GameObject_Barebones*& draggedObject);

    InputBindingEditor m_inputBindingEditor;
    EditorSerialization m_serialization;

    bool m_showSceneGraph = true;
    bool m_showProperties = true;
    bool m_showComponents = true;
    bool m_showInputBindingEditor = true;
    bool m_showMenuBar = true;

    static char m_nameBuffer[256];
    static bool m_openAddDialog;
    static bool m_openDeleteDialog;
    static bool m_openAddComponentDialog;
    static std::string m_selectedComponentType;
    static int m_selectedComponentIndex;
};
