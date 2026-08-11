#pragma once

#include <projectV/core/graphicsPipelineHandle.hpp>

#include <vulkan/vulkanRenderPass.hpp>
#include <vulkan/rendererResources/vulkanFramebuffer.hpp>
#include <vulkan/rendererResources/vulkanPipelineLayout.hpp>
#include <vulkan/rendererResources/vulkanGraphicsPipeline.hpp>
#include <vulkan/rendererResources/vulkanPipelineLayout.hpp>

#include <vector>
namespace projectv
{
    namespace vulkan
    {
        class RendererResources
        {
        public:
            /**
             * @brief A single registered graphics pipeline and the pipeline
             *        layout it was built against.
             *
             * The layout is kept alongside the pipeline (rather than in its
             * own separate registry) because, in this codebase, the two are
             * always created and looked up together as one unit.
             */
            struct GraphicsPipelineEntry
            {
                rendererResources::VulkanPipelineLayout layout;
                rendererResources::VulkanGraphicsPipeline pipeline;
            };

            VulkanRenderPass& renderPass();

            const VulkanRenderPass& renderPass() const;

            std::vector<rendererResources::VulkanFramebuffer>& framebuffers();

            const std::vector<rendererResources::VulkanFramebuffer>& framebuffers() const;

            /**
             * @brief Registers a newly created graphics pipeline.
             *
             * @param layout   Pipeline layout the pipeline was built against.
             * @param pipeline The created graphics pipeline.
             * @return A handle identifying the registered entry.
             */
            core::GraphicsPipelineHandle addGraphicsPipeline(
                rendererResources::VulkanPipelineLayout layout,
                rendererResources::VulkanGraphicsPipeline pipeline);

            /**
             * @brief Looks up a previously registered graphics pipeline entry by handle.
             * @param handle Handle returned by addGraphicsPipeline().
             * @return Pointer to the entry if the handle is valid and known; otherwise nullptr.
             */
            const GraphicsPipelineEntry* graphicsPipeline(core::GraphicsPipelineHandle handle) const;

        private:
            VulkanRenderPass m_renderPass;

            std::vector<rendererResources::VulkanFramebuffer> m_framebuffers;

            std::vector<GraphicsPipelineEntry> m_graphicsPipelines;
        };
    }
}
