#pragma once

#include <vulkan/vulkanRenderPass.hpp>
#include <vulkan/rendererResources/vulkanFramebuffer.hpp>

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
        private:
            VulkanRenderPass m_renderPass;

            std::vector<rendererResources::VulkanFramebuffer> m_frameBuffers;
        };
    }
}