#pragma once

#include <vulkan/vulkan.h>

namespace projectv
{
    namespace vulkan::command { class VulkanCommandBuffer ; }
    namespace vulkan::sync 
    { 
        class VulkanFence;
        class VulkanSemaphore; 
    }
    
    namespace vulkan::window 
    { 
        class VulkanSwapchain;
    }

    namespace vulkan::device
    {
        class VulkanQueue
        {
            public:
                void create(VkQueue inQueue);

                void submit(const command::VulkanCommandBuffer& commandBuffer, const sync::VulkanFence& fence) const;

                /**
                 * @brief Submits a command buffer with semaphore synchronization.
                 *
                 * Execution waits on @p waitSemaphore at @p waitStage, signals
                 * @p signalSemaphore when the command buffer completes, and
                 * signals @p fence so the CPU can wait on completion.
                 *
                 * @param commandBuffer Recorded command buffer to execute.
                 * @param waitSemaphore Semaphore to wait on before executing.
                 * @param waitStage Pipeline stage at which the wait occurs.
                 * @param signalSemaphore Semaphore signaled when execution completes.
                 * @param fence Fence signaled when execution completes.
                 * @throws std::runtime_error If vkQueueSubmit fails.
                 */
                void submit(const command::VulkanCommandBuffer& commandBuffer, const sync::VulkanSemaphore& waitSemaphore,
                    const VkPipelineStageFlags waitStage, const sync::VulkanSemaphore& signalSemaphore, const sync::VulkanFence& fence) const;

                /**
                 * @brief Queues a swapchain image for presentation.
                 *
                 * Presentation waits on @p waitSemaphore, typically the
                 * semaphore signaled by the rendering submission.
                 *
                 * @param swapchain Swapchain that owns the image.
                 * @param imageIndex Index of the swapchain image to present.
                 * @param waitSemaphore Semaphore to wait on before presenting.
                 * @return VK_SUCCESS, or VK_ERROR_OUT_OF_DATE_KHR /
                 *         VK_SUBOPTIMAL_KHR when the swapchain should be recreated.
                 * @throws std::runtime_error For any other presentation failure.
                 */
                [[nodiscard]]
                VkResult present(const window::VulkanSwapchain& swapchain, uint32_t imageIndex, const sync::VulkanSemaphore& waitSemaphore) const;

                void waitIdle() const;

                [[nodiscard]]
                VkQueue handle() const noexcept;

            private:
                VkQueue queue = VK_NULL_HANDLE;
        };
    }
}