#pragma once

#include <vulkan/vulkan.h>

namespace projectv
{
    namespace vulkan::util
    {
        /**
        * @brief Routes validation layer messages to the engine logger.
        *
        * Warnings go to engine::LogWarn, errors to engine::LogError.
        * Verbose and info messages are not requested.
        */
        class VulkanDebugMessenger
        {
        public:
            VulkanDebugMessenger();

            ~VulkanDebugMessenger();

            /**
            * @brief Fills a create info pointing at the logging callback.
            *
            * Also chained into VkInstanceCreateInfo::pNext so messages from
            * vkCreateInstance/vkDestroyInstance are caught, which happen
            * outside the lifetime of the messenger itself.
            */
            static void populateCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

            /**
            * @brief Create the debug messenger.
            *
            * @param instance Instance created with VK_EXT_debug_utils enabled.
            *                 Must outlive this messenger.
            */
            void create(VkInstance instance);

            /**
            * @brief Destroy the debug messenger. Must be called before the instance is destroyed.
            */
            void destroy();

            /**
            * @brief Returns the Handle to Vulkan Debug Messenger
            *
            * @return Vulkan Debug Messenger Handle
            */
            VkDebugUtilsMessengerEXT handle() const noexcept;

        private:
            VkInstance instance = VK_NULL_HANDLE;

            VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;
        };
    }
}
