#include <projectV/core/transform.hpp>
#include <projectV/core/shader.hpp>
#include <projectV/core/shaderHandle.hpp>
#include <projectV/core/mesh.hpp>
#include <projectV/core/meshHandle.hpp>
#include <projectV/core/texture.hpp>
#include <projectV/core/textureHandle.hpp>

#include <projectV/engine/logging/log.hpp>

#include <vulkan/builder/vulkanBufferBuilder.hpp>
#include <vulkan/builder/VulkanRenderPassBuilder.hpp>
#include <vulkan/builder/vulkanFramebufferBuilder.hpp>
#include <vulkan/builder/vulkanShaderModuleBuilder.hpp>
#include <vulkan/builder/vulkanPipelineLayoutBuilder.hpp>
#include <vulkan/builder/vulkanGraphicsPipelineBuilder.hpp>
#include <vulkan/builder/vulkanImageBuilder.hpp>
#include <vulkan/builder/vulkanDescriptorPoolBuilder.hpp>
#include <vulkan/builder/vulkanDescriptorSetLayoutBuilder.hpp>
#include <vulkan/builder/vulkanDescriptorSetLayoutBindingBuilder.hpp>
#include <vulkan/builder/vulkanSamplerBuilder.hpp>

#include <vulkan/config/vulkanConfig.hpp>
#include <vulkan/vulkanFactory.hpp>
#include <vulkan/vulkanRenderer.hpp>
#include <vulkan/rendererResources.hpp>
#include <vulkan/frameData.hpp>
#include <vulkan/cameraUniforms.hpp>


#include <vulkan/command/vulkanCommandPool.hpp>

#include <vulkan/device/vulkanDevice.hpp>
#include <vulkan/device/vulkanInstance.hpp>
#include <vulkan/device/vulkanPhysicalDevice.hpp>
#include <vulkan/device/vulkanQueue.hpp>

#include <vulkan/util/windowExtension.hpp>
#include <vulkan/util/vulkanCheck.hpp>
#include <vulkan/util/vulkanValidationLayerManager.hpp>
#include <vulkan/util/vulkanDebugMessenger.hpp>
#include <vulkan/util/vulkanFormat.hpp>
#include <vulkan/util/vulkanTextureMapping.hpp>

#include <vulkan/window/vulkanSurface.hpp>
#include <vulkan/window/vulkanSwapchain.hpp>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <utility>
#include <vector>

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
        createDepthBuffer();

        createFrameSetLayout();
        createDescriptorPool();

        createFrameUniformSets();

        createSwapchainRenderPass();
        createSwapchainFramebuffers();

        graphicsCommandPool->create(logicalDevice->handle(), physicalDevice->queueFamilies().graphics.value());

        createFrameData();
 
        createUploadCommandBuffer();

        createCubeSampler();

        return true;
    }

    void vulkan::vulkanRenderer::resize(std::uint32_t width, std::uint32_t height)
    {
        engine::LogInfo("resize");
    }

    void vulkan::vulkanRenderer::beginFrame(const core::RenderView& renderView)
    {
        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color.float32[0] = renderView.clearColor.x;
        clearValues[0].color.float32[1] = renderView.clearColor.y;
        clearValues[0].color.float32[2] = renderView.clearColor.z;
        clearValues[0].color.float32[3] = renderView.clearColor.w;

        clearValues[1].depthStencil = {1.0f, 0};

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
            std::span(clearValues));
        
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
        
        writeCameraUniforms(renderView.viewProj, renderView.cameraPosition);

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

        assert(currentFrame < renderResources->frameUniformSets().size());
        const VkDescriptorSet& frameDescriptorSet = renderResources->frameUniformSets()[currentFrame].descriptorSet;

        for (size_t i = 0; i < items.size(); i++)
        {
            auto graphicsPipeline = renderResources->graphicsPipeline(items[i].graphicsPipelineHandle);
            if (!graphicsPipeline) continue;

            const projectv::vulkan::MeshEntry *meshEntry = renderResources->gpuMesh(items[i].gpuMesh);
            if (!meshEntry) continue;

            if(previousGraphicsPipeline != graphicsPipeline->pipeline.handle())
            {
                previousGraphicsPipeline = graphicsPipeline->pipeline.handle();
                frame.commandBuffer.bindGraphicsPipeline(graphicsPipeline->pipeline);

                frame.commandBuffer.bindDescriptorSets(
                    graphicsPipeline->layout,
                    config::FrameSetIndex,
                    { &frameDescriptorSet, 1 }
                );
            }
            
            frame.commandBuffer.bindVertexBuffers(0, { &meshEntry->vertexBuffer });
            frame.commandBuffer.bindIndexBuffer(meshEntry->indexBuffer, VK_INDEX_TYPE_UINT32);

            auto vertexCount = meshEntry->vertexBuffer.getDeviceSize() / sizeof(core::Vertex);
            
            frame.commandBuffer.pushConstants(graphicsPipeline->layout, 
                VK_SHADER_STAGE_VERTEX_BIT, 
                0, sizeof(core::math::Matrix4), 
                &items[i].modelMatrix);

            if(meshEntry->indexBuffer.handle() != VK_NULL_HANDLE)
            {
                frame.commandBuffer.drawIndexed(meshEntry->indexCount, 1, 0, 0, 0);
                continue;
            }

            frame.commandBuffer.draw(vertexCount, 1, 0, 0);
        }
    }

    core::gpu::GpuMeshHandle vulkan::vulkanRenderer::uploadMesh(const struct core::Mesh& mesh)
    {
        if(physicalDevice->hasUnifiedMemory())
        {
            return unifiedMemoryUpload(mesh);
        }

        return stagedUpload(mesh);
    }

    core::gpu::GpuTextureHandle projectv::vulkan::vulkanRenderer::uploadTexture(const core::Texture &texture)
    {
        if(physicalDevice->hasUnifiedMemory())
        {
            return unifiedMemoryUpload(texture);
        }

        return stagedUpload(texture);
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

        if(!renderResources->frameSetLayout())
        {
            engine::LogError("vulkanRenderer::createGraphicsPipeline, frame descriptor set layout is invalid");
            return {};
        }

        rendererResources::VulkanShaderModule vertexModule =
            builder::VulkanShaderModuleBuilder().setCode(description.vertexShaderCode).build(logicalDevice->handle());

        rendererResources::VulkanShaderModule fragmentModule =
            builder::VulkanShaderModuleBuilder().setCode(description.fragmentShaderCode).build(logicalDevice->handle());

        rendererResources::VulkanPipelineLayout layout =
            builder::VulkanPipelineLayoutBuilder()
            .addDescriptorSetLayout(renderResources->frameSetLayout().handle())
            .addPushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(core::math::Matrix4)) // Make room for model Matrix in push constant
            .build(logicalDevice->handle());

        const VkVertexInputBindingDescription bindingDescription {0, sizeof(core::Vertex), VK_VERTEX_INPUT_RATE_VERTEX};

        const std::array<VkVertexInputAttributeDescription, 4>  attributeDescription 
        {{
            { 0, 0, VK_FORMAT_R32G32B32_SFLOAT,    offsetof(core::Vertex, position) },
            { 1, 0, VK_FORMAT_R32G32B32_SFLOAT,    offsetof(core::Vertex, normal) },
            { 2, 0, VK_FORMAT_R32G32B32A32_SFLOAT,    offsetof(core::Vertex, color) },
            { 3, 0, VK_FORMAT_R32G32_SFLOAT,    offsetof(core::Vertex, uv) },
        }};

        builder::VulkanGraphicsPipelineBuilder pipelineBuilder;
        pipelineBuilder
            .addShaderStage(vertexModule.handle(), VK_SHADER_STAGE_VERTEX_BIT)
            .addShaderStage(fragmentModule.handle(), VK_SHADER_STAGE_FRAGMENT_BIT)
            .setLayout(layout.handle())
            .setVertexInputState(std::span(&bindingDescription, 1), attributeDescription)
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
        vkCheck(vkDeviceWaitIdle(logicalDevice->handle()), "vulkan::vulkanRenderer::waitIdle, Wait idle failed !");
    }

    core::gpu::GpuMeshHandle vulkan::vulkanRenderer::unifiedMemoryUpload(const core::Mesh &mesh)
    {
        auto vertexBufferSize = sizeof(mesh.vertices[0]) * mesh.vertices.size();
        auto indexBufferSize  = sizeof(mesh.indices[0]) * mesh.indices.size();

        rendererResources::VulkanBuffer vertexBuffer = createBuffer(
            vertexBufferSize, 
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        rendererResources::VulkanBuffer indexBuffer = createBuffer(
            indexBufferSize,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        vertexBuffer.write(mesh.vertices.data(), vertexBufferSize);
        indexBuffer.write(mesh.indices.data(), indexBufferSize);

        return renderResources->addGpuMesh(
            {
                std::move(vertexBuffer),
                std::move(indexBuffer),
                static_cast<std::uint32_t>(mesh.indices.size())
            }
        );
    }

    core::gpu::GpuMeshHandle vulkan::vulkanRenderer::stagedUpload(const core::Mesh &mesh)
    {
        auto vertexBufferSize = sizeof(mesh.vertices[0]) * mesh.vertices.size();
        auto indexBufferSize  = sizeof(mesh.indices[0]) * mesh.indices.size();

        rendererResources::VulkanBuffer stagingVertexBuffer = createBuffer(
            vertexBufferSize, 
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        rendererResources::VulkanBuffer stagingIndexBuffer = createBuffer(
            indexBufferSize, 
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        stagingVertexBuffer.write(mesh.vertices.data(), vertexBufferSize);
        stagingIndexBuffer.write(mesh.indices.data(), indexBufferSize);
     
        rendererResources::VulkanBuffer gpuVertexBuffer = createBuffer(
            vertexBufferSize, 
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        rendererResources::VulkanBuffer gpuIndexBuffer = createBuffer(
            indexBufferSize, 
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        const std::array jobs{
            BufferCopyJob{stagingVertexBuffer.handle(), gpuVertexBuffer.handle(), vertexBufferSize},
            BufferCopyJob{stagingIndexBuffer.handle(),  gpuIndexBuffer.handle(),  indexBufferSize}
        };

        submitCopy(jobs);
        
        return renderResources->addGpuMesh(
        {
            std::move(gpuVertexBuffer),
            std::move(gpuIndexBuffer),
            static_cast<std::uint32_t>(mesh.indices.size())
        });
    }

    core::gpu::GpuTextureHandle projectv::vulkan::vulkanRenderer::unifiedMemoryUpload(const core::Texture &texture)
    {
        engine::LogWarn("vulkan::vulkanRenderer::unifiedMemoryUpload, Not implemented yet");
        return core::gpu::GpuTextureHandle{};
    }

    core::gpu::GpuTextureHandle projectv::vulkan::vulkanRenderer::stagedUpload(const core::Texture &texture)
    {
        const auto& tDesc = texture.desc;
        const auto& tData = texture.data;

        VkDeviceSize imageSize = sizeof(std::uint8_t) * tData.size();

        rendererResources::VulkanBuffer stagImageBuffer = createBuffer(
            imageSize, 
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        
        stagImageBuffer.write(tData.data(), imageSize);

        vulkan::builder::VulkanImageBuilder builder;
        auto imageFormat = vulkan::util::toVulkanFormat(tDesc.format);

        builder.setExtent(tDesc.width, tDesc.height )
                .setFormat(imageFormat)
                .setTiling(VK_IMAGE_TILING_OPTIMAL)
                .setUsageFlags(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
                .setInitialLayout(VK_IMAGE_LAYOUT_UNDEFINED)
                .setImageType(vulkan::util::toVulkanImageType(tDesc.type))
                .setMipLevels(tDesc.mipLevels)
                .setArrayLayers(tDesc.layers)
                .setSharingMode(VK_SHARING_MODE_EXCLUSIVE);

        // A cube view can only be created on an image made cube compatible.
        if (tDesc.type == core::TextureType::CubeMap)
        {
            builder.setCreateFlags(VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT);
        }

        auto vulkanTexture = builder.build(
            logicalDevice->handle(), 
            physicalDevice->handle());

        VkImageSubresourceRange sbr{};
        sbr.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        sbr.baseMipLevel   = 0;
        sbr.levelCount     = tDesc.mipLevels;
        sbr.baseArrayLayer = 0;
        sbr.layerCount     = tDesc.layers;

        rendererResources::VulkanImageView vulkanTextureImgView;
        vulkanTextureImgView.create(
            logicalDevice->handle(),
            vulkanTexture.handle(),
            vulkan::util::toVulkanImageViewType(tDesc.type),
            imageFormat,
            { 
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY 
            },
            sbr
        );
        
        // One region per (mip level, layer). Each image sits at
        // tDesc.offset(level, layer) in the staging buffer, and its rows are
        // tightly packed, so bufferRowLength/bufferImageHeight stay 0.
        std::vector<VkBufferImageCopy> copyRegions;
        copyRegions.reserve(static_cast<std::size_t>(tDesc.mipLevels) * tDesc.layers);

        for (std::uint32_t level = 0; level < tDesc.mipLevels; ++level)
        {
            const VkExtent3D mipExtent{
                std::max(1u, tDesc.width  >> level),
                std::max(1u, tDesc.height >> level),
                1u
            };

            for (std::uint32_t layer = 0; layer < tDesc.layers; ++layer)
            {
                VkBufferImageCopy region{};
                region.bufferOffset                    = static_cast<VkDeviceSize>(tDesc.offset(level, layer));
                region.bufferRowLength                 = 0;
                region.bufferImageHeight               = 0;
                region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
                region.imageSubresource.mipLevel       = level;
                region.imageSubresource.baseArrayLayer = layer;
                region.imageSubresource.layerCount     = 1;
                region.imageOffset                     = {0, 0, 0};
                region.imageExtent                     = mipExtent;

                copyRegions.push_back(region);
            }
        }

        submitCopy(vulkanTexture, copyRegions, stagImageBuffer);

        return renderResources->addGpuTexture(std::move(vulkanTexture), std::move(vulkanTextureImgView));
    }

    void vulkan::vulkanRenderer::createDescriptorPool()
    {
        builder::VulkanDescriptorPoolBuilder builder;
        builder.setMaxSets(config::MaxFramesInFlight + config::MaxMainCubeMap)
        .addPoolSize({VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, static_cast<uint32_t>(config::MaxFramesInFlight)})
        // Every frame set carries the cube map binding, so each one needs its own samplers.
        .addPoolSize({VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, static_cast<uint32_t>(config::MaxFramesInFlight * config::MaxMainCubeMap)});

        renderResources->descriptorPool() = builder.build(logicalDevice->handle());
    }

    VkDescriptorSetLayoutBinding vulkan::vulkanRenderer::createCubeMapSamplerBinding()
    {
       builder::VulkanDescriptorSetLayoutBindingBuilder bindingBuilder;
        bindingBuilder.setBinding(config::CubeMapBindingIndex)
        .setDescriptorCount(config::MaxMainCubeMap)
        .setDescriptorType(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
        .setStageFlags(VK_SHADER_STAGE_FRAGMENT_BIT);

        VkDescriptorSetLayoutBinding binding = bindingBuilder.build();

        return bindingBuilder.build();
    }

    void vulkan::vulkanRenderer::createFrameSetLayout()
    {
        builder::VulkanDescriptorSetLayoutBindingBuilder bindingBuilder;
        bindingBuilder.setBinding(config::CameraBindingIndex)
        .setDescriptorCount(1)
        .setDescriptorType(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
        .setStageFlags(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);

        VkDescriptorSetLayoutBinding binding = bindingBuilder.build();

        builder::VulkanDescriptorSetLayoutBuilder builder;
        builder.addLayoutBinding(binding);
        builder.addLayoutBinding(createCubeMapSamplerBinding());

        renderResources->frameSetLayout() = builder.build(logicalDevice->handle());
    }

    void vulkan::vulkanRenderer::createFrameUniformSets()
    {
        if(!renderResources->frameSetLayout())
        {
            engine::LogError("vulkanRenderer::createFrameUniformSets, Required descriptor set layout is NULL unable to create desciprot set");
            throw std::runtime_error("vulkanRenderer::createFrameUniformSets, Required descriptor set layout is NULL unable to create desciprot set");
        }

        // One descriptor set per frame in flight. A set bound by a pending command buffer
        // must not be updated, so frames can't share a set.
        std::array<VkDescriptorSetLayout, config::MaxFramesInFlight> layouts;
        layouts.fill(renderResources->frameSetLayout().handle());

        auto descriptorSets = renderResources->descriptorPool().allocate(layouts);

        for (size_t i = 0; i < config::MaxFramesInFlight; i++)
        {
            rendererResources::VulkanBuffer cameraBuffer = createBuffer(
                sizeof(CameraUniforms),
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
            );
            cameraBuffer.map();
                               
            FrameUniformSet frameSet;
            frameSet.descriptorSet = descriptorSets[i];
            frameSet.uniformBuffer = std::move(cameraBuffer);

            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = frameSet.uniformBuffer.handle();
            bufferInfo.offset = 0;
            bufferInfo.range  = sizeof(CameraUniforms);

            std::array<VkWriteDescriptorSet, 1> descriptorWrites{};
            writeUniformDescSet(descriptorWrites[config::CameraBindingIndex], frameSet.descriptorSet, bufferInfo);

            vkUpdateDescriptorSets(logicalDevice->handle(), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);

            renderResources->frameUniformSets().push_back(std::move(frameSet));
        }
    }

    void vulkan::vulkanRenderer::writeCameraUniforms(const core::math::Matrix4& vpMatrix, const core::math::Vector3& cameraPosition)
    {
        const CameraUniforms cameraUniforms
        {
            .viewProj = vpMatrix,
            .invViewProj = glm::inverse(vpMatrix),
            .cameraPos = core::math::Vector4(cameraPosition, 1.0f)
        };

        // Safe to overwrite: beginFrame waited on this frame's fence, so the GPU is done reading it.
        renderResources->frameUniformSets()[currentFrame].uniformBuffer.writeMapped(&cameraUniforms, sizeof(CameraUniforms));
    }

    vulkan::rendererResources::VulkanBuffer vulkan::vulkanRenderer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memory)
    {
        return builder::VulkanBufferBuilder()
        .setDeviceSize(size)
        .setSharingMode(VK_SHARING_MODE_EXCLUSIVE)
        .setUsageFlags(usage)
        .setMemoryProperties(memory)
        .build(logicalDevice->handle(), physicalDevice->handle());
    }

    void vulkan::vulkanRenderer::createSwapchainRenderPass()
    {
        vulkan::builder::VulkanRenderPassBuilder builder;

        uint32_t colorIdx = builder.addColorAttachment(swapchain->format().format);
        uint32_t depthIdx = builder.addDepthAttachment(renderResources->getDepthImage().format()); 

        uint32_t subpass = builder.beginSubpass();
        builder.addColorRef(subpass, colorIdx);
        builder.setDepthRef(subpass, depthIdx, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

        builder.addDependency(
            VK_SUBPASS_EXTERNAL, 
            subpass,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, 
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, 
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
        );

        renderResources->renderPass() = builder.build(logicalDevice->handle());
    }

    void vulkan::vulkanRenderer::createDepthBuffer()
    {
        vulkan::builder::VulkanImageBuilder builder;
        auto imageFormat = physicalDevice->findSupportedFormat(
                {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                VK_IMAGE_TILING_OPTIMAL,
                VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

        builder.setExtent( swapchain->extent().width, swapchain->extent().height )
                .setFormat( imageFormat )
                .setTiling(VK_IMAGE_TILING_OPTIMAL)
                .setUsageFlags(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);

        auto depthImage = builder.build(
            logicalDevice->handle(), 
            physicalDevice->handle());
        auto depthImageView = vulkan::rendererResources::VulkanImageView();
        
        depthImageView.create(
            logicalDevice->handle(), 
            depthImage.handle(), 
            VK_IMAGE_VIEW_TYPE_2D, 
            imageFormat,
            { 
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY 
            }, 
            { 
                vulkan::util::depthStencilAspectFlags(imageFormat),
                0,
                1,
                0,
                1
            }
        );
        
        renderResources->setDepthEntry(std::move(depthImage), std::move(depthImageView));
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
                .addAttachment(renderResources->getDepthImageView().handle())
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

    void vulkan::vulkanRenderer::createUploadCommandBuffer()
    {
        uploadCommandBuffer = std::make_unique<command::VulkanCommandBuffer>(
            graphicsCommandPool->allocate(VK_COMMAND_BUFFER_LEVEL_PRIMARY));
    }

    void vulkan::vulkanRenderer::submitCopy(std::span<const BufferCopyJob> bufferCopyJobs)
    {
        if(!uploadCommandBuffer)
        {
            engine::LogError("vulkan::vulkanRenderer::submitCopy, uploadCommandBuffer is invalid");
            throw std::runtime_error("vulkan::vulkanRenderer::submitCopy, uploadCommandBuffer is invalid");
        }

        if(bufferCopyJobs.empty())
        {
            engine::LogError("vulkan::vulkanRenderer::submitCopy, bufferCopyJobs count is zero");
            throw std::runtime_error("vulkan::vulkanRenderer::submitCopy, bufferCopyJobs count is zero");
        }

        for (const auto &bufferCopyJob : bufferCopyJobs) 
        {
            if(bufferCopyJob.size == 0)
            {
                engine::LogError("vulkan::vulkanRenderer::submitCopy, bufferCopyJob buffer size is zero");
                throw std::runtime_error("vulkan::vulkanRenderer::submitCopy, bufferCopyJob buffer size is zero");
            }
        }
        auto& uploadCmd = *uploadCommandBuffer;

        projectv::vulkan::sync::VulkanFence fence{};
        fence.create(logicalDevice->handle(), false);

        uploadCmd.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        for (const auto &bufferCopyJob : bufferCopyJobs) 
        {
            uploadCmd.copyBuffer(bufferCopyJob.srcBuffer, bufferCopyJob.dstBuffer, bufferCopyJob.size);
        }

        uploadCmd.end();

        logicalDevice->graphicsQueue().submit(uploadCmd, fence);
        fence.wait();
    }
    
    void vulkan::vulkanRenderer::submitCopy(rendererResources::VulkanImage &inTexture, std::span<const VkBufferImageCopy> region, rendererResources::VulkanBuffer &inImgBuffer)
    {
        if(!uploadCommandBuffer)
        {
            engine::LogError("vulkan::vulkanRenderer::submitCopy, uploadCommandBuffer is invalid");
            throw std::runtime_error("vulkan::vulkanRenderer::submitCopy, uploadCommandBuffer is invalid");
        }

        if(region.empty())
        {
            engine::LogError("vulkan::vulkanRenderer::submitCopy, region count is zero");
            throw std::runtime_error("vulkan::vulkanRenderer::submitCopy, region count is zero");
        }

        auto& uploadCmd = *uploadCommandBuffer;

        projectv::vulkan::sync::VulkanFence fence{};
        fence.create(logicalDevice->handle(), false);

        // Every mip level and layer of the image changes layout together.
        VkImageMemoryBarrier barrier{};
        barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.image                           = inTexture.handle();
        barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel   = 0;
        barrier.subresourceRange.levelCount     = VK_REMAINING_MIP_LEVELS;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount     = VK_REMAINING_ARRAY_LAYERS;

        uploadCmd.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        // UNDEFINED -> TRANSFER_DST_OPTIMAL: nothing to wait for, the old
        // contents are discarded; the copy's writes wait on the transition.
        barrier.oldLayout     = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        uploadCmd.pipelineBarrier(
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            std::span{&barrier, 1});

        uploadCmd.copyBufferToImage(inImgBuffer.handle(), inTexture.handle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, region);

        // TRANSFER_DST_OPTIMAL -> SHADER_READ_ONLY_OPTIMAL: make the copied
        // data visible to fragment shader reads.
        barrier.oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        uploadCmd.pipelineBarrier(
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            std::span{&barrier, 1});

        uploadCmd.end();

        logicalDevice->graphicsQueue().submit(uploadCmd, fence);
        fence.wait();        
    }

    void vulkan::vulkanRenderer::writeUniformDescSet(VkWriteDescriptorSet &descWrite, VkDescriptorSet descSet, VkDescriptorBufferInfo &bufferInfo)
    {
        descWrite.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descWrite.dstSet          = descSet;
        descWrite.dstBinding      = config::CameraBindingIndex;
        descWrite.dstArrayElement = 0;
        descWrite.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descWrite.descriptorCount = 1;
        descWrite.pBufferInfo     = &bufferInfo;
    }

    void vulkan::vulkanRenderer::writeSamplerDescSet(VkWriteDescriptorSet& descWrite, VkDescriptorSet descSet, VkDescriptorImageInfo& imgInfo)
    {
        descWrite.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descWrite.dstSet          = descSet;
        descWrite.dstBinding      = config::CubeMapBindingIndex;
        descWrite.dstArrayElement = 0;
        descWrite.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descWrite.descriptorCount = config::MaxMainCubeMap;
        descWrite.pImageInfo     = &imgInfo;
    }
    
    void vulkan::vulkanRenderer::createCubeSampler()
    {
        auto builder = builder::VulkanSamplerBuilder();
        builder.setAddressMode(VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE)
        .setMaxAnisotropy(4)
        .setLodRange(0.0f, 16);

        renderResources->cubeMapSampler() = builder.build(logicalDevice->handle());
    }

    void vulkan::vulkanRenderer::updateCubeMapDescriptorSets(core::gpu::GpuTextureHandle cubeMapHandle)
    {
        const rendererResources::VulkanImageView* cubeMapView = renderResources->gpuTextureImageView(cubeMapHandle);
        if (!cubeMapView || cubeMapView->handle() == VK_NULL_HANDLE)
        {
            engine::LogError("vulkanRenderer::updateCubeMapDescriptorSets, cube map texture handle is invalid");
            throw std::runtime_error("vulkanRenderer::updateCubeMapDescriptorSets, cube map texture handle is invalid");
        }

        if (!renderResources->cubeMapSampler())
        {
            engine::LogError("vulkanRenderer::updateCubeMapDescriptorSets, cube map sampler has not been created");
            throw std::runtime_error("vulkanRenderer::updateCubeMapDescriptorSets, cube map sampler has not been created");
        }

        auto& frameUniformSets = renderResources->frameUniformSets();
        if (frameUniformSets.empty())
        {
            engine::LogError("vulkanRenderer::updateCubeMapDescriptorSets, frame descriptor sets have not been created");
            throw std::runtime_error("vulkanRenderer::updateCubeMapDescriptorSets, frame descriptor sets have not been created");
        }

        // Every frame set samples the same cube map, so one image info serves all writes.
        VkDescriptorImageInfo imageInfo{};
        imageInfo.sampler     = renderResources->cubeMapSampler().handle();
        imageInfo.imageView   = cubeMapView->handle();
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        std::vector<VkWriteDescriptorSet> descriptorWrites(frameUniformSets.size());

        for (size_t i = 0; i < frameUniformSets.size(); i++)
        {
            writeSamplerDescSet(descriptorWrites[i], frameUniformSets[i].descriptorSet, imageInfo);
        }

        vkUpdateDescriptorSets(
            logicalDevice->handle(),
            static_cast<std::uint32_t>(descriptorWrites.size()),
            descriptorWrites.data(),
            0,
            nullptr);
    }
}