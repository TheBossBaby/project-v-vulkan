#include <vulkan/command/vulkanCommandBuffer.hpp>
#include <vulkan/util/vulkanCheck.hpp>

#include <vulkanQueue.hpp>

namespace projectv
{
    namespace vulkan::device
    {
        void VulkanQueue::create(VkQueue inQueue)
        {
            queue = inQueue;
        }

        void VulkanQueue::submit(const command::VulkanCommandBuffer &commandBuffer) const
        {
            VkCommandBuffer cmdBufferHandle = commandBuffer.handle();
            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &cmdBufferHandle;

            vkCheck(vkQueueSubmit( queue, 1, &submitInfo, VK_NULL_HANDLE), "VulkanQueue::submit, Failed to sumbmit command buffer");
        }

        void VulkanQueue::waitIdle() const
        {
            vkCheck(vkQueueWaitIdle(queue), "VulkanQueue::waitIdle, Failed waiting for queue idle.");
        }

        VkQueue VulkanQueue::handle() const noexcept
        {
            return queue;
        }
    }
}
