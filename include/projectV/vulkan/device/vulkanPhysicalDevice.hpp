#pragma once

#include <vulkan/vulkan.h>

#include <vulkan/types/queueFamilies.hpp>

#include <vector>
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
           * @param instance 
           * @param surface 
           * @param requiredDeviceExtensionList 
            */
            void select(VkInstance instance, VkSurfaceKHR surface, const std::vector<const char*>& requiredDeviceExtensionList);

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
            bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface, const std::vector<const char*>& requiredDeviceExtensionList);

            /**
            * @brief 
            * 
            * @param device 
            * @return 
            */
            vulkan::types::QueueFamilies findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
            
            /**
            * @brief Function to check if physical device supports required Device Extensions or not.
            * 
            * @param device Handle to physical device
            * @return True if all required devices extensions are supported else false.
             */
            bool supportsDeviceExtensions(VkPhysicalDevice device, const std::vector<const char*>& deviceExtensions);
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