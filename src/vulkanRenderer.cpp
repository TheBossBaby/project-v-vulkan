#include <projectV/core/transform.hpp>
#include <projectV/core/shaderHandle.hpp>
#include <projectV/core/meshHandle.hpp>

#include <projectV/engine/graph/scene/ecs/world.hpp>
#include <projectV/engine/logging/log.hpp>

#include <vulkan/vulkanFactory.hpp>
#include <vulkan/device/vulkanInstance.hpp>
#include <vulkan/util/windowExtension.hpp>
#include <vulkan/vulkanRenderer.hpp>

namespace projectv
{
    vulkan::vulkanRenderer::vulkanRenderer()
    {
    }

    vulkan::vulkanRenderer::~vulkanRenderer()
    {
    }

    bool vulkan::vulkanRenderer::init(const core::RendererConfig &config)
    {
        engine::LogInfo("vulkan::vulkanRenderer::init");

        instance = VulkanFactory::createInstance();
        windowExtension = VulkanFactory::createWindowExtension();    
        instance->create(*windowExtension.get());

        return true;
    }

    void vulkan::vulkanRenderer::resize(std::uint32_t width, std::uint32_t height)
    {
        engine::LogInfo("resize");
    }

    void vulkan::vulkanRenderer::beginFrame(const core::RenderView& renderView)
    {
    }

    void vulkan::vulkanRenderer::draw(const std::vector<core::Renderable>& items)
    {
    }

    void vulkan::vulkanRenderer::endFrame()
    {
    }

    void vulkan::vulkanRenderer::waitIdle()
    {
    }
}