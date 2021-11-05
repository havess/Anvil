cmake_minimum_required(VERSION 3.0.0)
project(Anvil VERSION 0.1.0)


include(CTest)
enable_testing()

file(GLOB sources 
${CMAKE_SOURCE_DIR}/src/*.cpp
${CMAKE_SOURCE_DIR}/src/*.c
${CMAKE_SOURCE_DIR}/src/imgui/*.cpp
${CMAKE_SOURCE_DIR}/src/Geometry/*.cpp
${CMAKE_SOURCE_DIR}/src/Render/*.cpp
${CMAKE_SOURCE_DIR}/src/UI/*.cpp)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
find_package(GLFW3 REQUIRED)
message(STATUS "GLFW3 included at ${GLFW3_INCLUDE_DIR} with lib at ${GLFW3_LIBRARY}")

find_package(GLM REQUIRED)
message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")

set(LIBS glfw3 opengl32)

include_directories(includes)
link_directories(lib)
add_executable(${PROJECT_NAME} ${sources})
set_target_properties(${PROJECT_NAME} PROPERTIES
            CXX_STANDARD 17
            CXX_EXTENSIONS OFF)
target_link_libraries(${PROJECT_NAME} ${LIBS})

file(GLOB SHADERS 
"src/shaders/*.glsl"
"src/shaders/*.vs"
"src/shaders/*.gs"
"src/shaders/*.fs")
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SHADERS} $<TARGET_FILE_DIR:${PROJECT_NAME}>)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
