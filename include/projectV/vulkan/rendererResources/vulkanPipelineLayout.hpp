#pragma once

#include <vulkan/vulkan.h>

namespace projectv
{
    namespace vulkan
    {
        namespace rendererResources
        {
            /**
             * @brief RAII wrapper around a VkPipelineLayout.
             *
             * Owns a VkPipelineLayout handle and the VkDevice it was created from,
             * and guarantees the pipeline layout is destroyed via vkDestroyPipelineLayout
             * when the wrapper goes out of scope. Non-copyable, move-only.
             */
            class VulkanPipelineLayout
            {
            public:
                /**
                 * @brief Constructs an empty wrapper with no owned pipeline layout.
                 *
                 * Useful for deferred initialization; the object is invalid
                 * (operator bool() returns false) until move-assigned from
                 * a valid instance.
                 */
                VulkanPipelineLayout() = default;

                /**
                 * @brief Creates a VkPipelineLayout and takes ownership of it.
                 *
                 * @param device     Logical device used to create the pipeline layout.
                 *                   Stored internally and reused for destruction.
                 * @param createInfo Pipeline layout creation parameters, as passed to
                 *                   vkCreatePipelineLayout.
                 *
                 * @throws std::runtime_error if vkCreatePipelineLayout fails.
                 */
                VulkanPipelineLayout(VkDevice device, const VkPipelineLayoutCreateInfo& createInfo);

                /**
                 * @brief Destroys the owned pipeline layout, if any.
                 */
                ~VulkanPipelineLayout();

                /// Non-copyable: a VkPipelineLayout has single-owner destruction semantics.
                VulkanPipelineLayout(const VulkanPipelineLayout&) = delete;

                /// Non-copyable: a VkPipelineLayout has single-owner destruction semantics.
                VulkanPipelineLayout& operator=(const VulkanPipelineLayout&) = delete;

                /**
                 * @brief Move-constructs, transferring ownership from another wrapper.
                 * @param other Source wrapper; left in an empty, valid-to-destroy state.
                 */
                VulkanPipelineLayout(VulkanPipelineLayout&& other) noexcept;

                /**
                 * @brief Move-assigns, destroying any currently owned pipeline layout
                 *        and transferring ownership from another wrapper.
                 * @param other Source wrapper; left in an empty, valid-to-destroy state.
                 * @return Reference to this object.
                 */
                VulkanPipelineLayout& operator=(VulkanPipelineLayout&& other) noexcept;

                /**
                 * @brief Returns the underlying VkPipelineLayout handle.
                 * @return The owned handle, or VK_NULL_HANDLE if none is owned.
                 */
                VkPipelineLayout handle() const noexcept;

                /**
                 * @brief Checks whether this wrapper owns a valid pipeline layout.
                 * @return true if a pipeline layout is owned, false otherwise.
                 */
                explicit operator bool() const noexcept;

            private:
                /**
                 * @brief Destroys the owned pipeline layout and resets internal state
                 *        to VK_NULL_HANDLE. Safe to call multiple times.
                 */
                void destroy() noexcept;

                /// Logical device used to create/destroy m_pipelineLayout.
                VkDevice          m_logicalDevice  = VK_NULL_HANDLE;

                /// Owned pipeline layout handle.
                VkPipelineLayout  m_pipelineLayout = VK_NULL_HANDLE;
            };
        }
    }
}
