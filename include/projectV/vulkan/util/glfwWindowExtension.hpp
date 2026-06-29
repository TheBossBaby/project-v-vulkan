#pragma once

#include <windowExtension.hpp>
#include <cstdint>
#include <span>
#include <vector>

namespace projectv
{
    namespace vulkan::util
    {
        class GlfwWindowExtension : public IWindowExtension
        {
        public:
            std::span<const char* const> Extensions() const override;
        };
    }
}