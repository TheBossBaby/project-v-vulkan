#pragma once

#include <projectV/core/graphicsPipelineHandle.hpp>
#include <projectV/core/gpu/gpuMeshHandle.hpp>

#include <vulkan/vulkanRenderPass.hpp>
#include <vulkan/rendererResources/meshEntry.hpp>
#include <vulkan/rendererResources/vulkanFramebuffer.hpp>
#include <vulkan/rendererResources/vulkanPipelineLayout.hpp>
#include <vulkan/rendererResources/vulkanGraphicsPipeline.hpp>
#include <vulkan/rendererResources/vulkanImage.hpp>
#include <vulkan/window/VulkanImageView.hpp>

#include <vector>
namespace projectv
{
    namespace vulkan
    {
        class RendererResources
        {
        public:
            /**
             * @brief A single registered graphics pipeline and the pipeline
             *        layout it was built against.
             *
             * The layout is kept alongside the pipeline (rather than in its
             * own separate registry) because, in this codebase, the two are
             * always created and looked up together as one unit.
             */
            struct GraphicsPipelineEntry
            {
                rendererResources::VulkanPipelineLayout layout;
                rendererResources::VulkanGraphicsPipeline pipeline;
            };

            VulkanRenderPass& renderPass();

            const VulkanRenderPass& renderPass() const;

            std::vector<rendererResources::VulkanFramebuffer>& framebuffers();

            const std::vector<rendererResources::VulkanFramebuffer>& framebuffers() const;

            /**
             * @brief Takes ownership of a mesh's GPU buffers and registers it.
             * @param buffer Mesh entry to store; moved into the cache.
             * @return Handle identifying the mesh for later lookup via gpuMesh().
             */
            core::gpu::GpuMeshHandle addGpuMesh(MeshEntry buffer);

            /**
             * @brief Looks up a previously registered mesh.
             * @param gpuMeshHandle Handle returned by addGpuMesh().
             * @return Pointer to the mesh entry, or nullptr if the handle is
             *         invalid or out of range. Invalidated by later addGpuMesh() calls.
             */
            const MeshEntry* gpuMesh(core::gpu::GpuMeshHandle gpuMeshHandle) const;             

            /**
             * @brief Registers a newly created graphics pipeline.
             *
             * @param layout   Pipeline layout the pipeline was built against.
             * @param pipeline The created graphics pipeline.
             * @return A handle identifying the registered entry.
             */
            core::GraphicsPipelineHandle addGraphicsPipeline(
                rendererResources::VulkanPipelineLayout layout,
                rendererResources::VulkanGraphicsPipeline pipeline);

            /**
             * @brief Looks up a previously registered graphics pipeline entry by handle.
             * @param handle Handle returned by addGraphicsPipeline().
             * @return Pointer to the entry if the handle is valid and known; otherwise nullptr.
             */
            const GraphicsPipelineEntry* graphicsPipeline(core::GraphicsPipelineHandle handle) const;

            /**
             * @brief Takes ownership of the depth image and its view.
             *
             * Any previously stored image and view are destroyed, so the GPU
             * must have finished using them.
             *
             * @param inDepthImage     Depth image to store; moved into the cache.
             * @param inDepthImageView View of @p inDepthImage; moved into the cache.
             */
            void setDepthEntry( rendererResources::VulkanImage inDepthImage, window::VulkanImageView inDepthImageView);

            /**
             * @brief Returns the stored depth image.
             * @return The depth image, or an empty wrapper if setDepthEntry()
             *         has not been called.
             */
            const rendererResources::VulkanImage& getDepthImage() const;

            /**
             * @brief Returns the view of the stored depth image.
             * @return The depth image view, or an empty wrapper if
             *         setDepthEntry() has not been called.
             */
            const window::VulkanImageView& getDepthImageView() const;

        private:
            VulkanRenderPass m_renderPass;

            /// Depth image shared by every swapchain framebuffer.
            rendererResources::VulkanImage m_depthImage;

            /// View of m_depthImage, attached to every swapchain framebuffer.
            window::VulkanImageView m_depthImageView;

            std::vector<rendererResources::VulkanFramebuffer> m_framebuffers;

            std::vector<GraphicsPipelineEntry> m_graphicsPipelines;

            /// Registered meshes, indexed by GpuMeshHandle::index.
            std::vector<MeshEntry> m_meshList;
        };
    }
}
