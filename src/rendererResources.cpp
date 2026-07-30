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
    }
}
