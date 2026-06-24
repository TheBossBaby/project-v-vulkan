#include <projectV/core/transform.hpp>
#include <projectV/core/shaderHandle.hpp>
#include <projectV/core/meshHandle.hpp>

#include <projectV/engine/graph/scene/ecs/world.hpp>
#include <projectV/engine/logging/log.hpp>

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
        engine::LogInfo("init");
        return true;
    }

    void vulkan::vulkanRenderer::resize(std::uint32_t width, std::uint32_t height)
    {
        engine::LogInfo("resize");
    }

    void vulkan::vulkanRenderer::beginFrame(const core::RenderView& renderView)
    {
        engine::LogInfo("beginFrame");
    }

    void vulkan::vulkanRenderer::draw(const std::vector<core::Renderable>& items)
    {
        engine::LogInfo("draw");
    }

    void vulkan::vulkanRenderer::endFrame()
    {
        engine::LogInfo("endFrame");
    }

    void vulkan::vulkanRenderer::waitIdle()
    {
        engine::LogInfo("waitIdle");
    }
}