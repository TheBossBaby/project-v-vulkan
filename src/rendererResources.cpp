#include <rendererResources.hpp>

#include <cstdint>
#include <utility>

namespace projectv
{
    namespace vulkan
    {
        VulkanRenderPass &RendererResources::renderPass()
        {
            return m_renderPass;
        }

        const VulkanRenderPass &RendererResources::renderPass() const
        {
            return m_renderPass;
        }

        std::vector<rendererResources::VulkanFramebuffer> &RendererResources::framebuffers()
        {
            return m_framebuffers;
        }

        const std::vector<rendererResources::VulkanFramebuffer> &RendererResources::framebuffers() const
        {
            return m_framebuffers;
        }

        core::GraphicsPipelineHandle RendererResources::addGraphicsPipeline(
            rendererResources::VulkanPipelineLayout layout,
            rendererResources::VulkanGraphicsPipeline pipeline)
        {
            core::GraphicsPipelineHandle handle;
            handle.index = static_cast<std::uint32_t>(m_graphicsPipelines.size());

            m_graphicsPipelines.push_back({ std::move(layout), std::move(pipeline) });

            return handle;
        }

        const RendererResources::GraphicsPipelineEntry* RendererResources::graphicsPipeline(
            core::GraphicsPipelineHandle handle) const
        {
            if (!handle.isValid() || handle.index >= m_graphicsPipelines.size())
                return nullptr;

            return &m_graphicsPipelines[handle.index];
        }
    }
}
