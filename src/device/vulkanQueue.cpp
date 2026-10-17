#include <vulkan/command/vulkanCommandBuffer.hpp>
#include <vulkan/synchronization/vulkanFence.hpp>
#include <vulkan/synchronization/vulkanSemaphore.hpp>
#include <vulkan/util/vulkanCheck.hpp>
#include <vulkan/window/vulkanSwapchain.hpp>

#include <vulkanQueue.hpp>

namespace projectv
{
    namespace vulkan::device
    {
        void VulkanQueue::create(VkQueue inQueue)
        {
            queue = inQueue;
        }

        void VulkanQueue::submit(const command::VulkanCommandBuffer &commandBuffer, const sync::VulkanFence& fence) const
        {
            VkCommandBuffer cmdBufferHandle = commandBuffer.handle();
            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &cmdBufferHandle;

            vkCheck(vkQueueSubmit( queue, 1, &submitInfo, fence.handle()), "VulkanQueue::submit, Failed to sumbmit command buffer");
        }

        void VulkanQueue::submit(const command::VulkanCommandBuffer& commandBuffer, const sync::VulkanSemaphore& waitSemaphore,
                    const VkPipelineStageFlags waitStage, const sync::VulkanSemaphore& signalSemaphore, const sync::VulkanFence& fence) const
        {
            VkCommandBuffer cmdBufferHandle = commandBuffer.handle();
            VkSemaphore waitSemaphoreHandle = waitSemaphore.handle();
            VkSemaphore signalSemaphoreHandle = signalSemaphore.handle();

            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = &waitSemaphoreHandle;
            submitInfo.pWaitDstStageMask = &waitStage;

            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &cmdBufferHandle;

            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = &signalSemaphoreHandle;

            vkCheck(vkQueueSubmit( queue, 1, &submitInfo, fence.handle()), "VulkanQueue::submit, Failed to sumbmit command buffer");
        }

        VkResult VulkanQueue::present(const window::VulkanSwapchain &swapchain, uint32_t imageIndex, const sync::VulkanSemaphore &waitSemaphore) const
        {
            VkSemaphore    waitSemaphoreHandle = waitSemaphore.handle();
            VkSwapchainKHR swapchainHandle     = swapchain.handle();

            VkPresentInfoKHR presentInfo{};
            presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores    = &waitSemaphoreHandle;
            presentInfo.swapchainCount     = 1;
            presentInfo.pSwapchains        = &swapchainHandle;
            presentInfo.pImageIndices      = &imageIndex;

            const VkResult result = vkQueuePresentKHR(queue, &presentInfo);
            if (result != VK_ERROR_OUT_OF_DATE_KHR && result != VK_SUBOPTIMAL_KHR)
                vkCheck(result, "VulkanQueue::present, failed to present swapchain image");
            return result;
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
