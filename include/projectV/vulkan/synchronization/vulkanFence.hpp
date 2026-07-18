#pragma once

#include <vulkan/vulkan.h>

namespace projectv
{
    namespace vulkan::sync
    {
        class VulkanFence
        {
        public:
            ~ VulkanFence();

            void create(VkDevice device);

            void destroy();

            void wait();

            void reset();

            VkFence handle() const noexcept;

        private:

            VkDevice device = VK_NULL_HANDLE;

            VkFence fence = VK_NULL_HANDLE;
        };
    }
}