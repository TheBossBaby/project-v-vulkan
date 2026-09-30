#include <vulkan/rendererFactory.hpp>
#include <vulkan/vulkanRenderer.hpp>

namespace projectv::vulkan
{
    std::unique_ptr<core::IRenderer> createRenderer()
    {
        return std::make_unique<vulkanRenderer>();
    }
}
