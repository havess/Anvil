cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
find_package(GLFW3 REQUIRED)
message(STATUS "GLFW3 included at ${GLFW3_INCLUDE_DIR} with lib at ${GLFW3_LIBRARY}")

find_package(GLM REQUIRED)
message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")

find_package(Vulkan REQUIRED FATAL_ERROR)
message(STATUS "Vulkan included at ${Vulkan_FOUND} ${Vulkan_LIBRARY}")

set(LIBS glfw3 vulkan-1 Engine)

set(APP_NAME Basic)
include_directories(../../includes)
link_directories(../../lib ${Vulkan_LIBRARY}/../)
add_executable(${APP_NAME} main.cpp)
set_target_properties(${APP_NAME} PROPERTIES
            CXX_STANDARD 17
            CXX_EXTENSIONS OFF)
target_link_libraries(${APP_NAME} ${LIBS})

file(GLOB SHADERS "${CMAKE_SOURCE_DIR}/shaders/*")

add_custom_command(TARGET ${APP_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SHADERS} $<TARGET_FILE_DIR:${APP_NAME}>)
