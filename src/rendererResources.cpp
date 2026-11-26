#include <rendererResources.hpp>

#include <cstdint>
#include <utility>

namespace projectv
{
    namespace vulkan
    {
        VulkanRenderPass &RendererResources::renderPass()
        {
            return m_renderPass;
        }

        const VulkanRenderPass &RendererResources::renderPass() const
        {
            return m_renderPass;
        }

        std::vector<rendererResources::VulkanFramebuffer> &RendererResources::framebuffers()
        {
            return m_framebuffers;
        }

        const std::vector<rendererResources::VulkanFramebuffer> &RendererResources::framebuffers() const
        {
            return m_framebuffers;
        }

        core::gpu::GpuMeshHandle RendererResources::addGpuMesh(MeshEntry buffer)
        {
            core::gpu::GpuMeshHandle handle;
            handle.index = static_cast<std::uint32_t>(m_meshList.size());

            m_meshList.push_back(std::move(buffer));

            return handle;
        }

        const MeshEntry *RendererResources::gpuMesh(core::gpu::GpuMeshHandle gpuMeshHandle) const
        {
            if (!gpuMeshHandle.isValid() || gpuMeshHandle.index >= m_meshList.size())
                return nullptr;
            
            return &m_meshList[gpuMeshHandle.index];
        }

        core::GraphicsPipelineHandle RendererResources::addGraphicsPipeline(
            rendererResources::VulkanPipelineLayout layout,
            rendererResources::VulkanGraphicsPipeline pipeline)
        {
            core::GraphicsPipelineHandle handle;
            handle.index = static_cast<std::uint32_t>(m_graphicsPipelines.size());

            m_graphicsPipelines.emplace_back( std::move(layout), std::move(pipeline) );

            return handle;
        }

        const RendererResources::GraphicsPipelineEntry* RendererResources::graphicsPipeline(
            core::GraphicsPipelineHandle handle) const
        {
            if (!handle.isValid() || handle.index >= m_graphicsPipelines.size())
                return nullptr;

            return &m_graphicsPipelines[handle.index];
        }

        void RendererResources::setDepthEntry(rendererResources::VulkanImage inDepthImage, rendererResources::VulkanImageView inDepthImageView)
        {
            m_depthImage     = std::move(inDepthImage);
            m_depthImageView = std::move(inDepthImageView);
        }
        
        const rendererResources::VulkanImage &RendererResources::getDepthImage() const
        {
            return m_depthImage;
        }

        const rendererResources::VulkanImageView &RendererResources::getDepthImageView() const
        {
            return m_depthImageView;
        }

        std::vector<FrameUniformSet> &RendererResources::frameUniformSets()
        {
            return m_frameUniformSets;
        }

        const std::vector<FrameUniformSet> &RendererResources::frameUniformSets() const
        {
            return m_frameUniformSets;
        }

        descriptor::VulkanDescriptorPool& RendererResources::descriptorPool()
        {
            return m_descriptorPool;
        }

        const descriptor::VulkanDescriptorPool &RendererResources::descriptorPool() const
        {
            return m_descriptorPool;
        }

        descriptor::VulkanDescriptorSetLayout& RendererResources::frameSetLayout()
        {
            return m_frameSetLayout;
        }

        const descriptor::VulkanDescriptorSetLayout &RendererResources::frameSetLayout() const
        {
            return m_frameSetLayout;
        }

        descriptor::VulkanDescriptorSetLayout& RendererResources::cubeMapSetLayout()
        {
            return m_cubeMapSetLayout;
        }

        const descriptor::VulkanDescriptorSetLayout& RendererResources::cubeMapSetLayout() const
        {
            return m_cubeMapSetLayout;
        }

        rendererResources::VulkanSampler& RendererResources::cubeMapSampler()
        {
            return m_cubeMapSampler;
        }

        const rendererResources::VulkanSampler& RendererResources::cubeMapSampler() const
        {
            return m_cubeMapSampler;
        }

        core::gpu::GpuTextureHandle RendererResources::addGpuTexture(rendererResources::VulkanImage inTexture, rendererResources::VulkanImageView inTextureImgView)
        {
            core::gpu::GpuTextureHandle handle;
            handle.index = static_cast<std::uint32_t>(m_gpuTextures.size());

            m_gpuTextures.emplace_back( std::move(inTexture), std::move(inTextureImgView) );

            return handle;
        }
            
        const rendererResources::VulkanImage* RendererResources::gpuTexture(core::gpu::GpuTextureHandle gpuTextureHandle) const
        {
            if (!gpuTextureHandle.isValid() || gpuTextureHandle.index >= m_gpuTextures.size())
                return nullptr;
            
            return &m_gpuTextures[gpuTextureHandle.index].image;
        }

        const rendererResources::VulkanImageView* RendererResources::gpuTextureImageView(core::gpu::GpuTextureHandle gpuTextureHandle) const
        {
            if (!gpuTextureHandle.isValid() || gpuTextureHandle.index >= m_gpuTextures.size())
                return nullptr;
            
            return &m_gpuTextures[gpuTextureHandle.index].imageView;
        }
    }
}
