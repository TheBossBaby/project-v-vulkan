#include <format>

#include <projectV/engine/logging/log.hpp>

#include <vulkan/util/vulkanCheck.hpp>
#include <vulkan/vulkanRenderPass.hpp>

#include <vulkan/rendererResources/vulkanFramebuffer.hpp>
#include <vulkan/rendererResources/vulkanGraphicsPipeline.hpp>

#include "vulkanCommandBuffer.hpp"
namespace projectv
{
    namespace vulkan::command
    {
        VulkanCommandBuffer::VulkanCommandBuffer(VkCommandBuffer handle)
            : commandBuffer(handle)
        {
        }

        void VulkanCommandBuffer::begin()
        {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = 0; // No special recording behavior.
            beginInfo.pInheritanceInfo = nullptr; // Optional

            vkCheck(vkBeginCommandBuffer(commandBuffer, &beginInfo), 
                "VulkanCommandBuffer::begin, Failed to begin recording command buffer!");
        }

        void VulkanCommandBuffer::end()
        {
            vkCheck(vkEndCommandBuffer(commandBuffer), "VulkanCommandBuffer::end, Failed to end command buffer recording!");
        }

        void VulkanCommandBuffer::beginRenderPass(const VulkanRenderPass& renderPass,
                     const rendererResources::VulkanFramebuffer& framebuffer,
                     VkRect2D renderArea,
                     std::span<const VkClearValue> clearValues)
        {
            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = renderPass.handle();
            renderPassInfo.framebuffer = framebuffer.handle();
            renderPassInfo.renderArea = renderArea;
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        }

        void VulkanCommandBuffer::endRenderPass()
        {
            vkCmdEndRenderPass(commandBuffer);
        }

        void VulkanCommandBuffer::bindGraphicsPipeline(const rendererResources::VulkanGraphicsPipeline &graphicsPipeline)
        {
            vkCmdBindPipeline(
                commandBuffer, 
                VK_PIPELINE_BIND_POINT_GRAPHICS, 
                graphicsPipeline.handle());
        }

        void VulkanCommandBuffer::draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
        {
            vkCmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
        }

        void VulkanCommandBuffer::setViewport(const VkViewport &viewport)
        {
            vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        }

        void VulkanCommandBuffer::setScissor(const VkRect2D &scissor)
        {
            vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
        }

        void VulkanCommandBuffer::setViewport(float x, float y, float width, float height, float minDepth, float maxDepth)
        {
            VkViewport viewport{};
            viewport.x = x;
            viewport.y = y;
            viewport.width = width;
            viewport.height = height;
            viewport.minDepth = minDepth;
            viewport.maxDepth = maxDepth;

            setViewport(viewport);
        }

        void VulkanCommandBuffer::setScissor(VkOffset2D offset, VkExtent2D extent)
        {
            VkRect2D scissor{};
            scissor.offset = offset;
            scissor.extent = extent;

            setScissor(scissor);
        }

        VkCommandBuffer VulkanCommandBuffer::handle() const noexcept
        {
            return commandBuffer;
        }
    }
}
