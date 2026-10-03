#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/util/windowExtension.hpp>
#include <vulkan/util/vulkanValidationLayerManager.hpp>

namespace projectv
{
    namespace vulkan::device
    {
        /**
        * @brief Connection between your application and the Vulkan library.
        * 
         */
        class VulkanInstance
        {
        public:
            VulkanInstance();
            
            ~VulkanInstance();

            /**
            * @brief Create vulkan Instance.
            */
            void create(const vulkan::util::IWindowExtension& windowExtension, vulkan::util::VulkanValidationLayerManager& validationLayerManager);

            /**
            * @brief Destroy vulkan Instance.
            */
            void destroy();

            /**
            * @brief Returns the Handle to Vulkan Instance
            * 
            * @return Vulkan Instance Handle
            */
            VkInstance handle() const noexcept;

            /**
            * @brief Whether validation layers and VK_EXT_debug_utils were enabled on this instance.
            */
            bool validationEnabled() const noexcept;
            private:
            VkInstance instance = VK_NULL_HANDLE;

            bool m_validationEnabled = false;
        };       
    }
}