cmake_minimum_required(VERSION 3.26)
project(Minigin)

if(EMSCRIPTEN)
  message("Using Emscripten!")
endif()

set(BUILD_EDITOR ON CACHE BOOL "Build the editor executable")

include(FetchContent)

# add glm
find_package(glm CONFIG QUIET)
if(NOT glm_FOUND)
  FetchContent_Declare(
    glm
    URL https://github.com/g-truc/glm/releases/download/1.0.3/glm-1.0.3.zip
    DOWNLOAD_NO_PROGRESS ON
    DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
  )
  FetchContent_MakeAvailable(glm)
endif()

# Add SDL3
find_package(SDL3 3.4 CONFIG QUIET)
if(NOT SDL3_FOUND)
  if (WIN32)
    FetchContent_Declare(
      SDL3
      URL https://www.libsdl.org/release/SDL3-devel-3.4.0-VC.zip
      DOWNLOAD_NO_PROGRESS ON
      DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
    )
    FetchContent_MakeAvailable(SDL3)
    list(PREPEND CMAKE_PREFIX_PATH "${sdl3_SOURCE_DIR}")
    find_package(SDL3 CONFIG REQUIRED)
  else()
    FetchContent_Declare(
      SDL3
      GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
      GIT_TAG release-3.4.0
      GIT_SHALLOW TRUE
      GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(SDL3)
  endif()
endif()

# Add Shared library subdirectory (must be after SDL3 and GLM are available)
add_subdirectory(Shared)

# Only add precompiler as a subdirectory if building editor
if(BUILD_EDITOR)
  add_subdirectory(ComponentPrecompiler)
endif()

if(EMSCRIPTEN)
  set(TARGET_NAME minigin_web)
  set(TARGET_PARAM )
else()
  set(TARGET_NAME minigin)
  set(TARGET_PARAM WIN32)
endif()

# Common source files (shared between game and editor)
set(COMMON_SOURCES
  Minigin/Font.cpp
  Minigin/GameObject.cpp
  Minigin/InputManager.cpp
  Minigin/Minigin.cpp
  "Minigin/Components/RenderComponent.cpp"
  Minigin/ResourceManager.cpp
  Minigin/Scene.cpp
  Minigin/SceneManager.cpp
  Minigin/SceneLoader.cpp
  Minigin/TextObject.cpp
  Minigin/Texture2D.cpp   
  Minigin/Renderer.cpp      
  "Minigin/Components/TextureRendererComponent.cpp" 
  "Minigin/Components/Component.cpp" 
  "Minigin/Components/TextRendererComponent.cpp" 
  "Minigin/Debugger.cpp" 
  "Minigin/Timing.cpp" 
  "Minigin/Components/FPSCounterComponent.cpp" 
  "Minigin/Components/SpinnerComponent.cpp"  
  "Minigin/Components/ObjectMoveComponent.cpp"  
  "Minigin/Commands/MovementInputCommand.cpp"
  "Minigin/EventManager.cpp"
  "Minigin/Components/ScoreComponent.cpp" 
  "Minigin/Controller/ControllerManager.cpp"
)

# Game executable
add_executable(${TARGET_NAME} ${TARGET_PARAM}
  Minigin/Main.cpp
  ${COMMON_SOURCES}
)

# enable c++20 features
# enable c++20 features
target_compile_features(${TARGET_NAME} PUBLIC cxx_std_20)

# Dear ImGui
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui
  GIT_TAG docking
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
)

target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR} ${sdl3_SOURCE_DIR}/include)
target_link_libraries(imgui PUBLIC SDL3::SDL3)
# enable c++20 features
target_compile_features(imgui PUBLIC cxx_std_20)

# Add SDL3_ttf
find_package(SDL3_ttf 3.2.2 CONFIG QUIET)
if(NOT SDL3_ttf_FOUND)
  if (WIN32)
    FetchContent_Declare(
      SDL3_ttf
      URL https://www.libsdl.org/projects/SDL_ttf/release/SDL3_ttf-devel-3.2.2-VC.zip
      DOWNLOAD_NO_PROGRESS ON
      DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
    )
    FetchContent_MakeAvailable(SDL3_ttf)
    list(PREPEND CMAKE_PREFIX_PATH "${sdl3_ttf_SOURCE_DIR}")
    find_package(SDL3_ttf CONFIG REQUIRED)
  else()
    FetchContent_Declare(
      SDL3_ttf
      GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
      GIT_TAG release-3.2.2
      GIT_SHALLOW TRUE
      GIT_PROGRESS TRUE
    )
    set(SDL3TTF_INSTALL OFF)
    if(EMSCRIPTEN)
      list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/emscripten")
      set(SDL3TTF_USE_SYSTEM_FREETYPE ON)
      set(SDL3TTF_VENDORED OFF)
      set(SDLTTF_PLUTOSVG OFF)
      add_compile_options(-sUSE_FREETYPE=1 -sUSE_HARFBUZZ=1)
      add_link_options(-sUSE_FREETYPE=1 -sUSE_HARFBUZZ=1)
    endif()
    FetchContent_MakeAvailable(SDL3_ttf)
  endif()
endif()

if (WIN32)
  # Add VLD (Visual Leak Detector) for memory leak detection on Windows
  find_package(VLD CONFIG)
endif()

target_include_directories("${TARGET_NAME}" PUBLIC
  ${CMAKE_SOURCE_DIR}/Minigin/
  ${glm_SOURCE_DIR}
  ${sdl3_SOURCE_DIR}/include
  ${sdl3_image_SOURCE_DIR}/include
  ${sdl3_ttf_SOURCE_DIR}/include
  ${imgui_SOURCE_DIR}
  ${VLD_INCLUDE_DIR}
)

target_link_libraries("${TARGET_NAME}" PUBLIC
  SDL3::SDL3
  SDL3_ttf::SDL3_ttf
  glm::glm
  imgui
  minigin_shared
  ${VLD_LIBRARIES}
)

if(WIN32)
  find_library(XINPUT_LIBRARY Xinput)
  if(XINPUT_LIBRARY)
    target_link_libraries(${TARGET_NAME} PUBLIC ${XINPUT_LIBRARY})
  else()
    message(WARNING "XInput library not found; controller support may be unavailable")
  endif()
endif()

if(MSVC)
  # compile at warning level 4 + treat warnings as errors
  target_compile_options(${TARGET_NAME} PRIVATE /W4 /WX)
else()
  target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -Werror)
endif()

# Editor executable (desktop only, not for Emscripten)
if(BUILD_EDITOR AND NOT EMSCRIPTEN)
  set(EDITOR_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/editor/gencomponents")

  # Run precompiler to generate barebones components before building editor
  add_custom_target(GenerateBarebonesComponents ALL
    COMMAND $<TARGET_FILE:ComponentPrecompiler>
      "${CMAKE_SOURCE_DIR}/Minigin/Components"
      "${CMAKE_SOURCE_DIR}/Minigin/GameObject.h"
      "${EDITOR_OUTPUT_DIR}"
    COMMENT "Generating barebones components..."
  )

  # Explicitly list the generated ComponentRegisterMaster.cpp file
  set(GENERATED_CPP_FILES
    "${EDITOR_OUTPUT_DIR}/ComponentRegisterMaster.cpp"
  )

  add_executable(minigin_editor
    editor/Main.cpp
    editor/EditorScene.cpp
    editor/SceneViewport.cpp
    editor/EditorUI.cpp
    editor/PropertyEditor.cpp
    editor/InputBindingEditor.cpp
    editor/EditorSerialization.cpp
    ${GENERATED_CPP_FILES}
    ${COMMON_SOURCES}
  )

  # Ensure precompiler runs before building editor
  add_dependencies(minigin_editor GenerateBarebonesComponents)

  # Mark all generated .cpp files as generated to tell CMake not to check for them upfront
  set_source_files_properties(
    ${GENERATED_CPP_FILES}
    PROPERTIES GENERATED TRUE
  )

  target_compile_features(minigin_editor PUBLIC cxx_std_20)

  # Add generated components directory to include paths
  target_include_directories(minigin_editor PUBLIC
    "${EDITOR_OUTPUT_DIR}"
    ${CMAKE_SOURCE_DIR}/Minigin/
    ${glm_SOURCE_DIR}
    ${sdl3_SOURCE_DIR}/include
    ${sdl3_image_SOURCE_DIR}/include
    ${sdl3_ttf_SOURCE_DIR}/include
    ${imgui_SOURCE_DIR}
    ${VLD_INCLUDE_DIR}
  )

  target_link_libraries(minigin_editor PUBLIC
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
    glm::glm
    imgui
    minigin_shared
    ${VLD_LIBRARIES}
  )

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

  # Copy Data folder to editor build directory
  add_custom_command(TARGET minigin_editor POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
      "${CMAKE_SOURCE_DIR}/Data"
      "$<TARGET_FILE_DIR:minigin_editor>/Data"
    COMMENT "Copying Data folder to editor build directory..."
  )
endif()

if(EMSCRIPTEN)
  set(EMSCRIPTEN_COMPILE_FLAGS "${EMSCRIPTEN_LINK_FLAGS} -g")
  set(EMSCRIPTEN_LINK_FLAGS "${EMSCRIPTEN_LINK_FLAGS} -sUSE_FREETYPE=1 -sUSE_HARFBUZZ=1 -g -sALLOW_MEMORY_GROWTH --preload-file \"${CMAKE_SOURCE_DIR}/Data@/\"")

  # Use the Emscripten toolchain
  set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/EmscriptenToolchain.cmake)

  # Additional settings for Emscripten build
  set_target_properties(${TARGET_NAME} PROPERTIES
      COMPILE_FLAGS "${EMSCRIPTEN_COMPILE_FLAGS}"
      LINK_FLAGS "${EMSCRIPTEN_LINK_FLAGS}"
  )

  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/web" "$<TARGET_FILE_DIR:${TARGET_NAME}>/"
  )

  # Have emscripten generate a html page too.
  set(CMAKE_EXECUTABLE_SUFFIX ".html")

elseif(WIN32)
  # Set minigin as the default startup project
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})
  # Set working directory to output folder
  set_target_properties(${TARGET_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${TARGET_NAME}>")

  # Copy SDL dlls to output folder
  set(SDL_RUNTIME_DLLS
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
  )

  foreach(dllTarget IN LISTS SDL_RUNTIME_DLLS)
    add_custom_command(
      TARGET ${TARGET_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "$<TARGET_FILE:${dllTarget}>"
          "$<TARGET_FILE_DIR:${TARGET_NAME}>"
      VERBATIM
    )
  endforeach()

  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/Data" "$<TARGET_FILE_DIR:${TARGET_NAME}>/Data"
  )
elseif(APPLE)
  set_target_properties(${TARGET_NAME} PROPERTIES
    MACOSX_BUNDLE TRUE
    INSTALL_RPATH "@executable_path/../Frameworks"
    BUILD_WITH_INSTALL_RPATH TRUE
  )
  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_BUNDLE_DIR:${TARGET_NAME}>/Contents/Frameworks"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3::SDL3-shared>" "$<TARGET_BUNDLE_DIR:${TARGET_NAME}>/Contents/Frameworks/"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3_ttf::SDL3_ttf-shared>" "$<TARGET_BUNDLE_DIR:${TARGET_NAME}>/Contents/Frameworks/"
    install_name_tool -change "@rpath/libSDL3_ttf.0.dylib" "@executable_path/../Frameworks/libSDL3_ttf.0.2.2.dylib" "$<TARGET_FILE:${TARGET_NAME}>"
    COMMAND install_name_tool -change "@rpath/libSDL3.0.dylib" "@executable_path/../Frameworks/libSDL3.0.dylib" "$<TARGET_FILE:${TARGET_NAME}>"
  )

  # Copy Data folder to output directory after build
  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/Data" "$<TARGET_FILE_DIR:${TARGET_NAME}>/Data"
  )
else()
  # Copy Data folder to output directory after build
  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/Data" "$<TARGET_FILE_DIR:${TARGET_NAME}>/Data"
  )
endif()

# Editor-specific post-build commands (Windows)
if(BUILD_EDITOR AND NOT EMSCRIPTEN AND WIN32)
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

  add_custom_command(TARGET minigin_editor POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/Data" "$<TARGET_FILE_DIR:minigin_editor>/Data"
  )
elseif(BUILD_EDITOR AND NOT EMSCRIPTEN AND APPLE)
  add_custom_command(TARGET minigin_editor POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/Data" "$<TARGET_FILE_DIR:minigin_editor>/Data"
  )
elseif(BUILD_EDITOR AND NOT EMSCRIPTEN)
  add_custom_command(TARGET minigin_editor POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/Data" "$<TARGET_FILE_DIR:minigin_editor>/Data"
  )
endif()
