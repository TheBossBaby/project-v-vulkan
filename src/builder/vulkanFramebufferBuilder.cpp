#include <projectV/engine/logging/log.hpp>

#include <vulkan/builder/vulkanFramebufferBuilder.hpp>

#include <stdexcept>
#include <utility>

namespace projectv
{
    namespace vulkan
    {
        namespace builder
        {
            VulkanFramebufferBuilder& VulkanFramebufferBuilder::setRenderPass(VkRenderPass renderPass)
            {
                m_renderPass = renderPass;
                return *this;
            }

            VulkanFramebufferBuilder& VulkanFramebufferBuilder::addAttachment(VkImageView imageView)
            {
                m_attachments.push_back(imageView);
                return *this;
            }

            VulkanFramebufferBuilder& VulkanFramebufferBuilder::setAttachments(std::span<const window::VulkanImageView> attachments)
            {
                m_attachments.clear();

                for (const auto& view : attachments)
                    m_attachments.push_back(view.handle());

                return *this;
            }

            VulkanFramebufferBuilder& VulkanFramebufferBuilder::setExtent(uint32_t width, uint32_t height)
            {
                if(width == 0)
                {
                    engine::LogError("VulkanFramebufferBuilder::setExtent, invalid width");
                    throw std::runtime_error("VulkanFramebufferBuilder::setExtent, invalid width");
                }

                if(height == 0)
                {
                    engine::LogError("VulkanFramebufferBuilder::setExtent, invalid height");
                    throw std::runtime_error("VulkanFramebufferBuilder::setExtent, invalid height");
                }

                m_width  = width;
                m_height = height;
                return *this;
            }

            VulkanFramebufferBuilder& VulkanFramebufferBuilder::setLayers(uint32_t layers)
            {
                m_layers = layers;
                return *this;
            }

            VulkanFramebufferBuilder& VulkanFramebufferBuilder::setFlags(VkFramebufferCreateFlags flags)
            {
                m_flags = flags;
                return *this;
            }

            rendererResources::VulkanFramebuffer VulkanFramebufferBuilder::build(VkDevice device)
            {
                if (m_renderPass == VK_NULL_HANDLE)
                {
                    throw std::runtime_error("VulkanFramebufferBuilder::build called without a render pass set");
                }

                if(m_attachments.empty())
                {
                    engine::LogError("VulkanFramebufferBuilder::setExtent, m_attachments is empty");
                    throw std::runtime_error("VulkanFramebufferBuilder::setExtent, m_attachments is empty");
                }

                VkFramebufferCreateInfo createInfo{};
                createInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                createInfo.flags           = m_flags;
                createInfo.renderPass      = m_renderPass;
                createInfo.attachmentCount = static_cast<uint32_t>(m_attachments.size());
                createInfo.pAttachments    = m_attachments.data();
                createInfo.width           = m_width;
                createInfo.height          = m_height;
                createInfo.layers          = m_layers;

                return rendererResources::VulkanFramebuffer(device, createInfo);
            }
        }
    }
}