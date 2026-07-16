#include <vulkanSurface.hpp>

namespace projectv
{
    namespace vulkan::window
    {
        VulkanSurface::VulkanSurface()
        {
        }

        void VulkanSurface::create(VkInstance instance, IWindowSurface& windowSurfaceProvider, core::IWindow& window)
        {
            m_instance = instance;
            windowSurfaceProvider.createSurface(m_instance, window.nativeHandle(), m_surface);
        }

        void VulkanSurface::destroy()
        {
            if((m_instance != VK_NULL_HANDLE) && (m_surface != VK_NULL_HANDLE))
            {
                vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
                
                m_instance = VK_NULL_HANDLE;
                m_surface = VK_NULL_HANDLE; 
            }
        }

        VkSurfaceKHR VulkanSurface::handle() const noexcept
        {
            return m_surface;
        }

        VulkanSurface::~VulkanSurface()
        {
            destroy();
        }
    }
}