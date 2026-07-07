#include <format>

#include <projectV/engine/logging/log.hpp>

#include <vulkan/util/vulkanCheck.hpp>

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

        VkCommandBuffer VulkanCommandBuffer::handle() const noexcept
        {
            return commandBuffer;
        }
    }
}
