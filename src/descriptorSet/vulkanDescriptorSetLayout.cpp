#include <projectV/engine/logging/log.hpp>

#include <vulkan/descriptorSet/vulkanDescriptorSetLayout.hpp>
#include <vulkan/util/vulkanCheck.hpp>

namespace projectv
{
    namespace vulkan::descriptor
    {
        VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
        {
            destroy();
        }

        VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VulkanDescriptorSetLayout &&other) noexcept
            : m_logicalDevice(other.m_logicalDevice)
            , m_descriptorSetLayout(other.m_descriptorSetLayout)
        {   
            other.m_logicalDevice       = VK_NULL_HANDLE;
            other.m_descriptorSetLayout = VK_NULL_HANDLE;
        }

        VulkanDescriptorSetLayout &VulkanDescriptorSetLayout::operator=(VulkanDescriptorSetLayout &&other) noexcept
        {
            if (this != &other)
            {
                destroy();

                m_logicalDevice       = other.m_logicalDevice;
                m_descriptorSetLayout = other.m_descriptorSetLayout;

                other.m_logicalDevice       = VK_NULL_HANDLE;
                other.m_descriptorSetLayout = VK_NULL_HANDLE;
            }
            return *this;                
        }

        VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VkDevice device, const VkDescriptorSetLayoutCreateInfo &createInfo)
        {
            m_logicalDevice = device;
            vkCheck(
                    vkCreateDescriptorSetLayout(m_logicalDevice, &createInfo, nullptr, &m_descriptorSetLayout) ,
                    "VulkanDescriptorSetLayout::VulkanDescriptorSetLayout, Failed to create descriptor set layout");
        }

        VkDescriptorSetLayout VulkanDescriptorSetLayout::handle() const noexcept
        {
            return m_descriptorSetLayout;
        }

        VulkanDescriptorSetLayout::operator bool() const noexcept
        {
            return m_descriptorSetLayout != VK_NULL_HANDLE;
        }

        void VulkanDescriptorSetLayout::destroy()
        {
            if (m_descriptorSetLayout != VK_NULL_HANDLE && m_logicalDevice != VK_NULL_HANDLE)
            {
                engine::LogInfo("VulkanDescriptorSetLayout::destroy");

                vkDestroyDescriptorSetLayout(m_logicalDevice, m_descriptorSetLayout, nullptr);

                m_descriptorSetLayout = VK_NULL_HANDLE;
                m_logicalDevice       = VK_NULL_HANDLE;
            }
        }
    }
}