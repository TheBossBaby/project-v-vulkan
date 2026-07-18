#pragma once

#include <optional>

namespace projectv
{
    namespace vulkan::types
    {
        struct QueueFamilies
        {
            std::optional<uint32_t> graphics;

            std::optional<uint32_t> presentation;

            bool isComplete() const
            {
                return graphics.has_value() && presentation.has_value();
            }
        };
    }
}
