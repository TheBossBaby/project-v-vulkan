#include <projectV/engine/logging/log.hpp>

#include <vulkan/rendererResources/vulkanImage.hpp>

namespace projectv
{
    namespace vulkan
    {
        namespace rendererResources
        {
            VulkanImage::VulkanImage(VkDevice device, VkImage image, VkDeviceMemory deviceMemory, VkFormat format, VkExtent3D extent, VkDeviceSize deviceSize)
                : m_logicalDevice(device)
                , m_image(image)
                , m_deviceMemory(deviceMemory)
                , m_format(format)
                , m_extent{ extent }
                , m_deviceSize(deviceSize)
            {
            }

            VulkanImage::~VulkanImage()
            {
                destroy();
            }

            VulkanImage::VulkanImage(VulkanImage&& other) noexcept
                : m_logicalDevice(other.m_logicalDevice)
                , m_image(other.m_image)
                , m_deviceMemory(other.m_deviceMemory)
                , m_format(other.m_format)
                , m_extent(other.m_extent)
                , m_deviceSize(other.m_deviceSize)
            {
                other.m_logicalDevice  = VK_NULL_HANDLE;
                other.m_image          = VK_NULL_HANDLE;
                other.m_deviceMemory   = VK_NULL_HANDLE;
                other.m_format         = VK_FORMAT_UNDEFINED;
                other.m_extent         = { 0, 0, 1 };
                other.m_deviceSize     = 0;
            }

            VulkanImage& VulkanImage::operator=(VulkanImage&& other) noexcept
            {
                if (this != &other)
                {
                    destroy();

                    m_logicalDevice  = other.m_logicalDevice;
                    m_image          = other.m_image;
                    m_deviceMemory   = other.m_deviceMemory;
                    m_format         = other.m_format;
                    m_extent         = other.m_extent;
                    m_deviceSize     = other.m_deviceSize;

                    other.m_logicalDevice  = VK_NULL_HANDLE;
                    other.m_image          = VK_NULL_HANDLE;
                    other.m_deviceMemory   = VK_NULL_HANDLE;
                    other.m_format         = VK_FORMAT_UNDEFINED;
                    other.m_extent         = { 0, 0, 1 };
                    other.m_deviceSize     = 0;
                }
                return *this;
            }

            VkImage VulkanImage::handle() const noexcept
            {
                return m_image;
            }

            VulkanImage::operator bool() const noexcept
            {
                return m_image != VK_NULL_HANDLE;
            }

            VkFormat VulkanImage::format() const noexcept
            {
                return m_format;
            }

            VkExtent3D VulkanImage::extent() const noexcept
            {
                return m_extent;
            }

            VkDeviceSize VulkanImage::getDeviceSize() const
            {
                return m_deviceSize;
            }

            void VulkanImage::destroy() noexcept
            {
                if (m_image != VK_NULL_HANDLE && m_logicalDevice != VK_NULL_HANDLE)
                {
                    engine::LogInfo("VulkanImage::destroy");

                    vkDestroyImage(m_logicalDevice, m_image, nullptr);
                    vkFreeMemory(m_logicalDevice, m_deviceMemory, nullptr);

                    m_image          = VK_NULL_HANDLE;
                    m_deviceMemory   = VK_NULL_HANDLE;
                    m_logicalDevice  = VK_NULL_HANDLE;
                    m_format         = VK_FORMAT_UNDEFINED;
                    m_extent         = {0, 0, 1};
                    m_deviceSize     = 0;
                }
            }
        }
    }
}
