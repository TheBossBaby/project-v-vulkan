#pragma once

#include <projectV/core/renderer.hpp>

namespace projectv
{

    namespace vulkan
    {
        namespace command
        {
            class VulkanCommandPool;
            class VulkanCommandBuffer;
        }

        namespace device
        {
            class VulkanInstance;
            class VulkanPhysicalDevice;
            class VulkanDevice;
        }

        namespace sync
        {
            class VulkanFence;
        }

        namespace util 
        { 
            class IWindowExtension;
            class VulkanValidationLayerManager;
            class VulkanDebugMessenger;
        }

        namespace window 
        {
            class IWindowSurface; 
            class VulkanSurface;
            class VulkanSwapchain;
        }

        class RendererResources;

        class vulkanRenderer : public core::IRenderer
        {
        public:
            vulkanRenderer();

            ~vulkanRenderer();

            bool init(const core::RendererConfig& config, core::IWindow& inWindow) override;

            void resize(std::uint32_t width, std::uint32_t height) override;

            void beginFrame(const core::RenderView& renderView) override;

            void draw(const std::vector<core::Renderable>& items) override;

            core::GraphicsPipelineHandle createGraphicsPipeline(const core::GraphicsPipelineDescription& description) override;

            void endFrame() override;

            void waitIdle() override;

            void createHelloTriangleRenderPass();

        private:
            std::unique_ptr<device::VulkanInstance> instance;

            // Declared right after instance so it is destroyed just before it.
            std::unique_ptr<util::VulkanDebugMessenger> debugMessenger;

            std::unique_ptr<util::IWindowExtension> windowExtension;
            
            std::unique_ptr<window::IWindowSurface> windowSurfaceProvider;

            std::unique_ptr<window::VulkanSurface> windowSurface;

            std::unique_ptr<device::VulkanPhysicalDevice> physicalDevice;

            std::unique_ptr<device::VulkanDevice> logicalDevice;

            std::unique_ptr<command::VulkanCommandPool> graphicsCommandPool;

            std::unique_ptr<sync::VulkanFence> graphicsFence;

            std::unique_ptr<window::VulkanSwapchain> swapchain;

            std::unique_ptr<RendererResources> renderResources;

            std::unique_ptr<util::VulkanValidationLayerManager> vulkanValidationLayerManager;

            core::IWindow* window = nullptr;
        };
    }
}