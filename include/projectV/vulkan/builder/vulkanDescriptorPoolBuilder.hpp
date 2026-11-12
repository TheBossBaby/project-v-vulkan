#pragma once

#include <vulkan/vulkan.h>

#include <vulkan/descriptorSet/vulkanDescriptorPool.hpp>

#include <vector>

namespace projectv
{
    namespace vulkan
    {
        namespace builder
        {
            /**
             * @brief Builder for assembling and creating a VkDescriptorPool.
             *
             * Accumulates the pool's set limit, creation flags and per-type
             * descriptor capacities through a fluent API, then builds a
             * fully-formed VulkanDescriptorPool via build(). Pool sizes are
             * copied into the builder's own storage.
             */
            class VulkanDescriptorPoolBuilder
            {
                public:
                    /**
                     * @brief Sets the maximum number of descriptor sets that can be
                     *        allocated from the pool.
                     *
                     * Required: must be greater than 0. build() throws if it is
                     * left at the default of 0.
                     *
                     * @param maxSets Maximum number of sets allocated at once.
                     * @return Reference to this builder, for chaining.
                     */
                    VulkanDescriptorPoolBuilder& setMaxSets(uint32_t maxSets);

                    /**
                     * @brief Adds pool creation flags.
                     *
                     * Flags are combined with a bitwise OR, so calling this more
                     * than once keeps the earlier flags.
                     *
                     * @param flags VkDescriptorPoolCreateFlags bitmask to add, e.g.
                     *              VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT.
                     * @return Reference to this builder, for chaining.
                     */
                    VulkanDescriptorPoolBuilder& addFlags(VkDescriptorPoolCreateFlags flags);

                    /**
                     * @brief Appends the capacity for one descriptor type.
                     *
                     * The count is the total number of descriptors of that type
                     * across all sets allocated from the pool, not per set.
                     * At least one pool size is required, and each count must be
                     * greater than 0; build() checks both.
                     *
                     * @param poolSize Descriptor type and the total number of
                     *                 descriptors of that type the pool can hold.
                     * @return Reference to this builder, for chaining.
                     */
                    VulkanDescriptorPoolBuilder& addPoolSize(VkDescriptorPoolSize poolSize);

                    /**
                     * @brief Assembles all accumulated state into a
                     *        VkDescriptorPoolCreateInfo and creates the pool.
                     *
                     * @param device Logical device to create the pool on.
                     * @return A VulkanDescriptorPool owning the newly created handle.
                     *
                     * @throws std::runtime_error if a pool size has a descriptor
                     *         count of 0, maxSets is 0, no pool size was added,
                     *         or descriptor pool creation fails.
                     */
                    [[nodiscard]]
                    descriptor::VulkanDescriptorPool build(VkDevice device);
                private:
                    /// Maximum number of sets that can be allocated from the pool.
                    uint32_t m_maxSets = 0;

                    /// Accumulated pool creation flags.
                    VkDescriptorPoolCreateFlags m_flags = 0;

                    /// Per-type descriptor capacities, in the order they were added.
                    std::vector<VkDescriptorPoolSize> m_descriptorPoolSizeList;
            };
        }
    }
}
