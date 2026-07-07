#include <format>

#include <projectV/engine/logging/log.hpp>

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
            engine::LogInfo(std::format("VulkanCommandBuffer::begin, Command Buffer {} start", static_cast<void*>(commandBuffer)));
        }

        void VulkanCommandBuffer::end()
        {
            engine::LogInfo("VulkanCommandBuffer::end, Command Buffer end");
        }

        VkCommandBuffer VulkanCommandBuffer::handle() const noexcept
        {
            return commandBuffer;
        }
    }
}
