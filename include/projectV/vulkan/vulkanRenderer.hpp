#pragma once

#include <projectV/core/renderer.hpp>

#include <vulkan/vulkan.h>

#include <span>
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

        namespace rendererResources
        {
            class VulkanBuffer;
        }

        class RendererResources;

        struct FrameData;

        class vulkanRenderer : public core::IRenderer
        {
        private:
            /**
             * @brief One buffer-to-buffer copy for submitCopy().
             *
             * Copies @c size bytes from offset 0 of @c srcBuffer to
             * offset 0 of @c dstBuffer. The buffers are borrowed, not owned,
             * and must stay alive until submitCopy() returns.
             */
            //ToDo : Move to some helper
            struct BufferCopyJob
            {
                /// Buffer to copy from; needs VK_BUFFER_USAGE_TRANSFER_SRC_BIT.
                VkBuffer     srcBuffer = VK_NULL_HANDLE;

                /// Buffer to copy to; needs VK_BUFFER_USAGE_TRANSFER_DST_BIT.
                VkBuffer     dstBuffer = VK_NULL_HANDLE;

                /// Number of bytes to copy; must not be 0 and must fit in both buffers.
                VkDeviceSize size      = 0;
            };
        public:
            vulkanRenderer();

            ~vulkanRenderer();

            bool init(const core::RendererConfig& config, core::IWindow& inWindow) override;

            void resize(std::uint32_t width, std::uint32_t height) override;

            void beginFrame(const core::RenderView& renderView) override;

            void draw(const std::vector<core::Renderable>& items) override;

            /**
             * @brief Uploads a CPU mesh to the GPU and registers it with the
             *        renderer resources.
             *
             * Picks the upload path for the selected GPU. If hasUnifiedMemory()
             * reports CPU-writable device-local memory, the mesh is written
             * there directly (unifiedMemoryUpload()). Otherwise it goes through
             * staging buffers into device-local memory (stagedUpload()),
             * which blocks until the GPU copies have finished.
             *
             * @param mesh Mesh whose vertices and indices are copied to the GPU.
             *             Must contain at least one vertex and one index.
             * @return Handle used to reference the uploaded mesh when drawing.
             *
             * @throws std::runtime_error if buffer creation, memory allocation,
             *         mapping or the copy submission fails.
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
         
            /**
             * @brief Runs a batch of buffer-to-buffer copies on the GPU and
             *        waits for all of them to finish.
             *
             * Records every job, in order, into one one-time command buffer
             * from the graphics command pool, submits it once to the graphics
             * queue, blocks the CPU on a fence until it completes, then frees
             * the command buffer. A batch costs one submit and one wait, not
             * one per copy. Meant for load time, not for every frame.
             *
             * Every job is checked before the fence and command buffer are
             * created, so an empty batch or a job with size 0 throws before
             * anything is recorded or submitted.
             *
             * No barriers are recorded between jobs, so the GPU may run them
             * in any order or at the same time. A job must not read or write
             * a range that another job in the same batch writes.
             *
             * @param bufferCopyJobs Copies to perform; must not be empty. The
             *                       buffers they name only need to stay alive
             *                       until this call returns.
             *
             * @throws std::runtime_error if @p bufferCopyJobs is empty, if any
             *         job's size is 0, or if recording, submitting or waiting
             *         for the copies fails.
             */
            void submitCopy(std::span<const BufferCopyJob> bufferCopyJobs);
            
            /**
             * @brief Uploads a mesh by writing it directly into buffers the
             *        CPU can map, without a staging copy.
             *
             * Used by uploadMesh() when hasUnifiedMemory() reports that the
             * CPU can write GPU memory directly. Creates a vertex and an index
             * buffer in device-local, host-visible, host-coherent memory,
             * copies the mesh data in through a mapping, and registers both
             * buffers with the renderer resources.
             *
             * @param mesh Mesh whose vertices and indices are copied.
             * @return Handle used to reference the uploaded mesh when drawing.
             *
             * @throws std::runtime_error if buffer creation, memory allocation
             *         or mapping fails.
             */
            core::gpu::GpuMeshHandle unifiedMemoryUpload(const struct core::Mesh& mesh);
            
            /**
             * @brief Uploads a mesh into device-local buffers through
             *        temporary staging buffers.
             *
             * Used by uploadMesh() on GPUs whose device-local memory the CPU
             * cannot write directly. Copies the mesh data into host-visible
             * staging buffers, has the GPU copy them into device-local vertex
             * and index buffers, and registers those with the renderer
             * resources.
             *
             * Both copies go to the GPU in a single submitCopy() call, which
             * blocks until they have finished, so the staging buffers can be
             * released before returning. Meant for load time, not for every
             * frame.
             *
             * @param mesh Mesh whose vertices and indices are copied.
             * @return Handle used to reference the uploaded mesh when drawing.
             *
             * @throws std::runtime_error if buffer creation, memory allocation,
             *         mapping or the copy submission fails.
             */
            core::gpu::GpuMeshHandle stagedUpload(const struct core::Mesh& mesh);

            /**
             * @brief Creates a buffer with bound device memory, used by one
             *        queue family at a time (exclusive sharing).
             * @param size   Buffer size, in bytes; must not be 0.
             * @param usage  How the buffer will be used, e.g.
             *               VK_BUFFER_USAGE_VERTEX_BUFFER_BIT.
             * @param memory Memory properties the backing allocation must have.
             * @return A VulkanBuffer owning the new buffer and its memory.
             *
             * @throws std::runtime_error if buffer creation or memory allocation
             *         fails, or no memory type has all of @p memory.
             */
            [[nodiscard]]
            rendererResources::VulkanBuffer createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memory);
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