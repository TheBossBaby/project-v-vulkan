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
                 * @brief Unmaps the memory if it is mapped, then destroys the
                 *        owned buffer and memory, if any.
                 */
                ~VulkanBuffer();

                /// Non-copyable: a VkBuffer has single-owner destruction semantics.
                VulkanBuffer(const VulkanBuffer&) = delete;

                /// Non-copyable: a VkBuffer has single-owner destruction semantics.
                VulkanBuffer& operator=(const VulkanBuffer&) = delete;

                /**
                 * @brief Move-constructs, transferring ownership from another wrapper.
                 *
                 * An active mapping moves too, so pointers into it stay valid.
                 *
                 * @param other Source wrapper; left in an empty, unmapped,
                 *              valid-to-destroy state.
                 */
                VulkanBuffer(VulkanBuffer&& other) noexcept;

                /**
                 * @brief Move-assigns, unmapping and destroying any currently
                 *        owned buffer and transferring ownership from another
                 *        wrapper.
                 *
                 * An active mapping on @p other moves too, so pointers into it
                 * stay valid.
                 *
                 * @param other Source wrapper; left in an empty, unmapped,
                 *              valid-to-destroy state.
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
                 * @brief Copies data into the buffer, mapping its memory only
                 *        for the duration of the call if needed.
                 *
                 * Copies exactly @p size bytes from @p srcData. If the buffer
                 * is not mapped, maps the first @p size bytes, copies to
                 * offset 0 and unmaps again. If map() has already mapped it,
                 * that mapping is reused and left open, and the data lands at
                 * the start of the mapping (offset 0 unless map() was given an
                 * offset). Bytes past @p size are left unchanged. The memory
                 * must be host-visible; without HOST_COHERENT the write is not
                 * flushed.
                 *
                 * For a buffer written every frame, prefer map() once plus
                 * writeMapped().
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
                 * @brief Copies data into the buffer through the mapping kept
                 *        open by map().
                 *
                 * Never maps or unmaps, so it is the cheap path for buffers
                 * updated often, such as per-frame uniform buffers. Copies
                 * exactly @p size bytes to the start of the mapping (offset 0
                 * unless map() was given an offset). Bytes past @p size are
                 * left unchanged. Without HOST_COHERENT memory the write is
                 * not flushed.
                 *
                 * @p size is only checked against getDeviceSize(), not against
                 * the mapped range: if map() was given a smaller size or an
                 * offset, the caller must keep the write inside the mapping.
                 * The GPU must not be reading the written range, e.g. wait on
                 * the fence of the frame that last used the buffer first.
                 *
                 * @pre map() has been called.
                 *
                 * @param srcData Source data to copy; must point to at least
                 *                @p size readable bytes.
                 * @param size    Number of bytes to copy; must be
                 *                <= getDeviceSize(). A size of 0 is a no-op.
                 *
                 * @throws std::runtime_error if the buffer is not mapped or
                 *         @p size exceeds getDeviceSize().
                 */
                void writeMapped(const void* srcData, VkDeviceSize size);

                /**
                 * @brief Returns the size of the buffer.
                 * @return Buffer size in bytes, or 0 if no buffer is owned.
                 */
                VkDeviceSize getDeviceSize() const;

                /**
                 * @brief Maps the buffer's memory and keeps it mapped until
                 *        unmap() or destruction.
                 *
                 * Use for a persistent mapping, e.g. a uniform buffer written
                 * every frame through writeMapped(). Does nothing if the
                 * buffer is already mapped, even when @p size or @p offset
                 * differ from the existing mapping. The memory must be
                 * host-visible.
                 *
                 * @pre The wrapper owns a buffer.
                 *
                 * @param size   Number of bytes to map, or VK_WHOLE_SIZE
                 *               (default) to map from @p offset to the end of
                 *               the memory.
                 * @param offset Byte offset into the buffer where the mapping
                 *               starts (default: 0).
                 *
                 * @throws std::runtime_error if vkMapMemory fails.
                 */
                void map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

                /**
                 * @brief Unmaps the memory mapped by map().
                 *
                 * Pointers into the mapping become invalid. Does nothing if
                 * the buffer is not mapped. The destructor and move assignment
                 * call this automatically.
                 */
                void unmap();
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

                void* m_mappedData = nullptr; // Tracks the persistent pointer
            };
        }
    }
}
