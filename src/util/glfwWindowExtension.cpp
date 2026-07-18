#include "glfwWindowExtension.hpp"
#include <GLFW/glfw3.h>
#include <projectV/engine/logging/log.hpp>
namespace projectv
{
    namespace vulkan::util
    {
        std::span<const char* const> GlfwWindowExtension::Extensions() const
        {
            uint32_t extensionCount = 0;
            const char* const* extensions = glfwGetRequiredInstanceExtensions(&extensionCount);
            return { extensions, extensionCount };
        }
    }
}