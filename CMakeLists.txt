cmake_minimum_required(VERSION 3.0.0)
project(Anvil VERSION 0.1.0)

include_directories(includes)
add_subdirectory(Engine)
add_subdirectory(Apps/Basic)
add_subdirectory(Apps/Lines)
add_subdirectory(Apps/ShaderEditor)