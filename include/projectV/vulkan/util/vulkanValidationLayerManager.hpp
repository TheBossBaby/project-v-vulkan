#pragma once
#include <vector>

namespace projectv
{
    namespace core
    {
        class ILogger;
    }

    namespace vulkan::util
    {
        class VulkanValidationLayerManager
        {
            public:
                VulkanValidationLayerManager();

                bool enableValidationLayer(const std::vector<const char*>& inValidationLayer);
            private:
                bool isSupported(const std::vector<const char*>& inValidationLayer);
        };
    }
}