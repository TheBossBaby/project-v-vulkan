#pragma once

#include <vulkan/vulkan.h>

#include <vulkan/rendererResources/vulkanBuffer.hpp>


namespace projectv
{
    namespace vulkan
    {
        namespace builder
        {
            /**
             * @brief Builder for assembling and creating a VkBuffer with bound device memory.
             *
             * Accumulates buffer creation parameters and the desired memory
             * properties through a fluent API, then builds a VulkanBuffer that
             * owns both the VkBuffer and its backing VkDeviceMemory via build().
             */
            class VulkanBufferBuilder
            {
            public:
                /**
                 * @brief Sets additional buffer creation flags.
                 * @param createFlags VkBufferCreateFlags bitmask (default: 0).
                 * @return Reference to this builder, for chaining.
                 */
                VulkanBufferBuilder& setCreateFlags(VkBufferCreateFlags createFlags);

                /**
                 * @brief Sets the size of the buffer.
                 * @param deviceSize Buffer size, in bytes.
                 * @return Reference to this builder, for chaining.
                 */
                VulkanBufferBuilder& setDeviceSize(VkDeviceSize deviceSize);

                /**
                 * @brief Sets how the buffer will be used.
                 * @param usageFlags VkBufferUsageFlags bitmask
                 *        (e.g. VK_BUFFER_USAGE_VERTEX_BUFFER_BIT).
                 * @return Reference to this builder, for chaining.
                 */
                VulkanBufferBuilder& setUsageFlags(VkBufferUsageFlags usageFlags);

                /**
                 * @brief Sets the queue family sharing mode.
                 * @param sharingMode Sharing mode (default: VK_SHARING_MODE_EXCLUSIVE).
                 * @return Reference to this builder, for chaining.
                 */
                VulkanBufferBuilder& setSharingMode(VkSharingMode sharingMode);

                /**
                 * @brief Sets the memory properties the backing allocation must have.
                 * @param properties VkMemoryPropertyFlags bitmask
                 *        (e.g. HOST_VISIBLE | HOST_COHERENT for CPU-writable buffers).
                 * @return Reference to this builder, for chaining.
                 */
                VulkanBufferBuilder& setMemoryProperties(VkMemoryPropertyFlags properties);

                /**
                 * @brief Creates the buffer, allocates matching device memory
                 *        and binds the two together.
                 *
                 * If allocation or binding fails, any partially created
                 * resources are released before the exception propagates.
                 *
                 * @param device         Logical device to create the buffer on.
                 * @param physicalDevice Physical device queried for memory types.
                 * @return A VulkanBuffer owning the new buffer and its memory.
                 *
                 * @throws std::runtime_error if buffer creation, memory allocation
                 *         or binding fails, or no suitable memory type exists.
                 */
                rendererResources::VulkanBuffer build(VkDevice device, VkPhysicalDevice physicalDevice);
            private:
                /**
                 * @brief Allocates device memory satisfying the buffer's
                 *        requirements and the configured memory properties.
                 * @param device          Logical device to allocate on.
                 * @param physicalDevice  Physical device queried for memory types.
                 * @param buffer          Buffer whose memory requirements are used.
                 * @param outDeviceMemory Receives the allocated memory handle.
                 *
                 * @throws std::runtime_error if the allocation fails.
                 */
                void allocateBufferMemory(VkDevice device, VkPhysicalDevice physicalDevice, VkBuffer buffer, VkDeviceMemory& outDeviceMemory);

                /**
                 * @brief Finds a memory type index allowed by @p typeFilter that
                 *        has all of the configured memory properties.
                 * @param physicalDevice Physical device to query.
                 * @param typeFilter     Bitmask of acceptable memory type indices,
                 *                       from VkMemoryRequirements::memoryTypeBits.
                 * @return Index of the first matching memory type.
                 *
                 * @throws std::runtime_error if no suitable memory type exists.
                 */
                uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter);
            private:
                /// Buffer creation flags.
                VkBufferCreateFlags    m_createFlags = 0;

                /// Buffer size, in bytes.
                VkDeviceSize           m_size = 0;

                /// Buffer usage flags.
                VkBufferUsageFlags     m_usageFlags = 0;

                /// Queue family sharing mode.
                VkSharingMode          m_sharingMode = VK_SHARING_MODE_EXCLUSIVE;

                /// Required properties of the backing memory.
                VkMemoryPropertyFlags  m_properties = 0;
            };
        }
    }
}
