#pragma once

#include <vulkan/vulkan.h>

#include <string>
#include <string_view>
#include <stdexcept>

inline void vkCheck(VkResult result, std::string_view message)
{
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error(std::string(message));
    }
}