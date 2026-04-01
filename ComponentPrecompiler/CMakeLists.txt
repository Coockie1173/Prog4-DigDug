cmake_minimum_required(VERSION 3.26)
project(ComponentPrecompiler)

add_executable(ComponentPrecompiler
  main.cpp
  ComponentParser.cpp
)

target_compile_features(ComponentPrecompiler PUBLIC cxx_std_20)

if(MSVC)
  target_compile_options(ComponentPrecompiler PRIVATE /W4)
else()
  target_compile_options(ComponentPrecompiler PRIVATE -Wall -Wextra)
endif()
