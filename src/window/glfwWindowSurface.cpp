#include <glfwWindowSurface.hpp>
#include <GLFW/glfw3.h>

#include <vulkan/util/vulkanCheck.hpp>

namespace projectv
{
    namespace vulkan::window
    {
        void GlfwWindowSurface::createSurface(VkInstance instance, void* windowNativehandle,  VkSurfaceKHR& surface) const
        {
            vkCheck(glfwCreateWindowSurface(instance, static_cast<GLFWwindow*>(windowNativehandle), nullptr, &surface),
            "GlfwWindowSurface::createSurface, Failed to create window surface!");
        }
    }
}
