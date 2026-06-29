#include "glfwWindowExtension.hpp"
#include <GLFW/glfw3.h>

namespace projectv
{
    namespace vulkan::util
    {
        std::span<const char* const> Extensions() const
        {
            uint32_t extensionCount = 0;
            return glfwGetRequiredInstanceExtensions(&extensionCount);
        }
    }
}