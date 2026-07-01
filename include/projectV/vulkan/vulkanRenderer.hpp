#pragma once

#include <projectV/core/renderer.hpp>

namespace projectv
{
    
    namespace engine {class World;}

    namespace vulkan
    {
        namespace device
        {
            class VulkanInstance;
            class VulkanPhysicalDevice;
            class VulkanDevice;
        }
        namespace util { class IWindowExtension;}

        class vulkanRenderer : public core::IRenderer
        {
        public:
            vulkanRenderer();

            ~vulkanRenderer();

            bool init(const core::RendererConfig& config) override;

            void resize(std::uint32_t width, std::uint32_t height) override;

            void beginFrame(const core::RenderView& renderView) override;

            void draw(const std::vector<core::Renderable>& items) override;

            void endFrame() override;

            void waitIdle() override;
        private:
            std::unique_ptr<device::VulkanInstance> instance;

            std::unique_ptr<util::IWindowExtension> windowExtension;

            std::unique_ptr<device::VulkanPhysicalDevice> physicalDevice;

            std::unique_ptr<device::VulkanDevice> logicalDevice;
        };
    }
}