#pragma once

#include <vulkan/vulkan.h>

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
            void create(const class IWindowExtension& windowExtension);

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
            private:
            VkInstance instance = VK_NULL_HANDLE;
        };       
    }
}