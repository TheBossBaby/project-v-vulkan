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

            /**
             * @brief Returns a command buffer to this pool.
             *
             * The command buffer must have been allocated from this pool and
             * must not be pending execution on the GPU, so wait on its fence
             * first. Afterwards @p commandBuffer holds VK_NULL_HANDLE and must
             * not be recorded into. Freeing a wrapper that already holds
             * VK_NULL_HANDLE does nothing.
             *
             * @param commandBuffer Command buffer to free; reset to VK_NULL_HANDLE.
             */
            void free(VulkanCommandBuffer& commandBuffer);

            /**
             * @brief Resets a command buffer to the initial state so it can be
             *        recorded again.
             *
             * Allowed because this pool is created with
             * VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT. The command
             * buffer must have been allocated from this pool and must not be
             * pending execution on the GPU. VulkanCommandBuffer::begin() also
             * resets it implicitly, so an explicit reset is mainly useful to
             * discard recorded commands or to return their memory early.
             *
             * @param commandBuffer Command buffer to reset.
             * @param flags         0 (default), or
             *                      VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT
             *                      to also return its memory to the pool.
             *
             * @throws std::runtime_error if vkResetCommandBuffer fails.
             */
            void reset(VulkanCommandBuffer& commandBuffer, VkCommandBufferResetFlags flags = 0);
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