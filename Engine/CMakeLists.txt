cmake_minimum_required(VERSION 3.0.0)

include(CTest)
enable_testing()

file(GLOB sources 
${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
${CMAKE_CURRENT_SOURCE_DIR}/src/imgui/*.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/Geometry/*.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/Render/*.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/UI/*.cpp)

set(LIBRARY_NAME Engine)
include_directories(../includes)
add_library(${LIBRARY_NAME} STATIC ${sources})
set_target_properties(${LIBRARY_NAME} PROPERTIES
            CXX_STANDARD 17
            CXX_EXTENSIONS OFF)

