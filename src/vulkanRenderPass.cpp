#include <projectV/engine/logging/log.hpp>

#include <vulkan/util/vulkanCheck.hpp>
#include <vulkan/vulkanRenderPass.hpp>

namespace projectv
{
    namespace vulkan
    {
        VulkanRenderPass::VulkanRenderPass(VkDevice device, const VkRenderPassCreateInfo &createInfo)
            : m_logicalDevice(device)
        {
            vkCheck(
                vkCreateRenderPass(m_logicalDevice, &createInfo, nullptr, &m_renderPass),
                 "VulkanRenderPass::VulkanRenderPass, Failed to create VkRenderPass");
            engine::LogInfo("VulkanRenderPass::create");
        }

        vulkan::VulkanRenderPass::~VulkanRenderPass()
        {
            destroy();
        }

        VulkanRenderPass::VulkanRenderPass(VulkanRenderPass &&other) noexcept
            : m_logicalDevice(other.m_logicalDevice)
            , m_renderPass(other.m_renderPass)
        {
            other.m_logicalDevice = VK_NULL_HANDLE;
            other.m_renderPass = VK_NULL_HANDLE;
        }

        VulkanRenderPass &VulkanRenderPass::operator=(VulkanRenderPass &&other) noexcept
        {
            if (this != &other)
            {
                destroy();
                m_logicalDevice    = other.m_logicalDevice;
                m_renderPass = other.m_renderPass;

                other.m_logicalDevice    = VK_NULL_HANDLE;
                other.m_renderPass = VK_NULL_HANDLE;
            }
            return *this;
        }

        VkRenderPass VulkanRenderPass::handle() const noexcept
        {
            return m_renderPass;
        }
        
        VulkanRenderPass::operator bool() const noexcept
        {
            return m_renderPass != VK_NULL_HANDLE;
        }

        void VulkanRenderPass::destroy() noexcept
        {
            if (m_renderPass != VK_NULL_HANDLE && m_logicalDevice != VK_NULL_HANDLE)
            {
                engine::LogInfo("VulkanRenderPass::destroy");
                vkDestroyRenderPass(m_logicalDevice, m_renderPass, nullptr);
            }
            m_renderPass = VK_NULL_HANDLE;
            m_logicalDevice     = VK_NULL_HANDLE;
        }
    }
}
