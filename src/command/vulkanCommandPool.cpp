#include <projectV/engine/logging/log.hpp>
#include <vulkan/util/vulkanCheck.hpp>

#include <vulkanCommandPool.hpp>

namespace projectv
{
    namespace vulkan::command
    {
        VulkanCommandPool::~VulkanCommandPool()
        {
            destroy();
        }

        void VulkanCommandPool::create(VkDevice inDevice, uint32_t inQueueFamilyIndex)
        {
            device = inDevice;
            engine::LogInfo("VulkanCommandPool::create, Creating command pool");

            VkCommandPoolCreateInfo poolInfo{};
            poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            poolInfo.queueFamilyIndex = inQueueFamilyIndex;

            vkCheck(vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool), 
            "VulkanCommandPool::create, Failed to create command pool!");
        }

        void VulkanCommandPool::destroy()
        {
            if(commandPool != VK_NULL_HANDLE && device != VK_NULL_HANDLE)
            {
                engine::LogInfo("VulkanCommandPool::destroy, Destroying command pool");

                vkDestroyCommandPool(device, commandPool, nullptr);
                commandPool = VK_NULL_HANDLE;
                device = VK_NULL_HANDLE;
            }
        }

        VkCommandPool VulkanCommandPool::handle() const noexcept
        {
            return commandPool;
        }
    }
}