#pragma once

#include <vulkan/vulkan.h>

namespace projectv
{
    namespace vulkan::command { class VulkanCommandBuffer ; }
    namespace vulkan::sync { class VulkanFence; }
    
    namespace vulkan::device
    {
        class VulkanQueue
        {
            public:
                void create(VkQueue inQueue);

                void submit(const command::VulkanCommandBuffer& commandBuffer, const sync::VulkanFence& fence) const;

                void waitIdle() const;

                [[nodiscard]]
                VkQueue handle() const noexcept;

            private:
                VkQueue queue = VK_NULL_HANDLE;
        };
    }
}