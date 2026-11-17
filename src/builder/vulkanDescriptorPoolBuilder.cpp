#include <projectV/engine/logging/log.hpp>

#include <vulkan/builder/vulkanDescriptorPoolBuilder.hpp>

#include <cstddef>
#include <format>
#include <stdexcept>
#include <string>

namespace projectv
{
    namespace vulkan
    {
        namespace builder
        {
            VulkanDescriptorPoolBuilder &VulkanDescriptorPoolBuilder::setMaxSets(uint32_t maxSets)
            {
                m_maxSets = maxSets;
                return *this;
            }

            VulkanDescriptorPoolBuilder &VulkanDescriptorPoolBuilder::addFlags(VkDescriptorPoolCreateFlags flags)
            {
                m_flags |= flags;
                return *this;
            }

            VulkanDescriptorPoolBuilder &VulkanDescriptorPoolBuilder::addPoolSize(VkDescriptorPoolSize poolSize)
            {
                m_descriptorPoolSizeList.emplace_back(poolSize);
                return *this;
            }

            descriptor::VulkanDescriptorPool VulkanDescriptorPoolBuilder::build(VkDevice device)
            {
                for (size_t i = 0; i < m_descriptorPoolSizeList.size(); ++i)
                {
                    const VkDescriptorPoolSize& descriptorPoolSize = m_descriptorPoolSizeList[i];

                    if(descriptorPoolSize.descriptorCount == 0)
                    {
                        const std::string message = std::format(
                            "VulkanDescriptorPoolBuilder::build, descriptorCount is 0 for pool size {} (descriptorType {})",
                            i, static_cast<int>(descriptorPoolSize.type));
                        engine::LogError(message);
                        throw std::runtime_error(message);
                    }
                }

                if(m_maxSets == 0)
                {
                    const std::string message = "VulkanDescriptorPoolBuilder::build, maxSets is 0";
                    engine::LogError(message);
                    throw std::runtime_error(message);
                }

                if (m_descriptorPoolSizeList.empty())
                {
                    const std::string message = "VulkanDescriptorPoolBuilder::build, Descriptor Pool size is empty";
                    engine::LogError(message);
                    throw std::runtime_error(message);
                }

                VkDescriptorPoolCreateInfo poolInfo{};
                poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
                poolInfo.poolSizeCount = static_cast<uint32_t>(m_descriptorPoolSizeList.size());
                poolInfo.pPoolSizes    = m_descriptorPoolSizeList.data();
                poolInfo.maxSets       = m_maxSets;
                poolInfo.flags         = m_flags;

                return descriptor::VulkanDescriptorPool(device, poolInfo);
            }
        }
    }
}
