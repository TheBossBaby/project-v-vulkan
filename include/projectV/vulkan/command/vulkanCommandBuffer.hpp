#pragma once

#include <vulkan/vulkan.h>

namespace projectv::vulkan::command
{
    class VulkanCommandPool;

    /**
     * @brief Lightweight wrapper around a Vulkan command buffer.
     *
     * A VulkanCommandBuffer does not own the underlying Vulkan allocation.
     * Command buffers are allocated from a VulkanCommandPool, and their
     * lifetime is managed entirely by that pool.
     *
     * Destroying the associated VulkanCommandPool invalidates every
     * VulkanCommandBuffer allocated from it.
     */
    class VulkanCommandBuffer
    {
        /**
         * @brief Only a VulkanCommandPool may allocate command buffers.
         */
        friend class VulkanCommandPool;

    public:
        VulkanCommandBuffer(const VulkanCommandBuffer&) = delete;
        VulkanCommandBuffer& operator=(const VulkanCommandBuffer&) = delete;

        VulkanCommandBuffer(VulkanCommandBuffer&&) noexcept = default;
        VulkanCommandBuffer& operator=(VulkanCommandBuffer&&) noexcept = default;

        ~VulkanCommandBuffer() = default;

        /**
         * @brief Begins recording commands into this command buffer.
         *
         * The command buffer must not already be in the recording state.
         */
        void begin();

        /**
         * @brief Ends command recording.
         *
         * After this call succeeds, the command buffer is ready for queue
         * submission.
         */
        void end();

        /**
         * @brief Returns the underlying Vulkan command buffer handle.
         *
         * This handle is borrowed and remains valid only while the owning
         * VulkanCommandPool exists.
         *
         * @return The wrapped VkCommandBuffer handle.
         */
        [[nodiscard]]
        VkCommandBuffer handle() const noexcept;

    private:
        /**
         * @brief Constructs a wrapper around an allocated Vulkan command buffer.
         *
         * Only VulkanCommandPool may construct instances.
         *
         * @param handle Valid Vulkan command buffer handle.
         */
        explicit VulkanCommandBuffer(VkCommandBuffer handle);

    private:
        /// Wrapped Vulkan command buffer handle.
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    };
}