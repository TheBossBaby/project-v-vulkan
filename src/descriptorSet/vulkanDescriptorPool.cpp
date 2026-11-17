#include <projectV/engine/logging/log.hpp>

#include <vulkan/descriptorSet/vulkanDescriptorPool.hpp>
#include <vulkan/util/vulkanCheck.hpp>

namespace projectv
{
    namespace vulkan::descriptor
    {
        VulkanDescriptorPool::VulkanDescriptorPool(VulkanDescriptorPool &&other) noexcept
            : m_logicalDevice(other.m_logicalDevice)
            , m_descriptorPool(other.m_descriptorPool)
        {
            other.m_logicalDevice  = VK_NULL_HANDLE;
            other.m_descriptorPool = VK_NULL_HANDLE;
        }

        VulkanDescriptorPool &VulkanDescriptorPool::operator=(VulkanDescriptorPool &&other) noexcept
        {
            if (this != &other)
            {
                destroy();

                m_logicalDevice  = other.m_logicalDevice;
                m_descriptorPool = other.m_descriptorPool;

                other.m_logicalDevice  = VK_NULL_HANDLE;
                other.m_descriptorPool = VK_NULL_HANDLE;

            }
            return *this;
        }

        VulkanDescriptorPool::VulkanDescriptorPool(VkDevice device, const VkDescriptorPoolCreateInfo& createInfo)
        {
            m_logicalDevice = device;
            vkCheck(
                vkCreateDescriptorPool(m_logicalDevice, &createInfo, nullptr, &m_descriptorPool),
                "VulkanDescriptorPool::VulkanDescriptorPool, Failed to create descriptor pool"
            );
        }

        VulkanDescriptorPool::~VulkanDescriptorPool()
        {
            destroy();
        }

        std::vector<VkDescriptorSet> VulkanDescriptorPool::allocate(std::span<const VkDescriptorSetLayout> layouts)
        {
            if(layouts.empty())
            {
                engine::LogWarn("VulkanDescriptorPool::allocate, DescriptorSetLayout count is 0, Vulkan needs at least 1, returning");
                return {};
            }
            std::vector<VkDescriptorSet> setList(layouts.size());

            VkDescriptorSetAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool = m_descriptorPool;
            allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
            allocInfo.pSetLayouts = layouts.data();

            vkCheck(
                vkAllocateDescriptorSets(m_logicalDevice, &allocInfo, setList.data()),
                "VulkanDescriptorPool::allocate, failed to allocate Descriptor Sets"
            );

            return setList;
        }

        VkDescriptorPool  VulkanDescriptorPool::handle() const noexcept
        {
            return m_descriptorPool;
        }

        VulkanDescriptorPool::operator bool() const noexcept
        {
            return m_descriptorPool != VK_NULL_HANDLE;
        }

        void VulkanDescriptorPool::destroy()
        {
            if(m_logicalDevice != VK_NULL_HANDLE && m_descriptorPool != VK_NULL_HANDLE)
            {
                engine::LogInfo("VulkanDescriptorPool::destroy");

                vkDestroyDescriptorPool(m_logicalDevice, m_descriptorPool, nullptr);

                m_logicalDevice = VK_NULL_HANDLE;
                m_descriptorPool = VK_NULL_HANDLE;
            }
        }
    }
}
