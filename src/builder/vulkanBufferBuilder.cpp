#include <projectV/engine/logging/log.hpp>

#include <vulkan/builder/vulkanBufferBuilder.hpp>
#include <vulkan/util/vulkanCheck.hpp>
#include <vulkan/util/vulkanMemory.hpp>

namespace projectv
{
    namespace vulkan
    {
        namespace builder
        {
            VulkanBufferBuilder& VulkanBufferBuilder::setCreateFlags(VkBufferCreateFlags createFlags)
            {
                m_createFlags = createFlags;
                return *this;
            }

            VulkanBufferBuilder &VulkanBufferBuilder::setDeviceSize(VkDeviceSize deviceSize)
            {
                m_size = deviceSize;
                return *this;
            }

            VulkanBufferBuilder &VulkanBufferBuilder::setUsageFlags(VkBufferUsageFlags usageFlags)
            {
                m_usageFlags = usageFlags;
                return *this;
            }

            VulkanBufferBuilder &VulkanBufferBuilder::setSharingMode(VkSharingMode sharingMode)
            {
                m_sharingMode = sharingMode;
                return *this;
            }

            VulkanBufferBuilder &VulkanBufferBuilder::setMemoryProperties(VkMemoryPropertyFlags properties)
            {
                m_properties = properties;
                return *this;
            }

            rendererResources::VulkanBuffer VulkanBufferBuilder::build(VkDevice device, VkPhysicalDevice physicalDevice)
            {
                VkBuffer           buffer       = VK_NULL_HANDLE;
                VkDeviceMemory     deviceMemory = VK_NULL_HANDLE;

                VkBufferCreateInfo createInfo{};
                createInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                createInfo.flags       = m_createFlags;
                createInfo.size        = m_size;
                createInfo.usage       = m_usageFlags;
                createInfo.sharingMode = m_sharingMode;

                vkCheck(
                    vkCreateBuffer(device, &createInfo, nullptr, &buffer), 
                    "VulkanBufferBuilder::create, Failed to create VkBuffer"
                );

                try
                {
                    allocateBufferMemory(device, physicalDevice, buffer, deviceMemory);
                    vkCheck(
                        vkBindBufferMemory(device, buffer, deviceMemory, 0), 
                        "VulkanBufferBuilder::build, failed to bind buffer memory"
                    );
                }
                catch (...)
                {
                    if (deviceMemory != VK_NULL_HANDLE) vkFreeMemory(device, deviceMemory, nullptr);
                    vkDestroyBuffer(device, buffer, nullptr);
                    throw;
                }
                
                return rendererResources::VulkanBuffer(device, buffer, deviceMemory, m_size);
            }

            void VulkanBufferBuilder::allocateBufferMemory(VkDevice device, VkPhysicalDevice physicalDevice, VkBuffer buffer, VkDeviceMemory& outDeviceMemory)
            {
                VkMemoryRequirements memRequirements;
                vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

                VkMemoryAllocateInfo allocInfo{};
                allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                allocInfo.allocationSize = memRequirements.size;
                allocInfo.memoryTypeIndex = util::findMemoryType(physicalDevice, memRequirements.memoryTypeBits, m_properties);

                vkCheck(
                    vkAllocateMemory(device, &allocInfo, nullptr, &outDeviceMemory),
                    "VulkanBuffer::VulkanBuffer, Failed to allocate VkBufferMemory"
                );
            }
        }
    }
}
