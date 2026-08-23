#include <projectV/engine/logging/log.hpp>

#include <vulkan/rendererResources/vulkanGraphicsPipeline.hpp>
#include <vulkan/util/vulkanCheck.hpp>

#include <stdexcept>
#include <utility>

namespace projectv
{
    namespace vulkan
    {
        namespace rendererResources
        {
            VulkanGraphicsPipeline::VulkanGraphicsPipeline(VkDevice device, const VkGraphicsPipelineCreateInfo& createInfo)
                : m_logicalDevice(device)
            {
                vkCheck(
                    vkCreateGraphicsPipelines(m_logicalDevice, VK_NULL_HANDLE, 1, &createInfo, nullptr, &m_pipeline),
                    "VulkanGraphicsPipeline::VulkanGraphicsPipeline, Failed to create VkPipeline");

                engine::LogInfo("VulkanGraphicsPipeline::create");
            }

            VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
            {
                destroy();
            }

            VulkanGraphicsPipeline::VulkanGraphicsPipeline(VulkanGraphicsPipeline&& other) noexcept
                : m_logicalDevice(other.m_logicalDevice)
                , m_pipeline(other.m_pipeline)
            {
                other.m_logicalDevice = VK_NULL_HANDLE;
                other.m_pipeline      = VK_NULL_HANDLE;
            }

            VulkanGraphicsPipeline& VulkanGraphicsPipeline::operator=(VulkanGraphicsPipeline&& other) noexcept
            {
                if (this != &other)
                {
                    destroy();

                    m_logicalDevice = other.m_logicalDevice;
                    m_pipeline      = other.m_pipeline;

                    other.m_logicalDevice = VK_NULL_HANDLE;
                    other.m_pipeline      = VK_NULL_HANDLE;
                }
                return *this;
            }

            VkPipeline VulkanGraphicsPipeline::handle() const noexcept
            {
                return m_pipeline;
            }

            VulkanGraphicsPipeline::operator bool() const noexcept
            {
                return m_pipeline != VK_NULL_HANDLE;
            }

            void VulkanGraphicsPipeline::destroy() noexcept
            {
                if (m_pipeline != VK_NULL_HANDLE && m_logicalDevice != VK_NULL_HANDLE)
                {
                    engine::LogInfo("VulkanGraphicsPipeline::destroy");
                    vkDestroyPipeline(m_logicalDevice, m_pipeline, nullptr);
                    m_pipeline      = VK_NULL_HANDLE;
                    m_logicalDevice = VK_NULL_HANDLE;
                }
            }
        }
    }
}
