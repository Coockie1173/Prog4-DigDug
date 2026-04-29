add_library(minigin_engine STATIC
  ${CMAKE_SOURCE_DIR}/Minigin/Debugger.cpp
  ${CMAKE_SOURCE_DIR}/Minigin/EventManager.cpp
  ${CMAKE_SOURCE_DIR}/Minigin/Font.cpp
  ${CMAKE_SOURCE_DIR}/Minigin/GameObject.cpp
  ${CMAKE_SOURCE_DIR}/Minigin/InputManager.cpp
  ${CMAKE_SOURCE_DIR}/Minigin/Minigin.cpp
  ${CMAKE_SOURCE_DIR}/Minigin/Renderer.cpp
  ${CMAKE_SOURCE_DIR}/Minigin/ResourceManager.cpp
  ${CMAKE_SOURCE_DIR}/Minigin/Scene.cpp
  ${CMAKE_SOURCE_DIR}/Minigin/SceneLoader.cpp
  ${CMAKE_SOURCE_DIR}/Minigin/SceneManager.cpp
  ${CMAKE_SOURCE_DIR}/Minigin/TextObject.cpp
  ${CMAKE_SOURCE_DIR}/Minigin/Texture2D.cpp
  ${CMAKE_SOURCE_DIR}/Minigin/Timing.cpp
  ${CMAKE_SOURCE_DIR}/Minigin/ComponentRegistration.cpp
  ${CMAKE_SOURCE_DIR}/Minigin/ComponentFactoryRegistry.cpp
  ${CMAKE_SOURCE_DIR}/Minigin/Components/Component.cpp
  ${CMAKE_SOURCE_DIR}/Minigin/Components/FPSCounterComponent.cpp
  ${CMAKE_SOURCE_DIR}/Minigin/Components/RenderComponent.cpp
  ${CMAKE_SOURCE_DIR}/Minigin/Components/SwappableRenderComponent.cpp
  ${CMAKE_SOURCE_DIR}/Minigin/Components/TextRendererComponent.cpp
  ${CMAKE_SOURCE_DIR}/Minigin/Components/TextureRendererComponent.cpp
  ${CMAKE_SOURCE_DIR}/Minigin/Controller/ControllerManager.cpp
  ${CMAKE_SOURCE_DIR}/Minigin/Sound/SoundSerivceLocator.cpp
  ${CMAKE_SOURCE_DIR}/Minigin/Sound/SoundSystem_SDL.cpp
  "Sound/SoundSystem_Debug.cpp"
)

target_compile_features(minigin_engine PUBLIC cxx_std_20)

target_include_directories(minigin_engine PUBLIC
  ${CMAKE_SOURCE_DIR}/Shared/include
  ${CMAKE_SOURCE_DIR}/Minigin
  ${CMAKE_SOURCE_DIR}/Minigin/Components
  ${CMAKE_SOURCE_DIR}/Minigin/Commands
  ${CMAKE_SOURCE_DIR}/Minigin/Controller
  ${CMAKE_SOURCE_DIR}/Minigin/Sound
  ${glm_SOURCE_DIR}
  ${sdl3_SOURCE_DIR}/include
  ${sdl3_ttf_SOURCE_DIR}/include
  ${sdl3_mixer_SOURCE_DIR}/include
  ${imgui_SOURCE_DIR}
  ${VLD_INCLUDE_DIR}
)

target_link_libraries(minigin_engine PUBLIC
  SDL3::SDL3
  SDL3_ttf::SDL3_ttf
  SDL3_mixer::SDL3_mixer
  glm::glm
  imgui
  minigin_shared
  ${VLD_LIBRARIES}
)

target_compile_definitions(minigin_engine PUBLIC
  $<$<CONFIG:Debug>:DEBUG>
)

if(WIN32)
  find_library(XINPUT_LIBRARY Xinput)
  if(XINPUT_LIBRARY)
    target_link_libraries(minigin_engine PUBLIC ${XINPUT_LIBRARY})
  endif()
endif()

if(MSVC)
  target_compile_options(minigin_engine PRIVATE /W4 /WX)
else()
  target_compile_options(minigin_engine PRIVATE -Wall -Wextra -Werror)
endif()