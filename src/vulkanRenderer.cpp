#include <projectV/core/transform.hpp>
#include <projectV/core/shaderHandle.hpp>
#include <projectV/core/meshHandle.hpp>

#include <projectV/engine/graph/scene/ecs/world.hpp>
#include <projectV/engine/logging/log.hpp>

#include <vulkan/vulkan.h>

#include <vulkan/builder/VulkanRenderPassBuilder.hpp>
#include <vulkan/builder/vulkanFramebufferBuilder.hpp>

#include <vulkan/config/vulkanConfig.hpp>
#include <vulkan/vulkanFactory.hpp>
#include <vulkan/vulkanRenderer.hpp>
#include <vulkan/rendererResources.hpp>

#include <vulkan/command/vulkanCommandBuffer.hpp>
#include <vulkan/command/vulkanCommandPool.hpp>

#include <vulkan/device/vulkanDevice.hpp>
#include <vulkan/device/vulkanInstance.hpp>
#include <vulkan/device/vulkanPhysicalDevice.hpp>
#include <vulkan/device/vulkanQueue.hpp>

#include <vulkan/util/windowExtension.hpp>

#include <vulkan/window/vulkanSurface.hpp>
#include <vulkan/window/vulkanSwapchain.hpp>

namespace projectv
{
    vulkan::vulkanRenderer::vulkanRenderer()
    {
        renderResources = std::make_unique<RendererResources>();
    }

    vulkan::vulkanRenderer::~vulkanRenderer()
    {
        window = nullptr;
    }

    bool vulkan::vulkanRenderer::init(const core::RendererConfig &config, core::IWindow& inWindow)
    {
        engine::LogInfo("vulkan::vulkanRenderer::init");

        window = &inWindow;
        instance = VulkanFactory::createInstance();
        windowExtension = VulkanFactory::createWindowExtension();
        windowSurfaceProvider = VulkanFactory::createWindowSurfaceProvider();
        windowSurface = VulkanFactory::createSurface();  
        physicalDevice = VulkanFactory::createPhysicalDevice();
        logicalDevice = VulkanFactory::createLogicalDevice();
        graphicsCommandPool = VulkanFactory::createCommandPool();
        graphicsFence = VulkanFactory::createFence();
        swapchain= VulkanFactory::createSwapchain();

        instance->create(*windowExtension.get());
        
        windowSurface->create(instance->handle(), *windowSurfaceProvider.get(), *window);
        physicalDevice->select(instance->handle(), windowSurface->handle(), config::DeviceExtensions);
        logicalDevice->create(physicalDevice->handle(), physicalDevice->queueFamilies(), config::DeviceExtensions);
        swapchain->create(*physicalDevice.get(), logicalDevice->handle(), windowSurface->handle(), window->getWidth(), window->getHeight());
        
        createHelloTriangleRenderPass();

        graphicsCommandPool->create(logicalDevice->handle(), physicalDevice->queueFamilies().graphics.value());
        graphicsFence->create(logicalDevice->handle());

        command::VulkanCommandBuffer testCommandBuffer = graphicsCommandPool->allocate(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
        testCommandBuffer.begin();
        testCommandBuffer.end();

        engine::LogInfo("vulkan::vulkanRenderer::Submiting");

        logicalDevice->graphicsQueue().submit(testCommandBuffer, *graphicsFence.get());
        engine::LogInfo("vulkan::vulkanRenderer::Submitted");
        engine::LogInfo("vulkan::vulkanRenderer::Wait start");

        
        graphicsFence->wait();
        engine::LogInfo("vulkan::vulkanRenderer::Reseting");

        graphicsFence->reset();
        engine::LogInfo("vulkan::vulkanRenderer::Reset done");        
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

    void vulkan::vulkanRenderer::createHelloTriangleRenderPass()
    {
        vulkan::builder::VulkanRenderPassBuilder builder;

        uint32_t colorIdx = builder.addColorAttachment(swapchain->format().format);

        uint32_t subpass = builder.beginSubpass();
        builder.addColorRef(subpass, colorIdx);

        builder.addDependency(
            VK_SUBPASS_EXTERNAL, subpass,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT);

        renderResources->renderPass() = builder.build(logicalDevice->handle());

        // Create 1 Framebuffer per image view of swapchain used in render pass
        const VkRenderPass renderPass = renderResources->renderPass().handle();
        const VkExtent2D extent = swapchain->extent();

        auto& framebuffers = renderResources->framebuffers();
        framebuffers.reserve(swapchain->imageViews().size());

        for (const auto& imageView : swapchain->imageViews())
        {
            builder::VulkanFramebufferBuilder fbBuilder;

            fbBuilder
                .setRenderPass(renderResources->renderPass().handle())
                .addAttachment(imageView.handle())
                .setExtent(swapchain->extent().width, swapchain->extent().height)
                .setLayers(1);

            framebuffers.push_back(fbBuilder.build(logicalDevice->handle()));
        }      
    }
}