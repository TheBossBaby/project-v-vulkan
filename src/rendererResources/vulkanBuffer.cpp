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
                destroy();
            }

            VulkanBuffer::VulkanBuffer(VulkanBuffer&& other) noexcept
                : m_logicalDevice(other.m_logicalDevice)
                , m_buffer(other.m_buffer)
                , m_deviceMemory(other.m_deviceMemory)
                , m_deviceSize(other.m_deviceSize)
            {
                other.m_logicalDevice  = VK_NULL_HANDLE;
                other.m_buffer         = VK_NULL_HANDLE;
                other.m_deviceMemory   = VK_NULL_HANDLE;
                other.m_deviceSize     = 0;
            }

            VulkanBuffer &VulkanBuffer::operator=(VulkanBuffer &&other) noexcept
            {
                if (this != &other)
                {
                    destroy();

                    m_logicalDevice  = other.m_logicalDevice;
                    m_buffer         = other.m_buffer;
                    m_deviceMemory   = other.m_deviceMemory;
                    m_deviceSize     = other.m_deviceSize;

                    other.m_logicalDevice  = VK_NULL_HANDLE;
                    other.m_buffer         = VK_NULL_HANDLE;
                    other.m_deviceMemory   = VK_NULL_HANDLE;
                    other.m_deviceSize     = 0;
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

                if (size == 0)
                {
                    return;
                }

                void* data;
                vkCheck(
                    vkMapMemory(m_logicalDevice, m_deviceMemory, 0, size, 0, &data),
                    "VulkanBuffer::write, Mapping Failed");

                std::memcpy(data, srcData, static_cast<size_t>(size));
                vkUnmapMemory(m_logicalDevice, m_deviceMemory);
            }

            VkDeviceSize VulkanBuffer::getDeviceSize() const
            {
                return m_deviceSize;
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
