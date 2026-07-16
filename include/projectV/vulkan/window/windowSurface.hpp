#pragma once

#include <vulkan/vulkan.h>

namespace projectv
{
    namespace vulkan::window
    {
        class IWindowSurface
        {
        public:

            virtual ~IWindowSurface() = default;

            virtual void createSurface(VkInstance instance, void* windowNativehandle,  VkSurfaceKHR& surface) const = 0;
        };
    }
}
