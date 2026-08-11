#pragma once

#include <vulkan/vulkanRenderPass.hpp>
#include <vulkan/rendererResources/vulkanFramebuffer.hpp>
#include <vulkan/rendererResources/vulkanPipelineLayout.hpp>

#include <vector>
namespace projectv
{
    namespace vulkan
    {
        class RendererResources
        {
        public:
            VulkanRenderPass& renderPass();

            const VulkanRenderPass& renderPass() const;

            std::vector<rendererResources::VulkanFramebuffer>& framebuffers();

            const std::vector<rendererResources::VulkanFramebuffer>& framebuffers() const;

            rendererResources::VulkanPipelineLayout& pipelineLayout();

            const rendererResources::VulkanPipelineLayout& pipelineLayout() const;
        private:
            VulkanRenderPass m_renderPass;

            std::vector<rendererResources::VulkanFramebuffer> m_framebuffers;

            rendererResources::VulkanPipelineLayout m_pipelineLayout;
        };
    }
}
