#pragma once

#include <vulkan/vulkan.h>

namespace projectv
{
    namespace vulkan::types { struct QueueFamilies ;}

    namespace vulkan::device
    {
        class VulkanDevice
        {
        public:
            VulkanDevice();
            
            ~VulkanDevice();

            /**
            * @brief Create vulkan logical device.
            */
            void create(VkPhysicalDevice physicalDevice, const vulkan::types::QueueFamilies& queueFamilies);
            
            /**
            * @brief 
            * 
            */
            void destroy();

            /**
            * @brief 
            * 
            * @return 
            */
            VkDevice handle() const noexcept;

            VkQueue graphicsQueueHandle() const noexcept { return graphicsQueue; }
        private:
            void acquireGraphicsQueue(const vulkan::types::QueueFamilies& queueFamilies);
        private:
           VkDevice logicalDevice = VK_NULL_HANDLE;

           VkQueue graphicsQueue = VK_NULL_HANDLE;
        };        
    }
}