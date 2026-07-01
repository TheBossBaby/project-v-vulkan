#pragma once

#include <vulkan/vulkan.h>

#include <vulkan/types/queueFamilies.hpp>
namespace projectv
{
    
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
            
            const vulkan::types::QueueFamilies& queueFamilies() const { return queueFamiliesIndex; }
        private:
            /**
            * @brief 
            * 
            * @param device 
            * @return 
            * @return 
            */
            bool isDeviceSuitable(VkPhysicalDevice device);

            /**
            * @brief 
            * 
            * @param device 
            * @return 
            */
            vulkan::types::QueueFamilies findQueueFamilies(VkPhysicalDevice device);
        private:
            /**
            * @brief 
            * 
            */
            VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
            
            /**
            * @brief 
            * 
            */
            vulkan::types::QueueFamilies queueFamiliesIndex;
    };        
    }
}