#pragma once

#include <vulkan/descriptorSet/vulkanDescriptorSetLayout.hpp>

#include <vulkan/vulkan.h>
#include <vector>

namespace projectv
{
    namespace vulkan
    {
        namespace builder
        {
            /**
             * @brief Builder for assembling and creating a VkDescriptorSetLayout.
             *
             * Accumulates binding descriptions through a fluent API, then builds
             * a fully-formed VulkanDescriptorSetLayout via build(). Bindings are
             * copied into the builder's own storage, typically from
             * VulkanDescriptorSetLayoutBindingBuilder::build().
             */
            class VulkanDescriptorSetLayoutBuilder
            {
                public:
                    /**
                     * @brief Appends a single binding description.
                     *
                     * The binding is not checked here; build() checks all
                     * bindings together.
                     *
                     * @param layoutBinding Binding description to append, usually
                     *                      produced by
                     *                      VulkanDescriptorSetLayoutBindingBuilder.
                     * @return Reference to this builder, for chaining.
                     */
                    VulkanDescriptorSetLayoutBuilder& addLayoutBinding(VkDescriptorSetLayoutBinding layoutBinding);

                    /**
                     * @brief Checks the accumulated bindings, assembles them into a
                     *        VkDescriptorSetLayoutCreateInfo and creates the layout.
                     *
                     * A binding with a raw, zero-initialized descriptorType reads
                     * as VK_DESCRIPTOR_TYPE_SAMPLER and passes the type check; only
                     * the VK_DESCRIPTOR_TYPE_MAX_ENUM default of
                     * VulkanDescriptorSetLayoutBindingBuilder counts as "not set".
                     *
                     * @param device Logical device to create the layout on.
                     * @return A VulkanDescriptorSetLayout owning the newly created
                     *         handle.
                     *
                     * @throws std::runtime_error if a binding has no descriptor type
                     *         set, a binding with a non-zero descriptor count has no
                     *         stage flags set, two bindings share the same binding
                     *         index, or descriptor set layout creation fails.
                     */
                    [[nodiscard]]
                    descriptor::VulkanDescriptorSetLayout build(VkDevice device);
                private:
                    /// Accumulated binding descriptions, in the order they were added.
                    std::vector <VkDescriptorSetLayoutBinding> m_LayoutBindingList;
            };
        }
    }
}
