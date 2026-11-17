#include <projectV/engine/logging/log.hpp>

#include <vulkan/rendererResources/vulkanBuffer.hpp>
#include <vulkan/util/vulkanCheck.hpp>

#include <cstring>
#include <format>
#include <stdexcept>
namespace projectv
{
    namespace vulkan
    {
        namespace rendererResources
        {
            VulkanBuffer::VulkanBuffer(VkDevice device, VkBuffer buffer, VkDeviceMemory deviceMemory, VkDeviceSize deviceSize)
                : m_logicalDevice(device)
                , m_buffer(buffer)
                , m_deviceMemory(deviceMemory)
                , m_deviceSize(deviceSize)
            {
            }

            VulkanBuffer::~VulkanBuffer()
            {
                unmap();
                destroy();
            }

            VulkanBuffer::VulkanBuffer(VulkanBuffer&& other) noexcept
                : m_logicalDevice(other.m_logicalDevice)
                , m_buffer(other.m_buffer)
                , m_deviceMemory(other.m_deviceMemory)
                , m_deviceSize(other.m_deviceSize)
                , m_mappedData(other.m_mappedData)
            {
                other.m_logicalDevice  = VK_NULL_HANDLE;
                other.m_buffer         = VK_NULL_HANDLE;
                other.m_deviceMemory   = VK_NULL_HANDLE;
                other.m_deviceSize     = 0;
                other.m_mappedData     = nullptr;
            }

            VulkanBuffer &VulkanBuffer::operator=(VulkanBuffer &&other) noexcept
            {
                if (this != &other)
                {
                    unmap();
                    destroy();

                    m_logicalDevice  = other.m_logicalDevice;
                    m_buffer         = other.m_buffer;
                    m_deviceMemory   = other.m_deviceMemory;
                    m_deviceSize     = other.m_deviceSize;
                    m_mappedData     = other.m_mappedData;


                    other.m_logicalDevice  = VK_NULL_HANDLE;
                    other.m_buffer         = VK_NULL_HANDLE;
                    other.m_deviceMemory   = VK_NULL_HANDLE;
                    other.m_deviceSize     = 0;
                    other.m_mappedData     = nullptr;
                }
                return *this;
            }

            VkBuffer VulkanBuffer::handle() const noexcept
            {
                return m_buffer;
            }

            VulkanBuffer::operator bool() const noexcept
            {
                return m_buffer != VK_NULL_HANDLE;
            }

            void VulkanBuffer::write(const void* srcData, VkDeviceSize size)
            {
                if (size > m_deviceSize)
                {
                    throw std::runtime_error(std::format(
                        "VulkanBuffer::write, write size {} exceeds buffer size {}", size, m_deviceSize));
                }
                const bool wasMapped = m_mappedData != nullptr;

                if (size == 0)
                {
                    return;
                }

                map(size, 0);

                std::memcpy(m_mappedData, srcData, static_cast<size_t>(size));

                if (!wasMapped) unmap();
            }

            void VulkanBuffer::writeMapped(const void *srcData, VkDeviceSize size)
            {
                if (m_mappedData == nullptr)
                {
                    throw std::runtime_error("VulkanBuffer::writeMapped, buffer is not mapped; call map() first");
                }

                if (size > m_deviceSize)
                {
                    throw std::runtime_error(std::format(
                        "VulkanBuffer::writeMapped, writeMapped size {} exceeds buffer size {}", size, m_deviceSize));
                }

                if (size == 0)
                {
                    return;
                }
                std::memcpy(m_mappedData, srcData, static_cast<size_t>(size));
            }

            VkDeviceSize VulkanBuffer::getDeviceSize() const
            {
                return m_deviceSize;
            }

            void VulkanBuffer::map(VkDeviceSize size, VkDeviceSize offset)
            {
                if (m_mappedData != nullptr)
                {
                    return;
                }

                vkCheck(
                    vkMapMemory(m_logicalDevice, m_deviceMemory, offset, size, 0, &m_mappedData),
                    "VulkanBuffer::map, Mapping Failed");
            }

            void VulkanBuffer::unmap()
            {
                if (m_mappedData != nullptr)
                {
                    vkUnmapMemory(m_logicalDevice, m_deviceMemory);
                    m_mappedData = nullptr;
                }
            }

            void VulkanBuffer::destroy() noexcept
            {
                if (m_buffer != VK_NULL_HANDLE && m_logicalDevice != VK_NULL_HANDLE)
                {
                    engine::LogInfo("VulkanBuffer::destroy");

                    vkDestroyBuffer(m_logicalDevice, m_buffer, nullptr);
                    vkFreeMemory(m_logicalDevice, m_deviceMemory, nullptr);

                    m_buffer         = VK_NULL_HANDLE;
                    m_deviceMemory   = VK_NULL_HANDLE;   
                    m_logicalDevice  = VK_NULL_HANDLE;
                    m_deviceSize     = 0;
                }
            }
        }
    }
}
