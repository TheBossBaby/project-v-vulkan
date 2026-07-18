#pragma once

#include <vulkan/vulkan.h>

namespace projectv
{
    namespace vulkan::window
    {
        class VulkanSwapchain
        {
        public:
            VulkanSwapchain();

            ~VulkanSwapchain();
        private:
            VkSwapchainKHR swapchain;    
        };
    }
}