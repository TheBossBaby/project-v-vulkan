#include <projectV/core/transform.hpp>
#include <projectV/core/shaderHandle.hpp>
#include <projectV/core/meshHandle.hpp>

#include <projectV/engine/graph/scene/ecs/world.hpp>
#include <projectV/engine/logging/log.hpp>

#include <vulkan/vulkan.h>

#include <vulkan/vulkanFactory.hpp>
#include <vulkan/vulkanRenderer.hpp>

#include <vulkan/command/vulkanCommandBuffer.hpp>
#include <vulkan/command/vulkanCommandPool.hpp>

#include <vulkan/device/vulkanDevice.hpp>
#include <vulkan/device/vulkanInstance.hpp>
#include <vulkan/device/vulkanPhysicalDevice.hpp>
#include <vulkan/device/vulkanQueue.hpp>

#include <vulkan/util/windowExtension.hpp>

#include <vulkan/window/vulkanSurface.hpp>

namespace projectv
{
    vulkan::vulkanRenderer::vulkanRenderer()
    {
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

        instance->create(*windowExtension.get());
        windowSurface->create(instance->handle(), *windowSurfaceProvider.get(), *window);
        physicalDevice->select(instance->handle());
        logicalDevice->create(physicalDevice->handle(), physicalDevice->queueFamilies());

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
}