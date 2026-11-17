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
        * @brief Validation layers and the debug messenger are only enabled in Debug builds.
        *
        */
#ifdef NDEBUG
        constexpr bool EnableValidationLayers = false;
#else
        constexpr bool EnableValidationLayers = true;
#endif

        /**
        * @brief List of required Validation Layers
        * 
        */
        const std::vector<const char*> ValidationLayers = 
        {
            "VK_LAYER_KHRONOS_validation"
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

        /**
        * @brief Maximum number of frame to run in flight
        * TODO : Store this is Core::Config as this will be API agnostic
        * 
        */
        constexpr std::uint32_t MaxFramesInFlight = 2;

        /**
        * @brief Descriptor set index of the per-frame set (shader: layout(set = N)).
        * Must match the order the set layout is added to the pipeline layout.
        */
        constexpr uint32_t FrameSetIndex = 0;

        /**
        * @brief Binding index of the camera uniforms within the per-frame set (shader: layout(binding = N)).
        */
        constexpr uint32_t CameraBindingIndex = 0;
    }
}