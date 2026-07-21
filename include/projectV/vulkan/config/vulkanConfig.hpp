#pragma once
#include <vulkan/vulkan.h>

#include <vector>

namespace projectv
{
    namespace vulkan::config
    {
        const uint32_t VulkanVersion = VK_API_VERSION_1_3;

        /**
        * @brief List of required Device Extensions
        * 
        */
        const std::vector<const char *> DeviceExtensions = 
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        /**
        * @brief Prefered Swapchain Format
        * 
        */
        const VkFormat SwapchainFormat = VK_FORMAT_B8G8R8A8_SRGB;

        /**
        * @brief Prefered Swapchain Color Space
        * 
        */
        const VkColorSpaceKHR SwapchainColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

        /**
        * @brief Prefered Swapchain PresentMode
        * 
        */
        const VkPresentModeKHR SwapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;

        /**
        * @brief PresentMode when prefered swapchain is not supported.
        * 
        */
        const VkPresentModeKHR FallBackSwapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

        /**
        * @brief Amount of layers each image consists of. This is always 1 unless you are developing a stereoscopic 3D application.
        * 
        */
        const uint32_t SwapchainImageArrayLayers = 1;
    }
}