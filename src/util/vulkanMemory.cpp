#include <projectV/engine/logging/log.hpp>
#include <vulkan/util/vulkanMemory.hpp>

#include <stdexcept>

namespace projectv
{
    namespace vulkan::util
    {
        uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
        {
            VkPhysicalDeviceMemoryProperties memProperties;
            vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

            for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) 
            {
                if ((typeFilter & (1u << i)) && 
                    (memProperties.memoryTypes[i].propertyFlags & properties) == properties) 
                {
                    return i;
                }
            }

            projectv::engine::LogError("projectv::vulkan::util::findMemoryType, failed to find suitable memory type!");
            throw std::runtime_error("projectv::vulkan::util::findMemoryType, failed to find suitable memory type!");
        }
    }
}
