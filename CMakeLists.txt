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

message(STATUS "Vulkan include dir: ${Vulkan_INCLUDE_DIRS}")
message(STATUS "Vulkan libraries: ${Vulkan_LIBRARIES}")

# Vulkan renderer library (no main)
add_library(projectv_vulkan STATIC  
    include/projectV/vulkan/vulkanRenderer.hpp
    include/projectV/vulkan/vulkanFactory.hpp
    include/projectV/vulkan/vulkanRenderPass.hpp
    include/projectV/vulkan/rendererResources.hpp

    include/projectV/vulkan/builder/vulkanRenderPassBuilder.hpp
    include/projectV/vulkan/builder/vulkanFramebufferBuilder.hpp
    include/projectV/vulkan/builder/vulkanShaderModuleBuilder.hpp
    include/projectV/vulkan/builder/vulkanPipelineLayoutBuilder.hpp

    include/projectV/vulkan/config/vulkanConfig.hpp

    include/projectV/vulkan/command/vulkanCommandBuffer.hpp
    include/projectV/vulkan/command/vulkanCommandPool.hpp

    include/projectV/vulkan/device/vulkanDevice.hpp
    include/projectV/vulkan/device/vulkanInstance.hpp
    include/projectV/vulkan/device/vulkanPhysicalDevice.hpp
    include/projectV/vulkan/device/vulkanQueue.hpp

    include/projectV/vulkan/rendererResources/vulkanFramebuffer.hpp
    include/projectV/vulkan/rendererResources/vulkanShaderModule.hpp
    include/projectV/vulkan/rendererResources/vulkanPipelineLayout.hpp

    include/projectV/vulkan/synchronization/vulkanFence.hpp

    include/projectV/vulkan/types/queueFamilies.hpp

    include/projectV/vulkan/util/vulkanCheck.hpp
    include/projectV/vulkan/util/windowExtension.hpp
    include/projectV/vulkan/util/glfwWindowExtension.hpp

    include/projectV/vulkan/window/vulkanSurface.hpp
    include/projectV/vulkan/window/vulkanSwapchain.hpp
    include/projectV/vulkan/window/VulkanImageView.hpp
    include/projectV/vulkan/window/windowSurface.hpp
    include/projectV/vulkan/window/glfwWindowSurface.hpp

    src/vulkanRenderer.cpp
    src/vulkanRenderPass.cpp
    src/rendererResources.cpp

    src/builder/vulkanRenderPassBuilder.cpp
    src/builder/vulkanFramebufferBuilder.cpp
    src/builder/vulkanShaderModuleBuilder.cpp
    src/builder/vulkanPipelineLayoutBuilder.cpp

    src/command/vulkanCommandBuffer.cpp
    src/command/vulkanCommandPool.cpp
    
    src/device/vulkanDevice.cpp
    src/device/vulkanInstance.cpp
    src/device/vulkanPhysicalDevice.cpp
    src/device/vulkanQueue.cpp

    src/rendererResources/vulkanFramebuffer.cpp
    src/rendererResources/vulkanShaderModule.cpp
    src/rendererResources/vulkanPipelineLayout.cpp

    src/synchronization/vulkanFence.cpp

    src/util/glfwWindowExtension.cpp

    src/window/vulkanSurface.cpp
    src/window/vulkanSwapchain.cpp
    src/window/VulkanImageView.cpp
    src/window/windowSurface.cpp
    src/window/glfwWindowSurface.cpp
)

target_include_directories(projectv_vulkan
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/projectV
        ${CMAKE_CURRENT_SOURCE_DIR}/include/projectV/vulkan
        ${CMAKE_CURRENT_SOURCE_DIR}/include/projectV/vulkan/config
        ${CMAKE_CURRENT_SOURCE_DIR}/include/projectV/vulkan/command
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