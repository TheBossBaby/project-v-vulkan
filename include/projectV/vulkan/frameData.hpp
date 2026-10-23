#pragma once

#include <vulkan/command/vulkanCommandBuffer.hpp>
#include <vulkan/synchronization/vulkanFence.hpp>
#include <vulkan/synchronization/vulkanSemaphore.hpp>

namespace projectv
{

    namespace vulkan
    {
        struct FrameData
        {
            command::VulkanCommandBuffer commandBuffer;
            sync::VulkanFence            inFlight;
            sync::VulkanSemaphore        imageAvailableSemaphore;
        };
    }
}
