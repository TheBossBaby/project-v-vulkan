#pragma once

#include <projectV/core/renderer.hpp>

namespace projectv
{
    
    namespace engine {class World;}

    namespace vulkan
    {
        class vulkanRenderer : public core::IRenderer
        {
        public:
            vulkanRenderer();

            ~vulkanRenderer();

            bool init(const core::RendererConfig& config) override;

            void resize(std::uint32_t width, std::uint32_t height) override;

            void beginFrame() override;

            void renderScene(engine::World& world);

            void endFrame() override;

            void waitIdle() override;
        };
    }
}