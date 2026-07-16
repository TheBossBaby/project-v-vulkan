#pragma once

#include <projectV/core/window.hpp>
#include <vulkan/window/windowSurface.hpp>

#include <vulkan/vulkan.h>

namespace projectv
{
    namespace vulkan::window
    {
        class VulkanSurface
        {
        public:
            VulkanSurface();

            void create(VkInstance instance, IWindowSurface& windowSurfaceProvider, core::IWindow& window);

            void destroy();

            VkSurfaceKHR handle() const noexcept;

            ~VulkanSurface();
        private:
            VkInstance m_instance = VK_NULL_HANDLE;

            VkSurfaceKHR m_surface = VK_NULL_HANDLE;  
        };
    }
}