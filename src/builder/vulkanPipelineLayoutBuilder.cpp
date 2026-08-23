#include <projectV/engine/logging/log.hpp>

#include <vulkan/builder/vulkanPipelineLayoutBuilder.hpp>

#include <stdexcept>
namespace projectv
{
    namespace vulkan::builder
    {
        VulkanPipelineLayoutBuilder& VulkanPipelineLayoutBuilder::addDescriptorSetLayout(VkDescriptorSetLayout setLayout)
        {
            m_descriptorSetLayouts.push_back(setLayout);
            return *this;
        }

        VulkanPipelineLayoutBuilder& VulkanPipelineLayoutBuilder::setDescriptorSetLayouts(std::span<const VkDescriptorSetLayout> setLayouts)
        {
            m_descriptorSetLayouts.assign(setLayouts.begin(), setLayouts.end());
            return *this;
        }

        VulkanPipelineLayoutBuilder& VulkanPipelineLayoutBuilder::addPushConstantRange(const VkPushConstantRange& range)
        {
            m_pushConstantRanges.push_back(range);
            return *this;
        }

        VulkanPipelineLayoutBuilder& VulkanPipelineLayoutBuilder::addPushConstantRange(VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size)
        {
            VkPushConstantRange range{};
            range.stageFlags = stageFlags;
            range.offset     = offset;
            range.size       = size;
            return addPushConstantRange(range);
        }

        VulkanPipelineLayoutBuilder& VulkanPipelineLayoutBuilder::setFlags(VkPipelineLayoutCreateFlags flags)
        {
            m_flags = flags;
            return *this;
        }

        rendererResources::VulkanPipelineLayout VulkanPipelineLayoutBuilder::build(VkDevice device)
        {
            if (device == VK_NULL_HANDLE)
            {
                engine::LogError(
                    "VulkanPipelineLayoutBuilder::build, invalid VkDevice");
                throw std::runtime_error(
                    "VulkanPipelineLayoutBuilder::build, invalid VkDevice");
            }
            
            for (const auto setLayout : m_descriptorSetLayouts)
            {
                if (setLayout == VK_NULL_HANDLE)
                {
                    engine::LogError(
                        "VulkanPipelineLayoutBuilder::build, invalid VkDescriptorSetLayout");
                    throw std::runtime_error(
                        "VulkanPipelineLayoutBuilder::build, invalid VkDescriptorSetLayout");
                }
            }

            VkPipelineLayoutCreateInfo createInfo{};
            createInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            createInfo.flags                  = m_flags;
            createInfo.setLayoutCount         = static_cast<uint32_t>(m_descriptorSetLayouts.size());
            createInfo.pSetLayouts            = m_descriptorSetLayouts.empty() ? nullptr : m_descriptorSetLayouts.data();
            createInfo.pushConstantRangeCount = static_cast<uint32_t>(m_pushConstantRanges.size());
            createInfo.pPushConstantRanges    = m_pushConstantRanges.empty() ? nullptr : m_pushConstantRanges.data();

            return rendererResources::VulkanPipelineLayout(device, createInfo);
        }
    }
}
