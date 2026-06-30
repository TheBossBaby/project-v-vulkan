#pragma once

#include <vulkan/vulkan.h>

namespace projectv
{
    namespace vulkan::types { struct QueueFamilies ;}
    
    namespace vulkan::device
    {
        class VulkanPhysicalDevice
        {
        public:
            /**
            * @brief Construct a new Vulkan Physical Device object
            * 
             */
            VulkanPhysicalDevice();

            /**
            * @brief Destroy the Vulkan Physical Device object
            * 
             */
            ~VulkanPhysicalDevice();

            /**
            * @brief 
            * 
             */
            void select(VkInstance instance);

            /**
            * @brief 
            * 
            * @return 
             */
            VkPhysicalDevice handle() const noexcept;
        private:
            /**
            * @brief 
            * 
            * @param device 
            * @return 
            * @return 
            */
            bool isDeviceSuitable(VkPhysicalDevice device);

            vulkan::types::QueueFamilies findQueueFamilies(VkPhysicalDevice device);
        private:
            /**
            * @brief 
            * 
            */
            VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;        
    };        
    }
}