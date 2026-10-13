#include <vulkan/util/vulkanCheck.hpp>

#include <synchronization/vulkanSemaphore.hpp>

namespace projectv
{
    namespace vulkan::sync
    {
        VulkanSemaphore::VulkanSemaphore(VulkanSemaphore &&other) noexcept
            : device(other.device)
            , semaphore(other.semaphore)
        {
            other.device = VK_NULL_HANDLE;
            other.semaphore   = VK_NULL_HANDLE;
        }

        VulkanSemaphore &VulkanSemaphore::operator=(VulkanSemaphore &&other) noexcept
        {
            if (this != &other)
            {
                destroy();

                device = other.device;
                semaphore   = other.semaphore;

                other.device = VK_NULL_HANDLE;
                other.semaphore   = VK_NULL_HANDLE;
            }
            return *this;
        }

        VulkanSemaphore::~VulkanSemaphore()
        {
            destroy();
        }

        void VulkanSemaphore::create(VkDevice device)
        {
            destroy();
            this->device = device;

            VkSemaphoreCreateInfo semaphoreInfo{};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            vkCheck(vkCreateSemaphore(this->device, &semaphoreInfo, nullptr, &semaphore), "VulkanSemaphore::create, Failed to create semaphore!");
        }

        void VulkanSemaphore::destroy()
        {
            if(device != VK_NULL_HANDLE && semaphore != VK_NULL_HANDLE) vkDestroySemaphore(device, semaphore, nullptr);

            semaphore = VK_NULL_HANDLE;
            device = VK_NULL_HANDLE;
        }

        VkSemaphore VulkanSemaphore::handle() const noexcept
        {
            return semaphore;
        }
    }
}
