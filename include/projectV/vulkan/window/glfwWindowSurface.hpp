#pragma once

#include <vulkan/window/windowSurface.hpp>

namespace projectv
{
    namespace vulkan::window
    {
        class GlfwWindowSurface : public IWindowSurface
        {
        public:
            void createSurface(VkInstance instance, void* windowNativehandle,  VkSurfaceKHR& surface) const override;
        };
    }
}
