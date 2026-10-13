#pragma once

#include <vulkan/vulkan.h>

namespace projectv
{
    namespace vulkan::sync
    {
        /**
         * @brief RAII wrapper around a binary Vulkan semaphore.
         *
         * A VulkanSemaphore owns its VkSemaphore and destroys it when the
         * wrapper is destroyed or explicitly via destroy(). The wrapper is
         * move-only; ownership of the handle transfers on move.
         *
         * The semaphore is not created on construction. Call create() with a
         * valid logical device before using handle().
         */
        class VulkanSemaphore
        {
        public:
            /**
             * @brief Constructs an empty wrapper that owns no semaphore.
             */
            VulkanSemaphore() = default;

            VulkanSemaphore(const VulkanSemaphore&) = delete;

            VulkanSemaphore& operator=(const VulkanSemaphore&) = delete;

            /**
             * @brief Takes ownership of another semaphore's handle.
             *
             * @param other Semaphore to move from. Left empty after the move.
             */
            VulkanSemaphore(VulkanSemaphore&& other) noexcept;

            /**
             * @brief Destroys the currently owned semaphore, then takes
             *        ownership of another semaphore's handle.
             *
             * Self-assignment is a no-op.
             *
             * @param other Semaphore to move from. Left empty after the move.
             * @return Reference to this semaphore.
             */
            VulkanSemaphore& operator=(VulkanSemaphore&& other) noexcept;

            /**
             * @brief Destroys the owned semaphore, if any.
             */
            ~ VulkanSemaphore();

            /**
             * @brief Creates the Vulkan semaphore on the given device.
             *
             * Any semaphore already owned by this wrapper is destroyed first.
             *
             * @param device Logical device that will own the semaphore. Must
             *               outlive this semaphore.
             * @throws std::runtime_error If vkCreateSemaphore fails.
             */
            void create(VkDevice device);

            /**
             * @brief Destroys the semaphore and resets the wrapper to empty.
             *
             * Safe to call multiple times or on a wrapper that was never
             * created. The semaphore must not be in use by any pending GPU
             * work when this is called.
             */
            void destroy();

            /**
             * @brief Returns the underlying Vulkan semaphore handle.
             *
             * The handle is borrowed and remains valid only while this
             * wrapper owns it.
             *
             * @return The wrapped VkSemaphore, or VK_NULL_HANDLE if empty.
             */
            VkSemaphore handle() const noexcept;

        private:

            /// Logical device the semaphore was created on.
            VkDevice device = VK_NULL_HANDLE;

            /// Owned Vulkan semaphore handle.
            VkSemaphore semaphore = VK_NULL_HANDLE;
        };
    }
}
