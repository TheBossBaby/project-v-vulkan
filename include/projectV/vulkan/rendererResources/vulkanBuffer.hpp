#pragma once

#include <vulkan/vulkan.h>

namespace projectv
{
    namespace vulkan
    {
        namespace rendererResources
        {
            /**
             * @brief RAII wrapper around a VkBuffer and its bound VkDeviceMemory.
             *
             * Owns both handles and releases them on destruction. Move-only.
             * Typically created via builder::VulkanBufferBuilder.
             */
            class VulkanBuffer
            {
            public:
                /**
                 * @brief Constructs an empty wrapper with no owned buffer.
                 *
                 * Useful for deferred initialization; the object is invalid
                 * (operator bool() returns false) until move-assigned from
                 * a valid instance.
                 */
                VulkanBuffer() = default;

                /**
                 * @brief Takes ownership of an existing buffer and its memory.
                 * @param device         Logical device the buffer was created on.
                 * @param buffer         Buffer handle to own.
                 * @param deviceMemory   Device memory bound to @p buffer, to own.
                 * @param deviceSize     Size of the buffer, in bytes.
                 */
                VulkanBuffer(VkDevice device, VkBuffer buffer, VkDeviceMemory deviceMemory, VkDeviceSize deviceSize);

                /**
                 * @brief Destroys the owned buffer, if any.
                 */
                ~VulkanBuffer();

                /// Non-copyable: a VkBuffer has single-owner destruction semantics.
                VulkanBuffer(const VulkanBuffer&) = delete;

                /// Non-copyable: a VkBuffer has single-owner destruction semantics.
                VulkanBuffer& operator=(const VulkanBuffer&) = delete;

                /**
                 * @brief Move-constructs, transferring ownership from another wrapper.
                 * @param other Source wrapper; left in an empty, valid-to-destroy state.
                 */
                VulkanBuffer(VulkanBuffer&& other) noexcept;

                /**
                 * @brief Move-assigns, destroying any currently owned buffer
                 *        and transferring ownership from another wrapper.
                 * @param other Source wrapper; left in an empty, valid-to-destroy state.
                 * @return Reference to this object.
                 */
                VulkanBuffer& operator=(VulkanBuffer&& other) noexcept;

                /**
                 * @brief Returns the underlying VkBuffer handle.
                 * @return The owned handle, or VK_NULL_HANDLE if none is owned.
                 */
                VkBuffer handle() const noexcept;

                /**
                 * @brief Checks whether this wrapper owns a valid buffer.
                 * @return true if a buffer is owned, false otherwise.
                 */
                explicit operator bool() const noexcept;

                /**
                 * @brief Copies data into the buffer by mapping its memory.
                 *
                 * Copies exactly @p size bytes from @p srcData to offset 0 of the
                 * buffer. Bytes past @p size are left unchanged. The memory must
                 * be host-visible; without HOST_COHERENT the write is not flushed.
                 *
                 * @param srcData Source data to copy; must point to at least
                 *                @p size readable bytes.
                 * @param size    Number of bytes to copy; must be
                 *                <= getDeviceSize(). A size of 0 is a no-op.
                 *
                 * @throws std::runtime_error if @p size exceeds getDeviceSize()
                 *         or if mapping the memory fails.
                 */
                void write(const void* srcData, VkDeviceSize size);

                /**
                 * @brief Returns the size of the buffer.
                 * @return Buffer size in bytes, or 0 if no buffer is owned.
                 */
                VkDeviceSize getDeviceSize() const;
            private:
                /**
                 * @brief Destroys the owned buffer and resets internal state
                 *        to VK_NULL_HANDLE. Safe to call multiple times.
                 */
                void destroy() noexcept;

                /// Logical device used to create/destroy m_buffer.
                VkDevice         m_logicalDevice  = VK_NULL_HANDLE;

                /// Owned buffer handle.
                VkBuffer         m_buffer         = VK_NULL_HANDLE;
                
                /// Owned device memory handle.
                VkDeviceMemory   m_deviceMemory   = VK_NULL_HANDLE;

                /// Size of the buffer, in bytes.
                VkDeviceSize     m_deviceSize     = 0;
            };
        }
    }
}
