#pragma once

#include <projectV/core/renderer.hpp>
#include <vulkan/rendererResources/vulkanGraphicsPipeline.hpp>

namespace projectv
{

    namespace engine
    {
        class World;
        class ShaderManager;
    }

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

        namespace util { class IWindowExtension; }

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

            /**
             * @brief Supplies the ShaderManager used to resolve shader handles
             *        passed to createGraphicsPipeline().
             *
             * Must be called before any createGraphicsPipeline() call. Not part
             * of the core::IRenderer interface, since core must not depend on
             * the engine layer; callers that construct a vulkanRenderer
             * concretely (as GameApp does) call this directly.
             *
             * @param shaderManager Engine-side shader storage. Borrowed; must
             *                      outlive this renderer.
             */
            void setShaderManager(engine::ShaderManager& shaderManager);

        private:
            std::unique_ptr<device::VulkanInstance> instance;

            std::unique_ptr<util::IWindowExtension> windowExtension;
            
            std::unique_ptr<window::IWindowSurface> windowSurfaceProvider;

            std::unique_ptr<window::VulkanSurface> windowSurface;

            std::unique_ptr<device::VulkanPhysicalDevice> physicalDevice;

            std::unique_ptr<device::VulkanDevice> logicalDevice;

            std::unique_ptr<command::VulkanCommandPool> graphicsCommandPool;

            std::unique_ptr<sync::VulkanFence> graphicsFence;

            std::unique_ptr<window::VulkanSwapchain> swapchain;

            std::unique_ptr<RendererResources> renderResources;

            core::IWindow* window = nullptr;

            engine::ShaderManager* shaderManager = nullptr;
        };
    }
}