#include <vulkan/util/vulkanCheck.hpp>

#include <synchronization/vulkanFence.hpp>

namespace projectv
{
    namespace vulkan::sync
    {
        VulkanFence::~VulkanFence()
        {
            destroy();
        }

        void VulkanFence::create(VkDevice device)
        {
            this->device = device;

            VkFenceCreateInfo fenceInfo{};
            fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

            vkCheck(vkCreateFence(this->device, &fenceInfo, nullptr, &fence), "VulkanFence::create, Failed to create fence!");
        }

        void VulkanFence::destroy()
        {
            if(device != VK_NULL_HANDLE && fence != VK_NULL_HANDLE) vkDestroyFence(device, fence, nullptr);

            fence = VK_NULL_HANDLE;
            device = VK_NULL_HANDLE;
        }

        void VulkanFence::wait()
        {
            vkCheck(vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX), "VulkanFence::wait, Failed to wait for fence!");
        }

        void VulkanFence::reset()
        {
            vkCheck(vkResetFences(device, 1, &fence), "VulkanFence::reset, Failed to rest fence!");
        }

        VkFence VulkanFence::handle() const noexcept
        {
            return fence;
        }
    }
}
