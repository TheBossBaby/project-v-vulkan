#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/rendererResources/vulkanBuffer.hpp>

#include <span>
#include <vector>

namespace projectv::vulkan
{
    class VulkanRenderPass;

    namespace rendererResources
    {
        class VulkanFramebuffer;
        class VulkanGraphicsPipeline;
        class VulkanPipelineLayout;
    }
}

namespace projectv::vulkan::command
{
    class VulkanCommandPool;

    /**
     * @brief Lightweight wrapper around a Vulkan command buffer.
     *
     * A VulkanCommandBuffer does not own the underlying Vulkan allocation.
     * Command buffers are allocated from a VulkanCommandPool, and their
     * lifetime is managed entirely by that pool.
     *
     * Destroying the associated VulkanCommandPool invalidates every
     * VulkanCommandBuffer allocated from it.
     */
    class VulkanCommandBuffer
    {
        /**
         * @brief Only a VulkanCommandPool may allocate command buffers.
         */
        friend class VulkanCommandPool;

    public:
        /// Non-copyable: two wrappers would record into the same command buffer.
        VulkanCommandBuffer(const VulkanCommandBuffer&) = delete;

        /// Non-copyable: two wrappers would record into the same command buffer.
        VulkanCommandBuffer& operator=(const VulkanCommandBuffer&) = delete;

        /**
         * @brief Move-constructs from another wrapper.
         *
         * The defaulted move copies the handle, so the source still refers
         * to the same command buffer afterwards; do not record through it.
         */
        VulkanCommandBuffer(VulkanCommandBuffer&&) noexcept = default;

        /**
         * @brief Move-assigns from another wrapper.
         *
         * The defaulted move copies the handle, so the source still refers
         * to the same command buffer afterwards; do not record through it.
         *
         * @return Reference to this object.
         */
        VulkanCommandBuffer& operator=(VulkanCommandBuffer&&) noexcept = default;

        /**
         * @brief Does nothing; the owning VulkanCommandPool frees the
         *        command buffer.
         */
        ~VulkanCommandBuffer() = default;

        /**
         * @brief Begins recording commands into this command buffer.
         *
         * The command buffer must not already be in the recording state.
         */
        void begin();

        /**
         * @brief Ends command recording.
         *
         * After this call succeeds, the command buffer is ready for queue
         * submission.
         */
        void end();

        /**
         * @brief Begins a render pass instance with inline subpass contents.
         *
         * Must be called while recording (after begin()) and must be paired
         * with endRenderPass() before end() is called.
         *
         * @param renderPass  Render pass to begin.
         * @param framebuffer Framebuffer whose attachments are rendered into;
         *                    must be compatible with renderPass.
         * @param renderArea  Region of the framebuffer affected by the pass,
         *                    typically {{0, 0}, swapchain extent}.
         * @param clearValues One clear value per attachment, indexed by
         *                    attachment number. Only read for attachments
         *                    using VK_ATTACHMENT_LOAD_OP_CLEAR. Must stay
         *                    valid for the duration of this call only.
         */
        void beginRenderPass(const VulkanRenderPass& renderPass,
                     const rendererResources::VulkanFramebuffer& framebuffer,
                     VkRect2D renderArea,
                     std::span<const VkClearValue> clearValues);

        /**
         * @brief Ends the current render pass instance.
         *
         * Must be called inside a render pass started by beginRenderPass().
         */
        void endRenderPass();

        /**
         * @brief Binds a graphics pipeline for subsequent draw commands.
         *
         * The pipeline stays bound until another graphics pipeline is bound
         * or recording ends.
         *
         * @param graphicsPipeline Pipeline to bind; must be compatible with the
         *                         render pass and subpass currently active.
         */
        void bindGraphicsPipeline(const rendererResources::VulkanGraphicsPipeline& graphicsPipeline);

        /**
         * @brief Records a non-indexed draw using the currently bound pipeline.
         *
         * Must be called inside a render pass with a graphics pipeline bound,
         * and with any dynamic state (viewport, scissor) already set.
         *
         * @param vertexCount   Number of vertices to draw.
         * @param instanceCount Number of instances to draw; 1 for no instancing.
         * @param firstVertex   Index of the first vertex (gl_VertexIndex offset).
         * @param firstInstance Index of the first instance (gl_InstanceIndex offset).
         */
        void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);

        /**
         * @brief Records an indexed draw using the bound vertex and index buffers.
         * @param indexCount    Number of indices to draw.
         * @param instanceCount Number of instances to draw.
         * @param firstIndex    Index of the first index in the index buffer.
         * @param vertexOffset  Value added to each index before vertex lookup.
         * @param firstInstance Instance ID of the first instance.
         */
        void drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance);

        /**
         * @brief Sets viewport 0 as dynamic state.
         *
         * Only valid for pipelines created with VK_DYNAMIC_STATE_VIEWPORT.
         *
         * @param viewport Viewport describing the framebuffer-space mapping.
         */
        void setViewport(const VkViewport& viewport);

        /**
         * @brief Sets viewport 0 as dynamic state from individual components.
         *
         * Convenience overload of setViewport(const VkViewport&).
         *
         * @param x        Left edge of the viewport in pixels.
         * @param y        Top edge of the viewport in pixels.
         * @param width    Viewport width in pixels.
         * @param height   Viewport height in pixels.
         * @param minDepth Minimum depth, usually 0.0f.
         * @param maxDepth Maximum depth, usually 1.0f.
         */
        void setViewport(float x, float y, float width, float height,
                            float minDepth, float maxDepth);

        /**
         * @brief Sets scissor 0 as dynamic state.
         *
         * Fragments outside this rectangle are discarded. Only valid for
         * pipelines created with VK_DYNAMIC_STATE_SCISSOR.
         *
         * @param scissor Scissor rectangle in framebuffer pixels.
         */
        void setScissor(const VkRect2D& scissor);

        /**
         * @brief Sets scissor 0 as dynamic state from an offset and extent.
         *
         * Convenience overload of setScissor(const VkRect2D&).
         *
         * @param offset Top-left corner of the scissor rectangle in pixels.
         * @param extent Size of the scissor rectangle in pixels.
         */
        void setScissor(VkOffset2D offset, VkExtent2D extent);

        /**
         * @brief Binds vertex buffers to consecutive binding slots.
         *
         * Each buffer is bound at offset 0.
         *
         * @param firstBinding Binding slot of the first buffer.
         * @param buffers      Buffers to bind, in binding order; must not contain nullptr.
         */
        void bindVertexBuffers(uint32_t firstBinding, const std::vector<const rendererResources::VulkanBuffer*>& buffers);

        /**
         * @brief Binds an index buffer for subsequent indexed draws.
         * @param buffer    Buffer holding the index data.
         * @param indexType Type of each index (e.g. VK_INDEX_TYPE_UINT32).
         * @param offSet    Byte offset into @p buffer where indices start.
         */
        void bindIndexBuffer(const rendererResources::VulkanBuffer& buffer, VkIndexType indexType, uint64_t offSet = 0);

        /**
         * @brief Updates push constant values for subsequent draws.
         *
         * @p offSet and @p size must be multiples of 4 and lie inside a push
         * constant range of @p pipelineLayout that covers @p stageFlags.
         *
         * @param pipelineLayout Layout declaring the push constant range; must
         *                       be compatible with the pipelines used by the
         *                       following draws.
         * @param stageFlags     Shader stages that read the updated range.
         * @param offSet         Byte offset of the first value to update.
         * @param size           Number of bytes to update.
         * @param pValues        Source data, at least @p size bytes. Copied
         *                       during the call.
         */
        void pushConstants(const rendererResources::VulkanPipelineLayout &pipelineLayout, VkShaderStageFlags stageFlags, uint32_t offSet, uint32_t size, const void *pValues);

        /**
         * @brief Records a copy of @p size bytes from one buffer to another.
         *
         * Typically used to upload data from a host-visible staging buffer
         * into a device-local buffer. The copy is only recorded here; it
         * runs when this command buffer is submitted, so both buffers must
         * stay alive until the GPU has finished executing it.
         *
         * Must be recorded outside a render pass. @p source needs
         * VK_BUFFER_USAGE_TRANSFER_SRC_BIT and @p desination needs
         * VK_BUFFER_USAGE_TRANSFER_DST_BIT. Both ranges must fit inside
         * their buffers: srcOffset + size <= size of @p source, and
         * dstOffset + size <= size of @p desination.
         *
         * @param source     Buffer to copy from.
         * @param desination Buffer to copy to.
         * @param size       Number of bytes to copy; must not be 0.
         * @param srcOffset  Byte offset into @p source where the copy starts (default: 0).
         * @param dstOffset  Byte offset into @p desination where the data is written (default: 0).
         *
         * @throws std::runtime_error if @p size is 0.
         */
        void copyBuffer(VkBuffer source, VkBuffer desination, VkDeviceSize size, VkDeviceSize srcOffset = 0, VkDeviceSize dstOffset = 0);

        /**
         * @brief Returns the underlying Vulkan command buffer handle.
         *
         * This handle is borrowed and remains valid only while the owning
         * VulkanCommandPool exists.
         *
         * @return The wrapped VkCommandBuffer handle.
         */
        [[nodiscard]]
        VkCommandBuffer handle() const noexcept;

    private:
        /**
         * @brief Constructs a wrapper around an allocated Vulkan command buffer.
         *
         * Only VulkanCommandPool may construct instances.
         *
         * @param handle Valid Vulkan command buffer handle.
         */
        explicit VulkanCommandBuffer(VkCommandBuffer handle);

    private:
        /// Wrapped Vulkan command buffer handle.
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    };
}