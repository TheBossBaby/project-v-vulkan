#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/rendererResources/vulkanGraphicsPipeline.hpp>

#include <cstdint>
#include <span>
#include <vector>

namespace projectv
{
    namespace vulkan::builder
    {
        /**
         * @brief Builder for assembling and creating a VkPipeline (graphics pipeline).
         *
         * Accumulates shader stages, fixed-function state, and the pipeline
         * layout/render pass to build against through a fluent API, then
         * builds a fully-formed VulkanGraphicsPipeline via build().
         *
         * The builder consumes already-created Vulkan resources (shader
         * modules, pipeline layout, render pass); it does not create or own
         * any of them. Handles are borrowed and only need to remain valid
         * until build() calls vkCreateGraphicsPipelines(); the resulting
         * VulkanGraphicsPipeline additionally depends on the pipeline layout
         * and render pass staying valid for as long as the pipeline is used
         * (see VulkanGraphicsPipeline's class docs).
         *
         * Sensible defaults are provided for every fixed-function state so a
         * minimal "hello triangle" pipeline (no vertex buffers, single color
         * attachment, dynamic viewport/scissor) only needs shader stages,
         * a layout, and a render pass to be set explicitly.
         */
        class VulkanGraphicsPipelineBuilder
        {
        public:
            /**
             * @brief Appends a programmable shader stage.
             *
             * @param module     Shader module to invoke for this stage.
             *                   Borrowed; only needs to remain valid
             *                   until build() returns.
             * @param stage      Which pipeline stage this module implements.
             * @param entryPoint Name of the shader's entry point function.
             *                   Borrowed; must remain valid until build()
             *                   is called (a string literal such as "main",
             *                   the default, always satisfies this).
             * @return Reference to this builder, for chaining.
             */
            VulkanGraphicsPipelineBuilder& addShaderStage(
                VkShaderModule module, VkShaderStageFlagBits stage, const char* entryPoint = "main");

            /**
             * @brief Sets the vertex input state.
             *
             * Defaults to no bindings/attributes (empty vertex input state),
             * appropriate for a pipeline that generates vertices in-shader
             * (e.g. via gl_VertexIndex) instead of reading a vertex buffer.
             *
             * @param bindings   Vertex binding descriptions.
             * @param attributes Vertex attribute descriptions.
             * @return Reference to this builder, for chaining.
             */
            VulkanGraphicsPipelineBuilder& setVertexInputState(
                std::span<const VkVertexInputBindingDescription> bindings = {},
                std::span<const VkVertexInputAttributeDescription> attributes = {});

            /**
             * @brief Sets the input assembly state.
             * @param topology               Primitive topology (default: triangle list).
             * @param primitiveRestartEnable  Whether a special index value restarts
             *                                primitive assembly (default: disabled).
             * @return Reference to this builder, for chaining.
             */
            VulkanGraphicsPipelineBuilder& setInputAssemblyState(
                VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                VkBool32 primitiveRestartEnable = VK_FALSE);

            /**
             * @brief Sets the viewport/scissor counts for the viewport state.
             *
             * Only counts are configured; the actual viewport and scissor
             * rectangles are supplied dynamically via vkCmdSetViewport and
             * vkCmdSetScissor, so the swapchain extent does not need to be
             * baked into the pipeline.
             *
             * @param viewportCount Number of viewports (default: 1).
             * @param scissorCount  Number of scissor rectangles (default: 1).
             * @note Viewport and scissor are always enabled as dynamic state
             *       (VK_DYNAMIC_STATE_VIEWPORT / VK_DYNAMIC_STATE_SCISSOR).
             * @return Reference to this builder, for chaining.
             */
            VulkanGraphicsPipelineBuilder& setViewportState(uint32_t viewportCount = 1, uint32_t scissorCount = 1);

            /**
             * @brief Sets the rasterization state.
             * @param polygonMode How polygons are rasterized (default: fill).
             * @param cullMode    Which triangle faces to cull (default: none,
             *                    since front-face winding depends on the
             *                    vertex shader and is not assumed here).
             * @param frontFace   Which winding order is considered front-facing
             *                    (default: clockwise).
             * @param lineWidth   Width of rasterized lines (default: 1.0).
             * @return Reference to this builder, for chaining.
             */
            VulkanGraphicsPipelineBuilder& setRasterizationState(
                VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL,
                VkCullModeFlags cullMode = VK_CULL_MODE_NONE,
                VkFrontFace frontFace = VK_FRONT_FACE_CLOCKWISE,
                float lineWidth = 1.0f);

            /**
             * @brief Sets the multisample state.
             * @param sampleCount Rasterization sample count (default: 1, i.e. no MSAA).
             * @return Reference to this builder, for chaining.
             */
            VulkanGraphicsPipelineBuilder& setMultisampleState(VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT);

            /**
             * @brief Appends a raw per-attachment color blend state.
             *
             * If build() is called without any color blend attachment having
             * been added, a single default attachment (blending disabled,
             * full RGBA write mask) is used, matching a pipeline with one
             * color attachment.
             *
             * @param attachment Fully-specified VkPipelineColorBlendAttachmentState.
             * @return Reference to this builder, for chaining.
             */
            VulkanGraphicsPipelineBuilder& addColorBlendAttachment(const VkPipelineColorBlendAttachmentState& attachment);

            /**
             * @brief Appends a per-attachment color blend state from common fields.
             *
             * When blending is enabled, standard alpha blending factors
             * (src-alpha / one-minus-src-alpha, add) are used.
             *
             * @param blendEnable   Whether to blend with the destination attachment.
             * @param colorWriteMask Which color components are written (default: RGBA).
             * @return Reference to this builder, for chaining.
             */
            VulkanGraphicsPipelineBuilder& addColorBlendAttachment(
                VkBool32 blendEnable,
                VkColorComponentFlags colorWriteMask =
                    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);

            /**
             * @brief Sets the pipeline layout to build against.
             * @param layout Pipeline layout handle. Borrowed; the caller retains
             *               ownership and must keep it valid for as long as the
             *               resulting pipeline is used.
             * @return Reference to this builder, for chaining.
             */
            VulkanGraphicsPipelineBuilder& setLayout(VkPipelineLayout layout);

            /**
             * @brief Sets the render pass and subpass to build against.
             * @param renderPass Render pass handle. Borrowed; the caller retains
             *                   ownership and must keep it (or a compatible render
             *                   pass) valid for as long as the resulting pipeline is used.
             * @param subpass    Index of the subpass this pipeline will be used in.
             * @return Reference to this builder, for chaining.
             */
            VulkanGraphicsPipelineBuilder& setRenderPass(VkRenderPass renderPass, uint32_t subpass = 0);

            /**
             * @brief Sets additional pipeline creation flags.
             * @param flags VkPipelineCreateFlags bitmask.
             * @return Reference to this builder, for chaining.
             */
            VulkanGraphicsPipelineBuilder& setFlags(VkPipelineCreateFlags flags);

            /**
             * @brief Assembles all accumulated state into a
             *        VkGraphicsPipelineCreateInfo and creates the pipeline.
             *
             * @param device Logical device to create the pipeline on.
             * @return A VulkanGraphicsPipeline owning the newly created handle.
             *
             * @throws std::runtime_error if no shader stages were added, no
             *         layout or render pass was set, or pipeline creation fails.
             */
            rendererResources::VulkanGraphicsPipeline build(VkDevice device);

        private:
            /**
             * @brief Validates that all required state has been set.
             * @param device Logical device that build() was called with.
             * @throws std::runtime_error if device is null, no shader stages
             *         were added, any shader stage has a null module, or no
             *         pipeline layout/render pass was set.
             */
            void validate(VkDevice device) const;

            /**
             * @brief Returns the configured color blend attachments, or a
             *        single default (blending disabled, full RGBA write mask)
             *        attachment if none were added.
             */
            std::vector<VkPipelineColorBlendAttachmentState> resolveColorBlendAttachments() const;

            /// Builds the vertex input state sub-struct from m_vertexBindings/m_vertexAttributes.
            VkPipelineVertexInputStateCreateInfo buildVertexInputStateInfo() const;

            /// Builds the input assembly state sub-struct from m_topology/m_primitiveRestartEnable.
            VkPipelineInputAssemblyStateCreateInfo buildInputAssemblyStateInfo() const;

            /// Builds the viewport state sub-struct from m_viewportCount/m_scissorCount.
            VkPipelineViewportStateCreateInfo buildViewportStateInfo() const;

            /// Builds the rasterization state sub-struct from m_polygonMode/m_cullMode/m_frontFace/m_lineWidth.
            VkPipelineRasterizationStateCreateInfo buildRasterizationStateInfo() const;

            /// Builds the multisample state sub-struct from m_sampleCount.
            VkPipelineMultisampleStateCreateInfo buildMultisampleStateInfo() const;

            /**
             * @brief Builds the color blend state sub-struct.
             * @param attachments Per-attachment blend states; must outlive the
             *                    returned struct's use (it stores a pointer
             *                    into @p attachments).
             */
            VkPipelineColorBlendStateCreateInfo buildColorBlendStateInfo(
                const std::vector<VkPipelineColorBlendAttachmentState>& attachments) const;

            /// Builds the dynamic state sub-struct from m_dynamicStates.
            VkPipelineDynamicStateCreateInfo buildDynamicStateInfo() const;

        private:
            /// Accumulated shader stages.
            std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;

            /// Vertex input bindings (empty by default: no vertex buffers).
            std::vector<VkVertexInputBindingDescription> m_vertexBindings;

            /// Vertex input attributes (empty by default: no vertex buffers).
            std::vector<VkVertexInputAttributeDescription> m_vertexAttributes;

            /// Accumulated per-attachment color blend states.
            std::vector<VkPipelineColorBlendAttachmentState> m_colorBlendAttachments;

            /// Dynamic states enabled on the pipeline.
            std::vector<VkDynamicState> m_dynamicStates{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

            /// Primitive topology and restart behavior.
            VkPrimitiveTopology m_topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            VkBool32 m_primitiveRestartEnable = VK_FALSE;

            /// Viewport/scissor counts (actual rects supplied dynamically).
            uint32_t m_viewportCount = 1;
            uint32_t m_scissorCount  = 1;

            /// Rasterization state.
            VkPolygonMode   m_polygonMode = VK_POLYGON_MODE_FILL;
            VkCullModeFlags m_cullMode    = VK_CULL_MODE_NONE;
            VkFrontFace     m_frontFace   = VK_FRONT_FACE_CLOCKWISE;
            float           m_lineWidth   = 1.0f;

            /// Multisample state.
            VkSampleCountFlagBits m_sampleCount = VK_SAMPLE_COUNT_1_BIT;

            /// Pipeline layout to build against.
            VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;

            /// Render pass and subpass index to build against.
            VkRenderPass m_renderPass = VK_NULL_HANDLE;
            uint32_t     m_subpass    = 0;

            /// Additional pipeline creation flags.
            VkPipelineCreateFlags m_flags = 0;
        };
    }
}
