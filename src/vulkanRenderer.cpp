#include <projectV/core/transform.hpp>
#include <projectV/core/shader.hpp>
#include <projectV/core/shaderHandle.hpp>
#include <projectV/core/meshHandle.hpp>

#include <projectV/engine/logging/log.hpp>

#include <vulkan/vulkan.h>

#include <vulkan/builder/VulkanRenderPassBuilder.hpp>
#include <vulkan/builder/vulkanFramebufferBuilder.hpp>
#include <vulkan/builder/vulkanShaderModuleBuilder.hpp>
#include <vulkan/builder/vulkanPipelineLayoutBuilder.hpp>
#include <vulkan/builder/vulkanGraphicsPipelineBuilder.hpp>

#include <vulkan/config/vulkanConfig.hpp>
#include <vulkan/vulkanFactory.hpp>
#include <vulkan/vulkanRenderer.hpp>
#include <vulkan/rendererResources.hpp>
#include <vulkan/frameData.hpp>

#include <vulkan/command/vulkanCommandPool.hpp>

#include <vulkan/device/vulkanDevice.hpp>
#include <vulkan/device/vulkanInstance.hpp>
#include <vulkan/device/vulkanPhysicalDevice.hpp>
#include <vulkan/device/vulkanQueue.hpp>

#include <vulkan/util/windowExtension.hpp>
#include <vulkan/util/vulkanCheck.hpp>
#include <vulkan/util/vulkanValidationLayerManager.hpp>
#include <vulkan/util/vulkanDebugMessenger.hpp>

#include <vulkan/window/vulkanSurface.hpp>
#include <vulkan/window/vulkanSwapchain.hpp>

#include <utility>

namespace projectv
{
    vulkan::vulkanRenderer::vulkanRenderer()
    {
        renderResources = std::make_unique<RendererResources>();
    }

    vulkan::vulkanRenderer::~vulkanRenderer()
    {
        try
        {
            waitIdle();
        }
        catch (const std::exception& e)
        {
            engine::LogError(e.what());
        }
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
        swapchain= VulkanFactory::createSwapchain();
        vulkanValidationLayerManager = VulkanFactory::createValidationLayerManager();

        instance->create(*windowExtension.get(), *vulkanValidationLayerManager.get());

        if (instance->validationEnabled())
        {
            debugMessenger = VulkanFactory::createDebugMessenger();
            debugMessenger->create(instance->handle());
        }
        
        windowSurface->create(instance->handle(), *windowSurfaceProvider.get(), *window);
        physicalDevice->select(instance->handle(), windowSurface->handle(), config::DeviceExtensions);
        logicalDevice->create(physicalDevice->handle(), physicalDevice->queueFamilies(), config::DeviceExtensions);
        swapchain->create(*physicalDevice.get(), logicalDevice->handle(), windowSurface->handle(), window->getWidth(), window->getHeight());
        
        createRenderFinishedSemaphores();
        createSwapchainRenderPass();
        createSwapchainFramebuffers();

        graphicsCommandPool->create(logicalDevice->handle(), physicalDevice->queueFamilies().graphics.value());

        createFrameData();

        return true;
    }

    void vulkan::vulkanRenderer::resize(std::uint32_t width, std::uint32_t height)
    {
        engine::LogInfo("resize");
    }

    void vulkan::vulkanRenderer::beginFrame(const core::RenderView& renderView)
    {
        VkClearValue clearValue{};
        clearValue.color.float32[0] = renderView.clearColor.x;
        clearValue.color.float32[1] = renderView.clearColor.y;
        clearValue.color.float32[2] = renderView.clearColor.z;
        clearValue.color.float32[3] = renderView.clearColor.w;

        FrameData& frame = frames[currentFrame];

        frame.inFlight.wait();
        
        auto result = swapchain->acquireNextImage(frame.imageAvailableSemaphore, currentImageIndex);

        if(result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            engine::LogWarn("vulkan::vulkanRenderer::beginFrame, No image is acquired, recreate swapchain skipping the frame !");
            return;
        }

        frame.inFlight.reset();

        
        frame.commandBuffer.begin();
    
        frame.commandBuffer.beginRenderPass(
            renderResources->renderPass(), 
            renderResources->framebuffers().at(currentImageIndex), 
            {{0, 0}, swapchain->extent()},
            std::span(&clearValue, 1));
        
        frame.commandBuffer.setViewport(
            0.0f,
            0.0f,
            static_cast<float>(swapchain->extent().width),
            static_cast<float>(swapchain->extent().height),
            0.0f,
            1.0f);

        frame.commandBuffer.setScissor(
            {0, 0},
            swapchain->extent());
        
        isFrameRunning = true;
    }

    void vulkan::vulkanRenderer::draw(const std::vector<core::Renderable>& items)
    {
        if(!isFrameRunning)
        {
            engine::LogTrace("vulkan::vulkanRenderer::draw, Frame is not running, skipping draw command");
            return;
        }

        VkPipeline previousGraphicsPipeline = VK_NULL_HANDLE;
        FrameData& frame = frames[currentFrame];

        for (size_t i = 0; i < items.size(); i++)
        {
            auto graphicsPipeline = renderResources->graphicsPipeline(items[i].graphicsPipelineHandle);
            if (!graphicsPipeline) continue;

            if(previousGraphicsPipeline != graphicsPipeline->pipeline.handle())
            {
                previousGraphicsPipeline = graphicsPipeline->pipeline.handle();
                frame.commandBuffer.bindGraphicsPipeline(graphicsPipeline->pipeline);
            }
            
            frame.commandBuffer.draw(3, 1, 0, 0);
        }
    }

    core::GraphicsPipelineHandle vulkan::vulkanRenderer::createGraphicsPipeline(
        const core::GraphicsPipelineDescription& description)
    {
        if (description.vertexShaderCode.empty())
        {
            engine::LogError("vulkanRenderer::createGraphicsPipeline, failed to resolve vertex shader handle");
            return {};
        }

        if (description.fragmentShaderCode.empty())
        {
            engine::LogError("vulkanRenderer::createGraphicsPipeline, failed to resolve fragment shader handle");
            return {};
        }

        rendererResources::VulkanShaderModule vertexModule =
            builder::VulkanShaderModuleBuilder().setCode(description.vertexShaderCode).build(logicalDevice->handle());

        rendererResources::VulkanShaderModule fragmentModule =
            builder::VulkanShaderModuleBuilder().setCode(description.fragmentShaderCode).build(logicalDevice->handle());

        rendererResources::VulkanPipelineLayout layout =
            builder::VulkanPipelineLayoutBuilder().build(logicalDevice->handle());

        builder::VulkanGraphicsPipelineBuilder pipelineBuilder;
        pipelineBuilder
            .addShaderStage(vertexModule.handle(), VK_SHADER_STAGE_VERTEX_BIT)
            .addShaderStage(fragmentModule.handle(), VK_SHADER_STAGE_FRAGMENT_BIT)
            .setLayout(layout.handle())
            .setRenderPass(renderResources->renderPass().handle());

        rendererResources::VulkanGraphicsPipeline pipeline = pipelineBuilder.build(logicalDevice->handle());

        return renderResources->addGraphicsPipeline(std::move(layout), std::move(pipeline));
    }

    void vulkan::vulkanRenderer::endFrame()
    {
        if(!isFrameRunning)
        {
            engine::LogTrace("vulkan::vulkanRenderer::endFrame, Frame is not running, skipping endFrame");
            return;
        }

        FrameData& frame = frames[currentFrame];

        frame.commandBuffer.endRenderPass();
        frame.commandBuffer.end();
        logicalDevice->graphicsQueue().submit(frame.commandBuffer, frame.imageAvailableSemaphore, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, renderFinishedSemaphoreList[currentImageIndex], frame.inFlight);
        auto presentationResult = logicalDevice->presentQueue().present(*swapchain, currentImageIndex, renderFinishedSemaphoreList[currentImageIndex]);
        if(presentationResult != VK_SUCCESS)
        {
            engine::LogWarn("vulkan::vulkanRenderer::endFrame, Swapchain must be recreated");
        }
        
        currentFrame = (currentFrame + 1) % config::MaxFramesInFlight;        
        isFrameRunning = false;
    }

    void vulkan::vulkanRenderer::waitIdle()
    {
        if(!logicalDevice.get() || logicalDevice->handle() == VK_NULL_HANDLE)
        {
            engine::LogTrace("vulkan::vulkanRenderer::waitIdle, wait idle is not running as logical device is not created or it's vulkan Handle is NULL");
            return;
        }
        vkCheck(vkDeviceWaitIdle(logicalDevice->handle()), "vulkan::vulkanRenderer::waitIdle, Wait ideal failed !");
    }

    void vulkan::vulkanRenderer::createSwapchainRenderPass()
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
    }

    void vulkan::vulkanRenderer::createSwapchainFramebuffers()
    {
        // Create 1 Framebuffer per image view of swapchain used in render pass
        const VkRenderPass renderPass = renderResources->renderPass().handle();
        const VkExtent2D extent = swapchain->extent();

        auto& framebuffers = renderResources->framebuffers();
        framebuffers.clear();
        framebuffers.reserve(swapchain->imageViews().size());

        for (const auto& imageView : swapchain->imageViews())
        {
            builder::VulkanFramebufferBuilder fbBuilder;

            fbBuilder
                .setRenderPass(renderPass)
                .addAttachment(imageView.handle())
                .setExtent(extent.width, extent.height)
                .setLayers(1);

            framebuffers.push_back(fbBuilder.build(logicalDevice->handle()));
        }
    }

    void vulkan::vulkanRenderer::createRenderFinishedSemaphores()
    {
        renderFinishedSemaphoreList.clear();
        renderFinishedSemaphoreList.reserve(swapchain->imageViews().size());     
        
        for(size_t i = 0; i < swapchain->imageViews().size() ; i++)
        {
            renderFinishedSemaphoreList.emplace_back();
            renderFinishedSemaphoreList.back().create(logicalDevice->handle());
        }
    }

    void vulkan::vulkanRenderer::createFrameData()
    {
        frames.clear();
        frames.reserve(config::MaxFramesInFlight);

        for (std::uint32_t i = 0; i < config::MaxFramesInFlight; ++i)
        {
            FrameData& frame = frames.emplace_back(
                graphicsCommandPool->allocate(VK_COMMAND_BUFFER_LEVEL_PRIMARY));

            frame.inFlight.create(logicalDevice->handle(), true);
            frame.imageAvailableSemaphore.create(logicalDevice->handle());
        }
    }
}