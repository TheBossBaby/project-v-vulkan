#pragma once

#include <vulkan/vulkan.h>


namespace projectv
{
    namespace vulkan::command
    {
        class VulkanCommandPool
        {
        public:
            /**
            * @brief Destroy the Vulkan Command Pool object
            * 
            */
            ~VulkanCommandPool();

            /**
            * @brief 
            * 
            * @param inDevice 
            * @param inQueueFamilyIndex 
            */
            void create( VkDevice inDevice, uint32_t inQueueFamilyIndex);

            /**
            * @brief 
            * 
            * @return 
            */
            VkCommandPool handle() const noexcept;

            /**
            * @brief 
            * 
            * @param level 
            * @return 
            */
            [[nodiscard]] class VulkanCommandBuffer allocate(VkCommandBufferLevel inLevel);
        private:
            /**
            * @brief 
            * 
            */
            void destroy();
            
        private:
            VkDevice device = VK_NULL_HANDLE;

            VkCommandPool commandPool = VK_NULL_HANDLE;
        };
    }
}