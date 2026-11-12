#pragma once

#include <vulkan/vulkan.h>


namespace projectv
{
    namespace vulkan::descriptor
    {
        /**
         * @brief RAII wrapper around a VkDescriptorSetLayout.
         *
         * Owns a VkDescriptorSetLayout handle and the VkDevice it was created
         * from, and guarantees the layout is destroyed via
         * vkDestroyDescriptorSetLayout when the wrapper goes out of scope.
         * Non-copyable, move-only. Usually created through
         * builder::VulkanDescriptorSetLayoutBuilder.
         */
        class VulkanDescriptorSetLayout
        {
            public:
                /**
                 * @brief Constructs an empty wrapper with no owned layout.
                 *
                 * Useful for deferred initialization; the object is invalid
                 * (operator bool() returns false) until move-assigned from
                 * a valid instance.
                 */
                VulkanDescriptorSetLayout() = default;

                /**
                 * @brief Destroys the owned descriptor set layout, if any.
                 */
                ~VulkanDescriptorSetLayout();

                /// Non-copyable: a VkDescriptorSetLayout has single-owner destruction semantics.
                VulkanDescriptorSetLayout(const VulkanDescriptorSetLayout&) = delete;

                /// Non-copyable: a VkDescriptorSetLayout has single-owner destruction semantics.
                VulkanDescriptorSetLayout& operator=(const VulkanDescriptorSetLayout&) = delete;

                /**
                 * @brief Move-constructs, transferring ownership from another wrapper.
                 * @param other Source wrapper; left in an empty, valid-to-destroy state.
                 */
                VulkanDescriptorSetLayout(VulkanDescriptorSetLayout&& other) noexcept;

                /**
                 * @brief Move-assigns, destroying any currently owned layout and
                 *        transferring ownership from another wrapper.
                 * @param other Source wrapper; left in an empty, valid-to-destroy state.
                 * @return Reference to this object.
                 */
                VulkanDescriptorSetLayout& operator=(VulkanDescriptorSetLayout&& other) noexcept;

                /**
                 * @brief Creates a VkDescriptorSetLayout and takes ownership of it.
                 *
                 * @param device     Logical device used to create the layout.
                 *                   Stored internally and reused for destruction.
                 * @param createInfo Layout creation parameters, as passed to
                 *                   vkCreateDescriptorSetLayout.
                 *
                 * @throws std::runtime_error if vkCreateDescriptorSetLayout fails.
                 */
                VulkanDescriptorSetLayout(VkDevice device, const VkDescriptorSetLayoutCreateInfo &createInfo);

                /**
                 * @brief Returns the underlying VkDescriptorSetLayout handle.
                 * @return The owned handle, or VK_NULL_HANDLE if none is owned.
                 */
                [[nodiscard]]
                VkDescriptorSetLayout handle() const noexcept;

                /**
                 * @brief Checks whether this wrapper owns a valid layout.
                 * @return true if a layout is owned, false otherwise.
                 */
                explicit operator bool() const noexcept;
            private:
                /**
                 * @brief Destroys the owned layout and resets internal state
                 *        to VK_NULL_HANDLE. Safe to call multiple times.
                 */
                void destroy();
            private:
                /// Logical device used to create/destroy m_descriptorSetLayout.
                VkDevice              m_logicalDevice = VK_NULL_HANDLE;

                /// Owned descriptor set layout handle.
                VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
        };
    }
}
