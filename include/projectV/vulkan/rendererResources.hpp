#pragma once

#include <projectV/core/graphicsPipelineHandle.hpp>
#include <projectV/core/gpu/gpuMeshHandle.hpp>

#include <vulkan/vulkanRenderPass.hpp>
#include <vulkan/rendererResources/meshEntry.hpp>
#include <vulkan/rendererResources/vulkanFramebuffer.hpp>
#include <vulkan/rendererResources/vulkanPipelineLayout.hpp>
#include <vulkan/rendererResources/vulkanGraphicsPipeline.hpp>

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

        private:
            VulkanRenderPass m_renderPass;

            std::vector<rendererResources::VulkanFramebuffer> m_framebuffers;

            std::vector<GraphicsPipelineEntry> m_graphicsPipelines;

            /// Registered meshes, indexed by GpuMeshHandle::index.
            std::vector<MeshEntry> m_meshList;
        };
    }
}
