#pragma once
#include <span>

namespace projectv
{
    namespace vulkan::util
    {
        class IWindowExtension
        {
            public:
                virtual std::span<const char* const> Extensions() const = 0;

                virtual ~IWindowExtension() = default;
        };
    }
}