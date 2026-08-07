#include <projectV/engine/logging/log.hpp>

#include <vulkan/rendererResources/vulkanFramebuffer.hpp>
#include <vulkan/util/vulkanCheck.hpp>

#include <stdexcept>
#include <utility>

namespace projectv
{
    namespace vulkan
    {
        namespace rendererResources
        {
            VulkanFramebuffer::VulkanFramebuffer(VkDevice device, const VkFramebufferCreateInfo& createInfo)
                : m_logicalDevice(device)
            {
                vkCheck(
                    vkCreateFramebuffer(m_logicalDevice, &createInfo, nullptr, &m_framebuffer),
                    "VulkanFramebuffer::VulkanFramebuffer, Failed to create VkFramebuffer");

                engine::LogInfo("VulkanFramebuffer::create");
            }

            VulkanFramebuffer::~VulkanFramebuffer()
            {
                destroy();
            }

            VulkanFramebuffer::VulkanFramebuffer(VulkanFramebuffer&& other) noexcept
                : m_logicalDevice(other.m_logicalDevice)
                , m_framebuffer(other.m_framebuffer)
            {
                other.m_logicalDevice = VK_NULL_HANDLE;
                other.m_framebuffer   = VK_NULL_HANDLE;
            }

            VulkanFramebuffer& VulkanFramebuffer::operator=(VulkanFramebuffer&& other) noexcept
            {
                if (this != &other)
                {
                    destroy();

                    m_logicalDevice = other.m_logicalDevice;
                    m_framebuffer   = other.m_framebuffer;

                    other.m_logicalDevice = VK_NULL_HANDLE;
                    other.m_framebuffer   = VK_NULL_HANDLE;
                }
                return *this;
            }

            VkFramebuffer VulkanFramebuffer::handle() const noexcept
            {
                return m_framebuffer;
            }

            VulkanFramebuffer::operator bool() const noexcept
            {
                return m_framebuffer != VK_NULL_HANDLE;
            }

            void VulkanFramebuffer::destroy() noexcept
            {
                if (m_framebuffer != VK_NULL_HANDLE && m_logicalDevice != VK_NULL_HANDLE)
                {
                    engine::LogInfo("VulkanFramebuffer::destroy");
                    vkDestroyFramebuffer(m_logicalDevice, m_framebuffer, nullptr);
                    m_framebuffer   = VK_NULL_HANDLE;
                    m_logicalDevice = VK_NULL_HANDLE;
                }
            }
        }
    }
}