#pragma once

#include <vulkan/vulkan.h>

#include <vulkan/rendererResources/vulkanBuffer.hpp>
namespace projectv
{
    namespace vulkan
    {
        struct FrameUniformSet
        {
            rendererResources::VulkanBuffer uniformBuffer;

            VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
        };
    }
}