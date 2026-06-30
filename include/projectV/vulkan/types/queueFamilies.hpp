#pragma once

#include <optional>

namespace projectv
{
    namespace vulkan::types
    {
        struct QueueFamilies
        {
            std::optional<uint32_t> graphics;

            bool isComplete() const
            {
                return graphics.has_value();
            }
        };
    }
}
