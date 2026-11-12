#include <vulkan/builder/vulkanDescriptorSetLayoutBindingBuilder.hpp>

namespace projectv
{
    namespace vulkan
    {
        namespace builder
        {
            VkDescriptorSetLayoutBinding VulkanDescriptorSetLayoutBindingBuilder::build()
            {            
                VkDescriptorSetLayoutBinding layoutBinding{};

                layoutBinding.binding            = m_binding;
                layoutBinding.descriptorType     = m_descriptorType;
                layoutBinding.descriptorCount    = m_descriptorCount;
                layoutBinding.stageFlags         = m_stageFlags;
                layoutBinding.pImmutableSamplers = nullptr;
                
                return layoutBinding;
            }

            VulkanDescriptorSetLayoutBindingBuilder &VulkanDescriptorSetLayoutBindingBuilder::setBinding(uint32_t binding)
            {
                m_binding = binding;
                return *this;
            }

            VulkanDescriptorSetLayoutBindingBuilder &VulkanDescriptorSetLayoutBindingBuilder::setDescriptorType(VkDescriptorType descriptorType)
            {
                m_descriptorType = descriptorType;
                return *this;
            }

            VulkanDescriptorSetLayoutBindingBuilder &VulkanDescriptorSetLayoutBindingBuilder::setDescriptorCount(uint32_t descriptorCount)
            {
                m_descriptorCount = descriptorCount;
                return *this;
            }

            VulkanDescriptorSetLayoutBindingBuilder &VulkanDescriptorSetLayoutBindingBuilder::setStageFlags(VkShaderStageFlags stageFlags)
            {
                m_stageFlags = stageFlags;
                return *this;
            }
        }
    }
}
