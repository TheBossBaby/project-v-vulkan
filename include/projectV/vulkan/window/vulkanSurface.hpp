#pragma once

#include <vulkan/vulkan.h>

namespace projectv
{
    namespace vulkan::window
    {
        class VulkanSurface
        {
        public:
            VulkanSurface();

            ~VulkanSurface();
        private:
            VkSurfaceKHR surface;    
        };
    }
}