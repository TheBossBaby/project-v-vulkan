#pragma once

#include <vulkan/vulkan.h>

#include <span>
#include <vector>

namespace projectv
{
    namespace vulkan::descriptor
    {
        /**
         * @brief RAII wrapper around a VkDescriptorPool.
         *
         * Owns a VkDescriptorPool handle and the VkDevice it was created from,
         * and guarantees the pool is destroyed via vkDestroyDescriptorPool when
         * the wrapper goes out of scope. Destroying the pool also frees every
         * descriptor set allocated from it. Non-copyable, move-only. Usually
         * created through builder::VulkanDescriptorPoolBuilder.
         *
         * The VkDevice is stored as a raw handle, not owned. The pool must be
         * destroyed before that device: when both are members of one class,
         * declare the device first so reverse-order destruction destroys the
         * pool first.
         */
        class VulkanDescriptorPool
        {
            public:
                /**
                 * @brief Constructs an empty wrapper with no owned pool.
                 *
                 * Useful for deferred initialization; the object is invalid
                 * (operator bool() returns false) until move-assigned from
                 * a valid instance.
                 */
                VulkanDescriptorPool() = default;

                /// Non-copyable: a VkDescriptorPool has single-owner destruction semantics.
                VulkanDescriptorPool(const VulkanDescriptorPool&) = delete;

                /// Non-copyable: a VkDescriptorPool has single-owner destruction semantics.
                VulkanDescriptorPool& operator=(const VulkanDescriptorPool&) = delete;

                /**
                 * @brief Move-constructs, transferring ownership from another wrapper.
                 * @param other Source wrapper; left in an empty, valid-to-destroy state.
                 */
                VulkanDescriptorPool(VulkanDescriptorPool&& other) noexcept;

                /**
                 * @brief Move-assigns, destroying any currently owned pool and
                 *        transferring ownership from another wrapper.
                 * @param other Source wrapper; left in an empty, valid-to-destroy state.
                 * @return Reference to this object.
                 */
                VulkanDescriptorPool& operator=(VulkanDescriptorPool&& other) noexcept;

                /**
                 * @brief Creates a VkDescriptorPool and takes ownership of it.
                 *
                 * @param device     Logical device used to create the pool.
                 *                   Stored internally and reused for allocation
                 *                   and destruction.
                 * @param createInfo Pool creation parameters, as passed to
                 *                   vkCreateDescriptorPool.
                 *
                 * @throws std::runtime_error if vkCreateDescriptorPool fails.
                 */
                VulkanDescriptorPool(VkDevice device, const VkDescriptorPoolCreateInfo& createInfo);

                /**
                 * @brief Destroys the owned pool, if any, along with every
                 *        descriptor set allocated from it.
                 */
                ~VulkanDescriptorPool();

                /**
                 * @brief Allocates one descriptor set per layout from this pool.
                 *
                 * The returned sets are owned by the pool: they are not freed
                 * individually and become invalid when the pool is destroyed.
                 * The wrapper must own a valid pool.
                 *
                 * If @p layouts is empty, Vulkan is not called: a warning is
                 * logged and an empty vector is returned.
                 *
                 * Not thread-safe: the pool must be externally synchronized.
                 *
                 * @param layouts Layouts of the sets to allocate; the same layout
                 *                may appear more than once.
                 * @return Descriptor set handles, in the same order as @p layouts,
                 *         or an empty vector if @p layouts is empty.
                 *
                 * @throws std::runtime_error if vkAllocateDescriptorSets fails,
                 *         e.g. with VK_ERROR_OUT_OF_POOL_MEMORY when the pool's
                 *         set limit or descriptor capacity is used up.
                 */
                [[nodiscard]] std::vector<VkDescriptorSet> allocate(std::span<const VkDescriptorSetLayout> layouts);

                /**
                 * @brief Returns the underlying VkDescriptorPool handle.
                 * @return The owned handle, or VK_NULL_HANDLE if none is owned.
                 */
                [[nodiscard]] VkDescriptorPool handle() const noexcept;

                /**
                 * @brief Checks whether this wrapper owns a valid pool.
                 * @return true if a descriptor pool is owned, false otherwise.
                 */
                explicit operator bool() const noexcept;
            private:
                /**
                 * @brief Destroys the owned pool and resets internal state
                 *        to VK_NULL_HANDLE. Safe to call multiple times.
                 */
                void destroy();
            private:
                /// Logical device used to create/destroy m_descriptorPool.
                VkDevice m_logicalDevice = VK_NULL_HANDLE;

                /// Owned descriptor pool handle.
                VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
        };
    }
}
