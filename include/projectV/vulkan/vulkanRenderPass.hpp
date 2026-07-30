#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <utility>

namespace projectv
{
    namespace vulkan
    {
        /**
         * @brief RAII wrapper around a VkRenderPass.
         *
         * Owns a VkRenderPass handle and the VkDevice it was created from,
         * and guarantees the render pass is destroyed via vkDestroyRenderPass
         * when the wrapper goes out of scope. Non-copyable, move-only.
         */
        class VulkanRenderPass
        {
        public:
            /**
             * @brief Constructs an empty wrapper with no owned render pass.
             *
             * Useful for deferred initialization; the object is invalid
             * (operator bool() returns false) until move-assigned from
             * a valid instance.
             */
            VulkanRenderPass() = default;

            /**
             * @brief Creates a VkRenderPass and takes ownership of it.
             *
             * @param device     Logical device used to create the render pass.
             *                   Stored internally and reused for destruction.
             * @param createInfo Render pass creation parameters, as passed to
             *                   vkCreateRenderPass.
             *
             * @throws std::runtime_error if vkCreateRenderPass fails.
             */
            VulkanRenderPass(VkDevice device, const VkRenderPassCreateInfo& createInfo);

            /**
             * @brief Destroys the owned render pass, if any.
             */
            ~VulkanRenderPass();

            /// Non-copyable: a VkRenderPass has single-owner destruction semantics.
            VulkanRenderPass(const VulkanRenderPass&) = delete;

            /// Non-copyable: a VkRenderPass has single-owner destruction semantics.
            VulkanRenderPass& operator=(const VulkanRenderPass&) = delete;

            /**
             * @brief Move-constructs, transferring ownership from another wrapper.
             * @param other Source wrapper; left in an empty, valid-to-destroy state.
             */
            VulkanRenderPass(VulkanRenderPass&& other) noexcept;

            /**
             * @brief Move-assigns, destroying any currently owned render pass
             *        and transferring ownership from another wrapper.
             * @param other Source wrapper; left in an empty, valid-to-destroy state.
             * @return Reference to this object.
             */
            VulkanRenderPass& operator=(VulkanRenderPass&& other) noexcept;

            /**
             * @brief Returns the underlying VkRenderPass handle.
             * @return The owned handle, or VK_NULL_HANDLE if none is owned.
             */
            VkRenderPass handle() const noexcept;

            /**
             * @brief Checks whether this wrapper owns a valid render pass.
             * @return true if a render pass is owned, false otherwise.
             */
            explicit operator bool() const noexcept;

        private:
            /**
             * @brief Destroys the owned render pass and resets internal state
             *        to VK_NULL_HANDLE. Safe to call multiple times.
             */
            void destroy() noexcept;

            /// Logical device used to create/destroy m_renderPass.
            VkDevice     m_logicalDevice    = VK_NULL_HANDLE;

            /// Owned render pass handle.
            VkRenderPass m_renderPass = VK_NULL_HANDLE;
        };
    }
}