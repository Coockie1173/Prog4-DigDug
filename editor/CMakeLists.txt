set(EDITOR_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/editor/gencomponents")

add_custom_target(GenerateBarebonesComponents ALL
  COMMAND $<TARGET_FILE:ComponentPrecompiler>
    "${CMAKE_SOURCE_DIR}/Minigin/GameObject.h"
    "${EDITOR_OUTPUT_DIR}"
    "${CMAKE_SOURCE_DIR}/Minigin/Components"
    "${CMAKE_SOURCE_DIR}/Game/include/Components"
  COMMENT "Generating barebones components..."
)

add_dependencies(GenerateBarebonesComponents ComponentPrecompiler)

set(GENERATED_CPP_FILES
  "${EDITOR_OUTPUT_DIR}/ComponentRegisterMaster.cpp"
)

add_executable(minigin_editor
  Main.cpp
  EditorScene.cpp
  SceneViewport.cpp
  EditorUI.cpp
  PropertyEditor.cpp
  InputBindingEditor.cpp
  EditorSerialization.cpp
  ${GENERATED_CPP_FILES}
)

add_dependencies(minigin_editor GenerateBarebonesComponents)

set_target_properties(minigin_editor PROPERTIES
  VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:minigin_editor>"
)

set_source_files_properties(
  ${GENERATED_CPP_FILES}
  PROPERTIES GENERATED TRUE
)

target_compile_features(minigin_editor PUBLIC cxx_std_20)

target_include_directories(minigin_editor PUBLIC
  "${EDITOR_OUTPUT_DIR}"
)

target_link_libraries(minigin_editor PUBLIC
  minigin_engine
  SDL3::SDL3
  SDL3_ttf::SDL3_ttf
  glm::glm
  imgui
  ${VLD_LIBRARIES}
)

target_compile_definitions(minigin_editor PRIVATE M_EDITOR)

if(WIN32)
  find_library(XINPUT_LIBRARY Xinput)
  if(XINPUT_LIBRARY)
    target_link_libraries(minigin_editor PUBLIC ${XINPUT_LIBRARY})
  endif()
endif()

if(MSVC)
  target_compile_options(minigin_editor PRIVATE /W4 /WX)
else()
  target_compile_options(minigin_editor PRIVATE -Wall -Wextra -Werror)
endif()

if(WIN32)
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT minigin_editor)
endif()

if(WIN32)
  set(EDITOR_SDL_DLLS
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
  )

  foreach(dllTarget IN LISTS EDITOR_SDL_DLLS)
    add_custom_command(
      TARGET minigin_editor POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "$<TARGET_FILE:${dllTarget}>"
          "$<TARGET_FILE_DIR:minigin_editor>"
      VERBATIM
    )
  endforeach()
endif()

add_custom_command(TARGET minigin_editor POST_BUILD
  COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/../Data" "$<TARGET_FILE_DIR:minigin_editor>/Data"
)