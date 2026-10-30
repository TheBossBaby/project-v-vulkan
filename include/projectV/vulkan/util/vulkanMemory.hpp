#pragma once

#include <vulkan/vulkan.h>

namespace projectv
{
    namespace vulkan::util
    {
        /**
         * @brief Finds a memory type index allowed by @p typeFilter that
         *        has all of the configured memory properties.
         * @param physicalDevice Physical device to query.
         * @param typeFilter     Bitmask of acceptable memory type indices,
         *                       from VkMemoryRequirements::memoryTypeBits.
         * @param properties     Required properties of the backing memory.
         * @return Index of the first matching memory type.
         *
         * @throws std::runtime_error if no suitable memory type exists.
        */
        uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
    }
}
