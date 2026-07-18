#pragma once

#include <vulkan/vulkan.h>

#include <vulkan/device/vulkanQueue.hpp>

#include <vector>
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
            * 
            * @param physicalDevice Handle of physcial Device
            * @param queueFamilies 
            * @param requiredDeviceExtensionList 
             */
            void create(VkPhysicalDevice physicalDevice, const vulkan::types::QueueFamilies& queueFamilies, const std::vector<const char*>& requiredDeviceExtensionList);
            
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

            VulkanQueue& graphicsQueue();

            VulkanQueue& presentQueue();
        private:
            void acquireQueue(const uint32_t queueFamilyIndex, VulkanQueue& vulkanQueue);
        private:
           VkDevice logicalDevice = VK_NULL_HANDLE;

           VulkanQueue m_graphicsQueue;

           VulkanQueue m_presentQueue;
        };        
    }
}