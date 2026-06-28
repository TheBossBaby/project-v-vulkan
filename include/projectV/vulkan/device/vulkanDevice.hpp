#pragma once

#include <vulkan/vulkan.h>

namespace projectv
{
    namespace vulkan::device
    {
        class VulkanDevice
        {
        public:
            VulkanDevice();
            
            ~VulkanDevice();
        private:
           VkDevice logicalDevice;
        };        
    }
}