#include <projectV/engine/logging/log.hpp>

#include <vulkan/builder/vulkanDescriptorSetLayoutBuilder.hpp>

#include <cstdint>
#include <format>
#include <stdexcept>
#include <string>
#include <unordered_set>

namespace projectv
{
    namespace vulkan
    {
        namespace builder
        {
            descriptor::VulkanDescriptorSetLayout VulkanDescriptorSetLayoutBuilder::build(VkDevice device)
            {
                std::unordered_set<uint32_t> usedBindings;
                usedBindings.reserve(m_LayoutBindingList.size());

                for (const VkDescriptorSetLayoutBinding& layoutBinding : m_LayoutBindingList)
                {
                    if (layoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_MAX_ENUM)
                    {
                        const std::string message = std::format(
                            "VulkanDescriptorSetLayoutBuilder::build, descriptor type not set for binding {}",
                            layoutBinding.binding);
                        engine::LogError(message);
                        throw std::runtime_error(message);
                    }

                    // stageFlags is ignored by Vulkan when descriptorCount is 0 (reserved binding slot)
                    if (layoutBinding.descriptorCount > 0 && layoutBinding.stageFlags == 0)
                    {
                        const std::string message = std::format(
                            "VulkanDescriptorSetLayoutBuilder::build, stage flags not set for binding {}",
                            layoutBinding.binding);
                        engine::LogError(message);
                        throw std::runtime_error(message);
                    }

                    // Binding numbers must be unique within a descriptor set layout
                    if (!usedBindings.insert(layoutBinding.binding).second)
                    {
                        const std::string message = std::format(
                            "VulkanDescriptorSetLayoutBuilder::build, duplicate binding index {}",
                            layoutBinding.binding);
                        engine::LogError(message);
                        throw std::runtime_error(message);
                    }
                }

                VkDescriptorSetLayoutCreateInfo layoutInfo{};
                layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                layoutInfo.bindingCount = static_cast<uint32_t>(m_LayoutBindingList.size());
                layoutInfo.pBindings = m_LayoutBindingList.data();

                return descriptor::VulkanDescriptorSetLayout(device, layoutInfo);
            }

            VulkanDescriptorSetLayoutBuilder& VulkanDescriptorSetLayoutBuilder::addLayoutBinding(VkDescriptorSetLayoutBinding layoutBinding)
            {
                m_LayoutBindingList.emplace_back(layoutBinding);

                return *this;
            }
        }
    }
}
