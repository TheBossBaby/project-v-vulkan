#pragma once

#include <projectV/core/renderer.hpp>

#include <vector>

namespace projectv
{

    namespace vulkan
    {
        namespace command
        {
            class VulkanCommandPool;
        }

        namespace device
        {
            class VulkanInstance;
            class VulkanPhysicalDevice;
            class VulkanDevice;
        }

        namespace sync
        {
            class VulkanSemaphore;
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

        struct FrameData;

        class vulkanRenderer : public core::IRenderer
        {
        public:
            vulkanRenderer();

            ~vulkanRenderer();

            bool init(const core::RendererConfig& config, core::IWindow& inWindow) override;

            void resize(std::uint32_t width, std::uint32_t height) override;

            void beginFrame(const core::RenderView& renderView) override;

            void draw(const std::vector<core::Renderable>& items) override;

            /**
             * @brief Uploads a CPU mesh into host-visible vertex and index
             *        buffers and registers it with the renderer resources.
             * @param mesh Mesh whose vertices and indices are copied to the GPU.
             * @return Handle used to reference the uploaded mesh when drawing.
             *
             * @throws std::runtime_error if buffer creation or memory allocation fails.
             */
            core::gpu::GpuMeshHandle uploadMesh(const struct core::Mesh& mesh) override;

            core::GraphicsPipelineHandle createGraphicsPipeline(const core::GraphicsPipelineDescription& description) override;

            void endFrame() override;

            void waitIdle() override;

        private:
            void createSwapchainRenderPass();

            /**
             * @brief Creates the depth image and its view and stores them in
             *        the renderer resources.
             *
             * Picks the first supported depth format, then creates an image
             * matching the swapchain extent and a 2D view of its depth aspect.
             * Must run after the swapchain is created and before
             * createSwapchainRenderPass() and createSwapchainFramebuffers(),
             * which read the depth format and view.
             *
             * @throws std::runtime_error if no candidate format is supported,
             *         or if creating the image or its view fails.
             */
            // TODO(resize)
            void createDepthBuffer();

            void createSwapchainFramebuffers();

            void createRenderFinishedSemaphores();

            void createFrameData();
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

            std::unique_ptr<window::VulkanSwapchain> swapchain;

            std::unique_ptr<RendererResources> renderResources;

            std::unique_ptr<util::VulkanValidationLayerManager> vulkanValidationLayerManager;

            core::IWindow* window = nullptr;

            std::vector<sync::VulkanSemaphore> renderFinishedSemaphoreList;

            uint32_t currentImageIndex = UINT32_MAX;

            uint32_t currentFrame = 0;

            bool isFrameRunning = false;

            std::vector<FrameData> frames;

            core::math::Matrix4 viewProj;
        };
    }
}