#pragma once

#include <vulkan/vulkan.h>

namespace projectv
{
    namespace vulkan
    {
        namespace rendererResources
        {
            /**
             * @brief RAII wrapper around a VkFramebuffer.
             *
             * Owns a VkFramebuffer handle and the VkDevice it was created from,
             * and guarantees the framebuffer is destroyed via vkDestroyFramebuffer
             * when the wrapper goes out of scope. Non-copyable, move-only.
             */
            class VulkanFramebuffer
            {
            public:
                /**
                 * @brief Constructs an empty wrapper with no owned framebuffer.
                 *
                 * Useful for deferred initialization; the object is invalid
                 * (operator bool() returns false) until move-assigned from
                 * a valid instance.
                 */
                VulkanFramebuffer() = default;

                /**
                 * @brief Creates a VkFramebuffer and takes ownership of it.
                 *
                 * @param device     Logical device used to create the framebuffer.
                 *                   Stored internally and reused for destruction.
                 * @param createInfo Framebuffer creation parameters, as passed to
                 *                   vkCreateFramebuffer.
                 *
                 * @throws std::runtime_error if vkCreateFramebuffer fails.
                 */
                VulkanFramebuffer(VkDevice device, const VkFramebufferCreateInfo& createInfo);

                /**
                 * @brief Destroys the owned framebuffer, if any.
                 */
                ~VulkanFramebuffer();

                /// Non-copyable: a VkFramebuffer has single-owner destruction semantics.
                VulkanFramebuffer(const VulkanFramebuffer&) = delete;

                /// Non-copyable: a VkFramebuffer has single-owner destruction semantics.
                VulkanFramebuffer& operator=(const VulkanFramebuffer&) = delete;

                /**
                 * @brief Move-constructs, transferring ownership from another wrapper.
                 * @param other Source wrapper; left in an empty, valid-to-destroy state.
                 */
                VulkanFramebuffer(VulkanFramebuffer&& other) noexcept;

                /**
                 * @brief Move-assigns, destroying any currently owned framebuffer
                 *        and transferring ownership from another wrapper.
                 * @param other Source wrapper; left in an empty, valid-to-destroy state.
                 * @return Reference to this object.
                 */
                VulkanFramebuffer& operator=(VulkanFramebuffer&& other) noexcept;

                /**
                 * @brief Returns the underlying VkFramebuffer handle.
                 * @return The owned handle, or VK_NULL_HANDLE if none is owned.
                 */
                VkFramebuffer handle() const noexcept;

                /**
                 * @brief Checks whether this wrapper owns a valid framebuffer.
                 * @return true if a framebuffer is owned, false otherwise.
                 */
                explicit operator bool() const noexcept;

            private:
                /**
                 * @brief Destroys the owned framebuffer and resets internal state
                 *        to VK_NULL_HANDLE. Safe to call multiple times.
                 */
                void destroy() noexcept;

                /// Logical device used to create/destroy m_framebuffer.
                VkDevice      m_logicalDevice = VK_NULL_HANDLE;

                /// Owned framebuffer handle.
                VkFramebuffer m_framebuffer   = VK_NULL_HANDLE;
            };
        }
    }
}