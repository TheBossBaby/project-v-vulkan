#include <projectV/engine/logging/log.hpp>

#include <vulkan/rendererResources/vulkanSampler.hpp>
#include <vulkan/util/vulkanCheck.hpp>

namespace projectv
{
    namespace vulkan
    {
        namespace rendererResources
        {
            VulkanSampler::VulkanSampler(VkDevice device, const VkSamplerCreateInfo& createInfo)
                : m_logicalDevice(device)
            {
                vkCheck(
                    vkCreateSampler(m_logicalDevice, &createInfo, nullptr, &m_sampler),
                    "VulkanSampler::VulkanSampler, Failed to create VkSampler");

                engine::LogInfo("VulkanSampler::create");
            }

            VulkanSampler::~VulkanSampler()
            {
                destroy();
            }

            VulkanSampler::VulkanSampler(VulkanSampler&& other) noexcept
                : m_logicalDevice(other.m_logicalDevice)
                , m_sampler(other.m_sampler)
            {
                other.m_logicalDevice = VK_NULL_HANDLE;
                other.m_sampler       = VK_NULL_HANDLE;
            }

            VulkanSampler& VulkanSampler::operator=(VulkanSampler&& other) noexcept
            {
                if (this != &other)
                {
                    destroy();

                    m_logicalDevice = other.m_logicalDevice;
                    m_sampler       = other.m_sampler;

                    other.m_logicalDevice = VK_NULL_HANDLE;
                    other.m_sampler       = VK_NULL_HANDLE;
                }
                return *this;
            }

            VkSampler VulkanSampler::handle() const noexcept
            {
                return m_sampler;
            }

            VulkanSampler::operator bool() const noexcept
            {
                return m_sampler != VK_NULL_HANDLE;
            }

            void VulkanSampler::destroy() noexcept
            {
                if (m_sampler != VK_NULL_HANDLE && m_logicalDevice != VK_NULL_HANDLE)
                {
                    engine::LogInfo("VulkanSampler::destroy");
                    vkDestroySampler(m_logicalDevice, m_sampler, nullptr);
                    m_sampler       = VK_NULL_HANDLE;
                    m_logicalDevice = VK_NULL_HANDLE;
                }
            }
        }
    }
}
