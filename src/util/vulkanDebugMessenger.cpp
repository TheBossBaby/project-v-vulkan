#include <vulkan/util/vulkanDebugMessenger.hpp>
#include <vulkan/util/vulkanCheck.hpp>

#include <projectV/engine/logging/log.hpp>

#include <stdexcept>
#include <string>

namespace projectv
{
    namespace vulkan::util
    {
        namespace
        {
            VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
                VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT,
                const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
                void*)
            {
                std::string message = "[Vulkan] ";
                message += callbackData->pMessage;

                if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
                    engine::LogError(message);
                else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
                    engine::LogWarn(message);
                else
                    engine::LogInfo(message);

                // VK_FALSE: never abort the Vulkan call that triggered the message.
                return VK_FALSE;
            }
        }

        VulkanDebugMessenger::VulkanDebugMessenger()
        {
        }

        VulkanDebugMessenger::~VulkanDebugMessenger()
        {
            destroy();
        }

        void VulkanDebugMessenger::populateCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
        {
            createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            createInfo.messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            createInfo.messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            createInfo.pfnUserCallback = debugCallback;
        }

        void VulkanDebugMessenger::create(VkInstance inInstance)
        {
            // Extension function: not exported by the loader, must be looked up.
            auto createFn = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
                vkGetInstanceProcAddr(inInstance, "vkCreateDebugUtilsMessengerEXT"));
            if (!createFn)
            {
                engine::LogError("VulkanDebugMessenger, VK_EXT_debug_utils is not enabled on the instance");
                throw std::runtime_error("VulkanDebugMessenger, VK_EXT_debug_utils is not enabled on the instance");
            }

            VkDebugUtilsMessengerCreateInfoEXT createInfo;
            populateCreateInfo(createInfo);

            vkCheck(createFn(inInstance, &createInfo, nullptr, &messenger),
            "Failed to create Vulkan Debug Messenger");

            instance = inInstance;
        }

        void VulkanDebugMessenger::destroy()
        {
            if (messenger != VK_NULL_HANDLE)
            {
                auto destroyFn = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
                    vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
                if (destroyFn)
                    destroyFn(instance, messenger, nullptr);

                messenger = VK_NULL_HANDLE;
                instance = VK_NULL_HANDLE;
            }
        }

        VkDebugUtilsMessengerEXT VulkanDebugMessenger::handle() const noexcept
        {
            return messenger;
        }
    }
}
