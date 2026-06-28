#pragma once

#include <vulkan/vulkan.h>

namespace projectv
{
    namespace vulkan::device
    {
        class VulkanInstance
        {
        public:
            VulkanInstance();
            
            ~VulkanInstance();
        private:
            VkInstance instance;
        };       
    }
}