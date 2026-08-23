#include <projectV/engine/logging/log.hpp>

#include <vulkan/rendererResources/vulkanShaderModule.hpp>
#include <vulkan/util/vulkanCheck.hpp>

#include <stdexcept>
#include <utility>

namespace projectv
{
    namespace vulkan
    {
        namespace rendererResources
        {
            VulkanShaderModule::VulkanShaderModule(VkDevice device, const VkShaderModuleCreateInfo& createInfo)
                : m_logicalDevice(device)
            {
                vkCheck(
                    vkCreateShaderModule(m_logicalDevice, &createInfo, nullptr, &m_shaderModule),
                    "VulkanShaderModule::VulkanShaderModule, Failed to create VkShaderModule");

                engine::LogInfo("VulkanShaderModule::create");
            }

            VulkanShaderModule::~VulkanShaderModule()
            {
                destroy();
            }

            VulkanShaderModule::VulkanShaderModule(VulkanShaderModule&& other) noexcept
                : m_logicalDevice(other.m_logicalDevice)
                , m_shaderModule(other.m_shaderModule)
            {
                other.m_logicalDevice = VK_NULL_HANDLE;
                other.m_shaderModule  = VK_NULL_HANDLE;
            }

            VulkanShaderModule& VulkanShaderModule::operator=(VulkanShaderModule&& other) noexcept
            {
                if (this != &other)
                {
                    destroy();

                    m_logicalDevice = other.m_logicalDevice;
                    m_shaderModule  = other.m_shaderModule;

                    other.m_logicalDevice = VK_NULL_HANDLE;
                    other.m_shaderModule  = VK_NULL_HANDLE;
                }
                return *this;
            }

            VkShaderModule VulkanShaderModule::handle() const noexcept
            {
                return m_shaderModule;
            }

            VulkanShaderModule::operator bool() const noexcept
            {
                return m_shaderModule != VK_NULL_HANDLE;
            }

            void VulkanShaderModule::destroy() noexcept
            {
                if (m_shaderModule != VK_NULL_HANDLE && m_logicalDevice != VK_NULL_HANDLE)
                {
                    engine::LogInfo("VulkanShaderModule::destroy");
                    vkDestroyShaderModule(m_logicalDevice, m_shaderModule, nullptr);
                    m_shaderModule  = VK_NULL_HANDLE;
                    m_logicalDevice = VK_NULL_HANDLE;
                }
            }
        }
    }
}
