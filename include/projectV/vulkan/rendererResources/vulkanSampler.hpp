#pragma once

#include <vulkan/vulkan.h>

namespace projectv
{
    namespace vulkan
    {
        namespace rendererResources
        {
            /**
             * @brief RAII wrapper around a VkSampler.
             *
             * Owns a VkSampler handle and the VkDevice it was created from,
             * and guarantees the sampler is destroyed via vkDestroySampler
             * when the wrapper goes out of scope. Non-copyable, move-only.
             *
             * @note The VkDevice is not owned. The device must outlive the
             *       sampler: when this is a class member, declare it after
             *       the device so it is destroyed first.
             */
            class VulkanSampler
            {
            public:
                /**
                 * @brief Constructs an empty wrapper with no owned sampler.
                 *
                 * Useful for deferred initialization; the object is invalid
                 * (operator bool() returns false) until move-assigned from
                 * a valid instance.
                 */
                VulkanSampler() = default;

                /**
                 * @brief Creates a VkSampler and takes ownership of it.
                 *
                 * @param device     Logical device used to create the sampler.
                 *                   Stored internally and reused for destruction.
                 * @param createInfo Sampler creation parameters, as passed to
                 *                   vkCreateSampler. If anisotropyEnable is
                 *                   VK_TRUE, the samplerAnisotropy device
                 *                   feature must be enabled.
                 *
                 * @throws std::runtime_error if vkCreateSampler fails.
                 */
                VulkanSampler(VkDevice device, const VkSamplerCreateInfo& createInfo);

                /**
                 * @brief Destroys the owned sampler, if any.
                 */
                ~VulkanSampler();

                /// Non-copyable: a VkSampler has single-owner destruction semantics.
                VulkanSampler(const VulkanSampler&) = delete;

                /// Non-copyable: a VkSampler has single-owner destruction semantics.
                VulkanSampler& operator=(const VulkanSampler&) = delete;

                /**
                 * @brief Move-constructs, transferring ownership from another wrapper.
                 * @param other Source wrapper; left in an empty, valid-to-destroy state.
                 */
                VulkanSampler(VulkanSampler&& other) noexcept;

                /**
                 * @brief Move-assigns, destroying any currently owned sampler
                 *        and transferring ownership from another wrapper.
                 * @param other Source wrapper; left in an empty, valid-to-destroy state.
                 * @return Reference to this object.
                 */
                VulkanSampler& operator=(VulkanSampler&& other) noexcept;

                /**
                 * @brief Returns the underlying VkSampler handle.
                 * @return The owned handle, or VK_NULL_HANDLE if none is owned.
                 */
                [[nodiscard]]
                VkSampler handle() const noexcept;

                /**
                 * @brief Checks whether this wrapper owns a valid sampler.
                 * @return true if a sampler is owned, false otherwise.
                 */
                explicit operator bool() const noexcept;

            private:
                /**
                 * @brief Destroys the owned sampler and resets internal state
                 *        to VK_NULL_HANDLE. Safe to call multiple times.
                 */
                void destroy() noexcept;

                /// Logical device used to create/destroy m_sampler.
                VkDevice  m_logicalDevice = VK_NULL_HANDLE;

                /// Owned sampler handle.
                VkSampler m_sampler       = VK_NULL_HANDLE;
            };
        }
    }
}
