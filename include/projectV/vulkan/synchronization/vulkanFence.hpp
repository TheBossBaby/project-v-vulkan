#pragma once

#include <vulkan/vulkan.h>

namespace projectv
{
    namespace vulkan::sync
    {
        /**
         * @brief RAII wrapper around a Vulkan fence.
         *
         * A VulkanFence owns its VkFence and destroys it when the wrapper is
         * destroyed or explicitly via destroy(). The wrapper is move-only;
         * ownership of the handle transfers on move.
         *
         * The fence is not created on construction. Call create() with a
         * valid logical device before using it.
         */
        class VulkanFence
        {
        public:
            /**
             * @brief Constructs an empty wrapper that owns no fence.
             */
            VulkanFence() = default;
            
            VulkanFence(const VulkanFence&) = delete;
            
            VulkanFence& operator=(const VulkanFence&) = delete;
            
            /**
             * @brief Takes ownership of another fence's handle.
             *
             * @param other Fence to move from. Left empty after the move.
             */
            VulkanFence(VulkanFence&& other) noexcept;

            /**
             * @brief Destroys the currently owned fence, then takes ownership
             *        of another fence's handle.
             *
             * Self-assignment is a no-op.
             *
             * @param other Fence to move from. Left empty after the move.
             * @return Reference to this fence.
             */
            VulkanFence& operator=(VulkanFence&& other) noexcept;

            /**
             * @brief Destroys the owned fence, if any.
             */
            ~ VulkanFence();

            /**
             * @brief Creates the Vulkan fence on the given device.
             *
             * Any fence already owned by this wrapper is destroyed first.
             *
             * @param device Logical device that will own the fence. Must
             *               outlive this fence.
             * @param startSignaled If true, the fence is created in the
             *                      signaled state so the first wait() returns
             *                      immediately.
             * @throws std::runtime_error If vkCreateFence fails.
             */
            void create(VkDevice device, bool startSignaled);

            /**
             * @brief Destroys the fence and resets the wrapper to empty.
             *
             * Safe to call multiple times or on a wrapper that was never
             * created. The fence must not be in use by any pending queue
             * submission when this is called.
             */
            void destroy();

            /**
             * @brief Blocks the CPU until the fence is signaled.
             *
             * Waits with no timeout. The fence must have been created.
             *
             * @throws std::runtime_error If vkWaitForFences fails.
             */
            void wait();

            /**
             * @brief Resets the fence to the unsignaled state.
             *
             * The fence must not be in use by any pending queue submission.
             *
             * @throws std::runtime_error If vkResetFences fails.
             */
            void reset();

            /**
             * @brief Returns the underlying Vulkan fence handle.
             *
             * The handle is borrowed and remains valid only while this
             * wrapper owns it.
             *
             * @return The wrapped VkFence, or VK_NULL_HANDLE if empty.
             */
            VkFence handle() const noexcept;

        private:

            /// Logical device the fence was created on.
            VkDevice device = VK_NULL_HANDLE;

            /// Owned Vulkan fence handle.
            VkFence fence = VK_NULL_HANDLE;
        };
    }
}