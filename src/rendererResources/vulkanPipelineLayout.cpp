#include <projectV/engine/logging/log.hpp>

#include <vulkan/rendererResources/vulkanPipelineLayout.hpp>
#include <vulkan/util/vulkanCheck.hpp>

#include <stdexcept>
#include <utility>

namespace projectv
{
    namespace vulkan
    {
        namespace rendererResources
        {
            VulkanPipelineLayout::VulkanPipelineLayout(VkDevice device, const VkPipelineLayoutCreateInfo& createInfo)
                : m_logicalDevice(device)
            {
                vkCheck(
                    vkCreatePipelineLayout(m_logicalDevice, &createInfo, nullptr, &m_pipelineLayout),
                    "VulkanPipelineLayout::VulkanPipelineLayout, Failed to create VkPipelineLayout");

                engine::LogInfo("VulkanPipelineLayout::create");
            }

            VulkanPipelineLayout::~VulkanPipelineLayout()
            {
                destroy();
            }

            VulkanPipelineLayout::VulkanPipelineLayout(VulkanPipelineLayout&& other) noexcept
                : m_logicalDevice(other.m_logicalDevice)
                , m_pipelineLayout(other.m_pipelineLayout)
            {
                other.m_logicalDevice  = VK_NULL_HANDLE;
                other.m_pipelineLayout = VK_NULL_HANDLE;
            }

            VulkanPipelineLayout& VulkanPipelineLayout::operator=(VulkanPipelineLayout&& other) noexcept
            {
                if (this != &other)
                {
                    destroy();

                    m_logicalDevice  = other.m_logicalDevice;
                    m_pipelineLayout = other.m_pipelineLayout;

                    other.m_logicalDevice  = VK_NULL_HANDLE;
                    other.m_pipelineLayout = VK_NULL_HANDLE;
                }
                return *this;
            }

            VkPipelineLayout VulkanPipelineLayout::handle() const noexcept
            {
                return m_pipelineLayout;
            }

            VulkanPipelineLayout::operator bool() const noexcept
            {
                return m_pipelineLayout != VK_NULL_HANDLE;
            }

            void VulkanPipelineLayout::destroy() noexcept
            {
                if (m_pipelineLayout != VK_NULL_HANDLE && m_logicalDevice != VK_NULL_HANDLE)
                {
                    engine::LogInfo("VulkanPipelineLayout::destroy");
                    vkDestroyPipelineLayout(m_logicalDevice, m_pipelineLayout, nullptr);
                    m_pipelineLayout = VK_NULL_HANDLE;
                    m_logicalDevice  = VK_NULL_HANDLE;
                }
            }
        }
    }
}
