#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace projectv
{
    namespace vulkan
    {
        namespace builder
        {
            /**
             * @brief Builder for a single VkDescriptorSetLayoutBinding.
             *
             * Configures one binding slot of a descriptor set layout through a
             * fluent API. The resulting struct is passed to
             * VulkanDescriptorSetLayoutBuilder::addLayoutBinding().
             *
             * Descriptor type and stage flags start in a "not set" state
             * (VK_DESCRIPTOR_TYPE_MAX_ENUM and 0). This builder does not check
             * them itself; VulkanDescriptorSetLayoutBuilder::build() rejects
             * bindings that still have them unset.
             */
            class VulkanDescriptorSetLayoutBindingBuilder
            {
                public:
                    /**
                     * @brief Assembles the configured state into a
                     *        VkDescriptorSetLayoutBinding.
                     *
                     * Immutable samplers are not supported yet, so
                     * pImmutableSamplers is always nullptr.
                     *
                     * @return The binding description, ready to add to a
                     *         VulkanDescriptorSetLayoutBuilder.
                     */
                    [[nodiscard]]
                    VkDescriptorSetLayoutBinding build();

                    /**
                     * @brief Sets the binding number of this slot.
                     *
                     * Must match the `binding` qualifier in the shader and be
                     * unique within the descriptor set layout.
                     *
                     * @param binding Binding index. Defaults to 0.
                     * @return Reference to this builder, for chaining.
                     */
                    VulkanDescriptorSetLayoutBindingBuilder& setBinding(uint32_t binding);

                    /**
                     * @brief Sets the type of descriptor bound at this slot.
                     *
                     * Required: VulkanDescriptorSetLayoutBuilder::build() throws
                     * if this is never called.
                     *
                     * @param descriptorType Descriptor type, e.g.
                     *                       VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER.
                     * @return Reference to this builder, for chaining.
                     */
                    VulkanDescriptorSetLayoutBindingBuilder& setDescriptorType(VkDescriptorType descriptorType);

                    /**
                     * @brief Sets the number of descriptors in this binding.
                     *
                     * A value greater than 1 makes the binding an array in the
                     * shader. A value of 0 reserves the binding number without
                     * using it; stage flags are then ignored.
                     *
                     * @param descriptorCount Number of descriptors. Defaults to 1.
                     * @return Reference to this builder, for chaining.
                     */
                    VulkanDescriptorSetLayoutBindingBuilder& setDescriptorCount(uint32_t descriptorCount);

                    /**
                     * @brief Sets the shader stages that can access this binding.
                     *
                     * Required when the descriptor count is greater than 0:
                     * VulkanDescriptorSetLayoutBuilder::build() throws if the
                     * flags are left at 0.
                     *
                     * @param stageFlags VkShaderStageFlagBits bitmask, e.g.
                     *                   VK_SHADER_STAGE_VERTEX_BIT.
                     * @return Reference to this builder, for chaining.
                     */
                    VulkanDescriptorSetLayoutBindingBuilder& setStageFlags(VkShaderStageFlags stageFlags);

                private:
                    /// Binding index within the descriptor set.
                    uint32_t m_binding = 0;

                    /// Number of descriptors in the binding (array size).
                    uint32_t m_descriptorCount = 1;

                    /// Descriptor type; VK_DESCRIPTOR_TYPE_MAX_ENUM means "not set".
                    VkDescriptorType m_descriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM;

                    /// Shader stages that access the binding; 0 means "not set".
                    VkShaderStageFlags m_stageFlags = 0;
            };
        }
    }
}
