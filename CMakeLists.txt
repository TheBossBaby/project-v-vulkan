cmake_minimum_required(VERSION 3.20)

project(projectv_vulkan LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Only add engine submodule if the target doesn't already exist
if (NOT TARGET projectv_engine)
    add_subdirectory(external/project-v-engine)
endif()

find_package(Vulkan REQUIRED)

# Vulkan renderer library (no main)
add_library(projectv_vulkan STATIC
    src/vulkanRenderer.cpp
    
    include/projectV/vulkan/vulkanRenderer.hpp

    include/projectV/vulkan/config/vulkanConfig.hpp

    include/projectV/vulkan/device/vulkanDevice.hpp
    include/projectV/vulkan/device/vulkanInstance.hpp
    include/projectV/vulkan/device/vulkanPhysicalDevice.hpp
    
    include/projectV/vulkan/util/vulkanCheck.hpp

    include/projectV/vulkan/window/vulkanSurface.hpp
    include/projectV/vulkan/window/vulkanSwapchain.hpp

    src/device/vulkanDevice.cpp
    src/device/vulkanInstance.cpp
    src/device/vulkanPhysicalDevice.cpp

    src/window/vulkanSurface.cpp
    src/window/vulkanSwapchain.cpp
)

target_include_directories(projectv_vulkan
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/projectV
        ${CMAKE_CURRENT_SOURCE_DIR}/include/projectV/vulkan
        ${CMAKE_CURRENT_SOURCE_DIR}/include/projectV/vulkan/config
        ${CMAKE_CURRENT_SOURCE_DIR}/include/projectV/vulkan/device
        ${CMAKE_CURRENT_SOURCE_DIR}/include/projectV/vulkan/util
        ${CMAKE_CURRENT_SOURCE_DIR}/include/projectV/vulkan/window/
)

target_link_libraries(projectv_vulkan
    PUBLIC
        projectv_engine    # depend on engine API

    PRIVATE
        Vulkan::Vulkan
)