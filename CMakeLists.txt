cmake_minimum_required(VERSION 3.20)

project(projectv_vulkan LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Engine as submodule
add_subdirectory(external/project-v-engine)

# Vulkan renderer library (no main)
add_library(projectv_vulkan STATIC
    src/vulkanRenderer.cpp
    include/projectV/vulkan/vulkanRenderer.hpp
)

target_include_directories(projectv_vulkan
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(projectv_vulkan
    PUBLIC
        projectv_engine    # depend on engine API
)