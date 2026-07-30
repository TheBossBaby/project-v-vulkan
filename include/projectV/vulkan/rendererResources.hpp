#pragma once

#include <vulkan/vulkanRenderPass.hpp>

namespace projectv
{
    namespace vulkan
    {
        class RendererResources
        {
        public:
            VulkanRenderPass& renderPass();

            const VulkanRenderPass& renderPass() const;
        private:
            VulkanRenderPass m_renderPass;
        };
    }
}