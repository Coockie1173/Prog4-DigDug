add_library(minigin_shared STATIC
    src/MBINSerializer.cpp
    src/SceneDeserializer.cpp
)

target_include_directories(minigin_shared PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}
)

target_compile_features(minigin_shared PUBLIC cxx_std_20)

# Link required dependencies
find_package(glm CONFIG QUIET)
if(NOT glm_FOUND)
  # glm will be fetched by the main CMakeLists.txt
endif()

target_include_directories(minigin_shared PUBLIC
    ${glm_SOURCE_DIR}
)

target_link_libraries(minigin_shared PUBLIC
    glm::glm
    SDL3::SDL3
)

if(MSVC)
  target_compile_options(minigin_shared PRIVATE /W4 /WX)
else()
  target_compile_options(minigin_shared PRIVATE -Wall -Wextra -Werror)
endif()
