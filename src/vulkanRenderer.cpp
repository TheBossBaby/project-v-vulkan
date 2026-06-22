#include <projectV/core/transform.hpp>
#include <projectV/core/shaderHandle.hpp>
#include <projectV/core/meshHandle.hpp>

#include <projectV/engine/graph/scene/ecs/world.hpp>

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
        return true;
    }

    void vulkan::vulkanRenderer::resize(std::uint32_t width, std::uint32_t height)
    {
    }

    void vulkan::vulkanRenderer::beginFrame()
    {
    }

    void vulkan::vulkanRenderer::renderScene(engine::World &world)
    {
        auto renderView = world.view<
        core::Transform,
        core::ShaderHandle,
        core::MeshHandle>();
    }

    void vulkan::vulkanRenderer::endFrame()
    {
    }

    void vulkan::vulkanRenderer::waitIdle()
    {
    }
}