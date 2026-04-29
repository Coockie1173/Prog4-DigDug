#pragma once

#include <string>
#include <memory>
#include <functional>
#include <ComponentData.h>
#include "../gencomponents/GameObject_Barebones.h"
#include "EditorScene.h"
#include "InputBindingEditor.h"

class EditorSerialization
{
public:
    EditorSerialization();

    void OpenSaveDialog(EditorScene* scene, InputBindingEditor* inputBindingEditor);
    void OpenLoadDialog(EditorScene* scene, InputBindingEditor* inputBindingEditor);

private:
    static void SaveFileDialogCallback(void* userdata, const char* const* filelist, int);
    static void LoadFileDialogCallback(void* userdata, const char* const* filelist, int);

    void SaveSceneToFile(const std::string& filepath);
    void LoadSceneFromFile(const std::string& filepath);

    EditorScene* m_sceneToSave = nullptr;
    InputBindingEditor* m_inputBindingToSave = nullptr;
    EditorScene* m_sceneToLoad = nullptr;
    InputBindingEditor* m_inputBindingToLoad = nullptr;
};

