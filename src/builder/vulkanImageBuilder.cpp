#include <projectV/engine/logging/log.hpp>

#include <vulkan/builder/vulkanImageBuilder.hpp>
#include <vulkan/util/vulkanCheck.hpp>
#include <vulkan/util/vulkanMemory.hpp>

#include <stdexcept>

namespace projectv
{
    namespace vulkan
    {
        namespace builder
        {
            VulkanImageBuilder& VulkanImageBuilder::setCreateFlags(VkImageCreateFlags createFlags)
            {
                m_createFlags = createFlags;
                return *this;
            }

            VulkanImageBuilder& VulkanImageBuilder::setImageType(VkImageType imageType)
            {
                m_imageType = imageType;
                return *this;
            }

            VulkanImageBuilder& VulkanImageBuilder::setFormat(VkFormat format)
            {
                m_format = format;
                return *this;
            }

            VulkanImageBuilder& VulkanImageBuilder::setExtent(uint32_t width, uint32_t height, uint32_t depth)
            {
                m_extent = { width, height, depth };
                return *this;
            }

            VulkanImageBuilder& VulkanImageBuilder::setMipLevels(uint32_t mipLevels)
            {
                m_mipLevels = mipLevels;
                return *this;
            }

            VulkanImageBuilder& VulkanImageBuilder::setArrayLayers(uint32_t arrayLayers)
            {
                m_arrayLayers = arrayLayers;
                return *this;
            }

            VulkanImageBuilder& VulkanImageBuilder::setSamples(VkSampleCountFlagBits samples)
            {
                m_samples = samples;
                return *this;
            }

            VulkanImageBuilder& VulkanImageBuilder::setTiling(VkImageTiling tiling)
            {
                m_tiling = tiling;
                return *this;
            }

            VulkanImageBuilder& VulkanImageBuilder::setUsageFlags(VkImageUsageFlags usageFlags)
            {
                m_usageFlags = usageFlags;
                return *this;
            }

            VulkanImageBuilder& VulkanImageBuilder::setSharingMode(VkSharingMode sharingMode)
            {
                m_sharingMode = sharingMode;
                return *this;
            }

            VulkanImageBuilder& VulkanImageBuilder::setInitialLayout(VkImageLayout initialLayout)
            {
                m_initialLayout = initialLayout;
                return *this;
            }

            VulkanImageBuilder& VulkanImageBuilder::setMemoryProperties(VkMemoryPropertyFlags properties)
            {
                m_memoryProperties = properties;
                return *this;
            }

            rendererResources::VulkanImage VulkanImageBuilder::build(VkDevice device, VkPhysicalDevice physicalDevice)
            {
                if (m_format == VK_FORMAT_UNDEFINED)
                {
                    engine::LogError("VulkanImageBuilder::build, format is not set");
                    throw std::runtime_error("VulkanImageBuilder::build, format is not set");
                }

                if (m_extent.width == 0 || m_extent.height == 0 || m_extent.depth == 0)
                {
                    engine::LogError("VulkanImageBuilder::build, extent must be non-zero");
                    throw std::runtime_error("VulkanImageBuilder::build, extent must be non-zero");
                }

                if (m_mipLevels == 0)
                {
                    engine::LogError("VulkanImageBuilder::build, mip levels are not set");
                    throw std::runtime_error("VulkanImageBuilder::build, mip levels are not set");
                }

                if (m_arrayLayers == 0)
                {
                    engine::LogError("VulkanImageBuilder::build, array layers are not set");
                    throw std::runtime_error("VulkanImageBuilder::build, array layers are not set");
                }

                if (m_usageFlags == 0)
                {
                    engine::LogError("VulkanImageBuilder::build, usage flags are not set");
                    throw std::runtime_error("VulkanImageBuilder::build, usage flags are not set");
                }

                VkImage        image        = VK_NULL_HANDLE;
                VkDeviceMemory deviceMemory = VK_NULL_HANDLE;

                VkImageCreateInfo createInfo{};
                createInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                createInfo.flags         = m_createFlags;
                createInfo.imageType     = m_imageType;
                createInfo.format        = m_format;
                createInfo.extent        = m_extent;
                createInfo.mipLevels     = m_mipLevels;
                createInfo.arrayLayers   = m_arrayLayers;
                createInfo.samples       = m_samples;
                createInfo.tiling        = m_tiling;
                createInfo.usage         = m_usageFlags;
                createInfo.sharingMode   = m_sharingMode;
                createInfo.initialLayout = m_initialLayout;

                vkCheck(
                    vkCreateImage(device, &createInfo, nullptr, &image),
                    "VulkanImageBuilder::build, failed to create VkImage"
                );

                VkMemoryRequirements memRequirements;
                vkGetImageMemoryRequirements(device, image, &memRequirements);

                try
                {
                    deviceMemory = allocateImageMemory(device, physicalDevice, memRequirements);
                    vkCheck(
                        vkBindImageMemory(device, image, deviceMemory, 0),
                        "VulkanImageBuilder::build, failed to bind image memory"
                    );
                }
                catch (...)
                {
                    if (deviceMemory != VK_NULL_HANDLE) vkFreeMemory(device, deviceMemory, nullptr);
                    vkDestroyImage(device, image, nullptr);
                    throw;
                }

                return rendererResources::VulkanImage(device, image, deviceMemory, m_format, m_extent, memRequirements.size);
            }

            VkDeviceMemory VulkanImageBuilder::allocateImageMemory(VkDevice device, VkPhysicalDevice physicalDevice, const VkMemoryRequirements& memRequirements)
            {
                VkMemoryAllocateInfo allocInfo{};
                allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                allocInfo.allocationSize  = memRequirements.size;
                allocInfo.memoryTypeIndex = util::findMemoryType(physicalDevice, memRequirements.memoryTypeBits, m_memoryProperties);

                VkDeviceMemory deviceMemory = VK_NULL_HANDLE;
                vkCheck(
                    vkAllocateMemory(device, &allocInfo, nullptr, &deviceMemory),
                    "VulkanImageBuilder::allocateImageMemory, failed to allocate image memory"
                );

                return deviceMemory;
            }
        }
    }
}
