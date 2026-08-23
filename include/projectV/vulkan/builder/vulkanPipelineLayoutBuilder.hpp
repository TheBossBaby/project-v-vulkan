#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/rendererResources/vulkanPipelineLayout.hpp>

#include <cstdint>
#include <span>
#include <vector>

namespace projectv
{
    namespace vulkan::builder
    {
        /**
         * @brief Builder for assembling and creating a VkPipelineLayout.
         *
         * Accumulates descriptor set layouts and push constant ranges through
         * a fluent API, then builds a fully-formed VulkanPipelineLayout via
         * build(). Unlike VulkanShaderModuleBuilder, which borrows its (large,
         * stably-owned) SPIR-V bytecode to avoid a copy, this builder copies
         * descriptor set layout handles and push constant ranges into its own
         * storage: they are cheap to copy (handles/small PODs), and the
         * single-item add*() API requires owned storage to accumulate into.
         */
        class VulkanPipelineLayoutBuilder
        {
        public:
            /**
             * @brief Appends a single descriptor set layout.
             *
             * Set layouts are stored in the order they are added, and are bound
             * at the matching set index when the layout is used.
             *
             * @param setLayout Descriptor set layout handle to append.
             * @return Reference to this builder, for chaining.
             */
            VulkanPipelineLayoutBuilder& addDescriptorSetLayout(VkDescriptorSetLayout setLayout);

            /**
             * @brief Replaces the full descriptor set layout list at once.
             * @param setLayouts Ordered list of descriptor set layout handles.
             * @return Reference to this builder, for chaining.
             */
            VulkanPipelineLayoutBuilder& setDescriptorSetLayouts(std::span<const VkDescriptorSetLayout> setLayouts);

            /**
             * @brief Appends a raw push constant range.
             * @param range Fully-specified VkPushConstantRange.
             * @return Reference to this builder, for chaining.
             */
            VulkanPipelineLayoutBuilder& addPushConstantRange(const VkPushConstantRange& range);

            /**
             * @brief Appends a push constant range from its individual fields.
             * @param stageFlags Shader stages that access this range.
             * @param offset     Offset, in bytes, of the range within the push
             *                   constant block.
             * @param size       Size, in bytes, of the range.
             * @return Reference to this builder, for chaining.
             */
            VulkanPipelineLayoutBuilder& addPushConstantRange(VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size);

            /**
             * @brief Sets additional pipeline layout creation flags.
             * @param flags VkPipelineLayoutCreateFlags bitmask.
             * @return Reference to this builder, for chaining.
             */
            VulkanPipelineLayoutBuilder& setFlags(VkPipelineLayoutCreateFlags flags);

            /**
             * @brief Assembles all accumulated state into a
             *        VkPipelineLayoutCreateInfo and creates the pipeline layout.
             *
             * @param device Logical device to create the pipeline layout on.
             * @return A VulkanPipelineLayout owning the newly created handle.
             *
             * @throws std::runtime_error if pipeline layout creation fails.
             */
            rendererResources::VulkanPipelineLayout build(VkDevice device);

        private:
            /// Ordered descriptor set layout handles.
            std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts;

            /// Accumulated push constant ranges.
            std::vector<VkPushConstantRange> m_pushConstantRanges;

            /// Additional pipeline layout creation flags.
            VkPipelineLayoutCreateFlags m_flags = 0;
        };
    }
}
