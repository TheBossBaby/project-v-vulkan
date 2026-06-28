#pragma once

#include <vulkan/vulkan.h>

namespace projectv
{
    namespace vulkan::device
    {
        class VulkanPhysicalDevice
        {
        public:
            VulkanPhysicalDevice(/* args */);

            ~VulkanPhysicalDevice();
        private:
            VkPhysicalDevice physicalDevice;        
    };        
    }
}