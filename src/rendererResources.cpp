#include <rendererResources.hpp>

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

        rendererResources::VulkanPipelineLayout &RendererResources::pipelineLayout()
        {
            return m_pipelineLayout;
        }

        const rendererResources::VulkanPipelineLayout &RendererResources::pipelineLayout() const
        {
            return m_pipelineLayout;
        }
    }
}
