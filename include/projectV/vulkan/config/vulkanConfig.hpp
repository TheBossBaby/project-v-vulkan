#pragma once
#include <vulkan/vulkan.h>

#include <vector>

namespace projectv
{
    namespace vulkan::config
    {
        const uint32_t VulkanVersion = VK_API_VERSION_1_3;

        const std::vector<const char *> DeviceExtensions = 
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
    }
}