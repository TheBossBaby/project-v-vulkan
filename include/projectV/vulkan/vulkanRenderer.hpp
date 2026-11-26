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
            class VulkanImage;
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

            /**
             * @brief Creates the Vulkan instance, devices, swapchain and every
             *        resource needed to render frames and upload meshes.
             *
             * Order matters: the per-frame descriptor set layout, descriptor
             * pool and camera uniform sets are created before any graphics
             * pipeline can be, and the upload command buffer after the
             * graphics command pool.
             *
             * @param config   Renderer settings. Currently unused: the
             *                 swapchain size is read from @p inWindow.
             * @param inWindow Window to render into; must outlive the renderer.
             * @return true once initialization has finished. Failures are
             *         reported by throwing, not by returning false.
             *
             * @throws std::runtime_error if creating any Vulkan object fails.
             */
            bool init(const core::RendererConfig& config, core::IWindow& inWindow) override;

            void resize(std::uint32_t width, std::uint32_t height) override;

            /**
             * @brief Starts recording a frame in the current frame-in-flight
             *        slot.
             *
             * Waits on the slot's fence, acquires the next swapchain image,
             * begins the slot's command buffer and the swapchain render pass
             * (clearing color to @c renderView.clearColor and depth to 1.0),
             * sets the viewport and scissor to the swapchain extent, and
             * writes the camera uniforms for the slot. The fence wait is what
             * makes overwriting the slot's uniform buffer safe.
             *
             * If the swapchain is out of date, logs a warning and returns
             * without starting a frame; draw() and endFrame() then do nothing.
             *
             * @param renderView View-projection matrix, camera position and
             *                   clear color for this frame.
             *
             * @throws std::runtime_error if a Vulkan call fails.
             */
            void beginFrame(const core::RenderView& renderView) override;

            /**
             * @brief Records draw commands for a list of renderables into the
             *        current frame.
             *
             * Items whose pipeline or mesh handle is invalid are skipped.
             * Whenever the pipeline changes, it is bound together with the
             * frame's camera descriptor set at config::FrameSetIndex. Each
             * item's model matrix is passed to the vertex shader as a push
             * constant at offset 0. Meshes with an index buffer are drawn
             * indexed, others non-indexed.
             *
             * Items are drawn in the given order, so sorting them by pipeline
             * reduces rebinds. Does nothing unless beginFrame() started a
             * frame.
             *
             * @param items Renderables to draw this frame.
             */
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

            core::gpu::GpuTextureHandle uploadTexture(const struct core::Texture& texture) override;

            /**
             * @brief Builds a graphics pipeline from SPIR-V shaders and
             *        registers it with the renderer resources.
             *
             * The pipeline layout has the per-frame descriptor set layout as
             * set 0 (config::FrameSetIndex, camera uniforms) and a
             * vertex-stage push constant range of sizeof(core::math::Matrix4)
             * bytes at offset 0 for the model matrix. Vertex input is one
             * binding of core::Vertex with position, normal, color and uv at
             * locations 0 to 3. The pipeline targets the swapchain render pass.
             *
             * @param description Vertex and fragment shader SPIR-V bytecode.
             * @return Handle to the new pipeline, or an invalid handle (and an
             *         error is logged) if either shader is empty or the frame
             *         descriptor set layout has not been created.
             *
             * @throws std::runtime_error if creating the shader modules, the
             *         pipeline layout or the pipeline fails.
             */
            core::GraphicsPipelineHandle createGraphicsPipeline(const core::GraphicsPipelineDescription& description) override;

            void endFrame() override;

            /**
             * @brief Blocks until the GPU has finished all submitted work.
             *
             * Does nothing if the logical device has not been created.
             *
             * @throws std::runtime_error if vkDeviceWaitIdle fails.
             */
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
             * @brief Allocates the primary command buffer that submitCopy()
             *        records every upload into.
             *
             * Allocating it once and reusing it avoids allocating and freeing
             * a command buffer on every upload. Must be called once, after
             * graphicsCommandPool has been created and before the first
             * submitCopy(); init() does this. The pool frees the command
             * buffer when the pool is destroyed.
             *
             * @throws std::runtime_error if the command buffer allocation fails.
             */
            void createUploadCommandBuffer();
         
            /**
             * @brief Runs a batch of buffer-to-buffer copies on the GPU and
             *        waits for all of them to finish.
             *
             * Records every job, in order, into uploadCommandBuffer, submits
             * it once to the graphics queue and blocks the CPU on a fence
             * until it completes. A batch costs one submit and one wait, not
             * one per copy. Meant for load time, not for every frame.
             *
             * The same command buffer is reused on every call. begin() resets
             * it implicitly, and because this call waits for the GPU, it is
             * never still pending when the next call starts. With only one
             * upload command buffer, uploads cannot overlap. It comes from
             * graphicsCommandPool, so call this on the thread that records
             * frames: Vulkan allows a pool and its command buffers to be used
             * by only one thread at a time.
             *
             * The command buffer and every job are checked before the fence
             * is created, so a missing command buffer, an empty batch or a
             * job with size 0 throws before anything is recorded or
             * submitted.
             *
             * No barriers are recorded between jobs, so the GPU may run them
             * in any order or at the same time. A job must not read or write
             * a range that another job in the same batch writes.
             *
             * @pre createUploadCommandBuffer() has been called.
             *
             * @param bufferCopyJobs Copies to perform; must not be empty. The
             *                       buffers they name only need to stay alive
             *                       until this call returns.
             *
             * @throws std::runtime_error if uploadCommandBuffer has not been
             *         created, if @p bufferCopyJobs is empty, if any job's
             *         size is 0, or if recording, submitting or waiting for
             *         the copies fails.
             */
            void submitCopy(std::span<const BufferCopyJob> bufferCopyJobs);

            void submitCopy(rendererResources::VulkanImage& inTexture, std::span<const VkBufferImageCopy> region, rendererResources::VulkanBuffer& inImgBuffer);
            
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

            core::gpu::GpuTextureHandle unifiedMemoryUpload(const struct core::Texture& texture);

            core::gpu::GpuTextureHandle stagedUpload(const struct core::Texture& texture);

            /**
             * @brief Creates the descriptor pool for the per-frame descriptor
             *        sets and stores it in the renderer resources.
             *
             * Holds one uniform buffer descriptor and config::MaxMainCubeMap
             * combined image sampler descriptors for each of the
             * config::MaxFramesInFlight per-frame sets. Must be called before
             * createFrameUniformSets().
             *
             * @throws std::runtime_error if descriptor pool creation fails.
             */
            void createDescriptorPool();

            VkDescriptorSetLayoutBinding createCubeMapSamplerBinding();

            /**
             * @brief Creates the layout of the per-frame descriptor set and
             *        stores it in the renderer resources.
             *
             * The layout has a single uniform buffer binding at
             * config::CameraBindingIndex, holding CameraUniforms and visible
             * to the vertex and fragment stages. Must be called before
             * createFrameUniformSets() and before any createGraphicsPipeline().
             *
             * @throws std::runtime_error if descriptor set layout creation fails.
             */
            void createFrameSetLayout();

            /**
             * @brief Creates one camera uniform buffer and descriptor set per
             *        frame in flight and stores them in the renderer resources.
             *
             * Each frame gets its own set because a set used by a command
             * buffer that is still pending must not be updated. Each buffer is
             * host-visible and host-coherent, sized for CameraUniforms, and
             * stays mapped for the renderer's lifetime. The descriptor is
             * written once here because the buffer never changes;
             * writeCameraUniforms() only updates the buffer contents.
             *
             * Call once: the pool only has room for one set per frame in
             * flight.
             *
             * @pre createFrameSetLayout() and createDescriptorPool() have been
             *      called.
             *
             * @throws std::runtime_error if the frame set layout has not been
             *         created, or if allocating the sets or creating or
             *         mapping a buffer fails.
             */
            void createFrameUniformSets();

            /**
             * @brief Writes the camera data for this frame into the current
             *        frame slot's uniform buffer.
             *
             * Fills CameraUniforms with @p vpMatrix, its inverse and
             * @p cameraPosition (w = 1), and copies it through the buffer's
             * persistent mapping. Only safe once the slot's fence has been
             * waited on, so the GPU is no longer reading the buffer;
             * beginFrame() calls it after that wait.
             *
             * @pre createFrameUniformSets() has been called.
             *
             * @param vpMatrix       Combined view-projection matrix.
             * @param cameraPosition Camera position in world space.
             *
             * @throws std::runtime_error if the uniform buffer is not mapped.
             */
            void writeCameraUniforms(const core::math::Matrix4& vpMatrix, const core::math::Vector3& cameraPosition);

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

            void writeUniformDescSet(VkWriteDescriptorSet& descWrite, VkDescriptorSet descSet, VkDescriptorBufferInfo& bufferInfo);

            void writeSamplerDescSet(VkWriteDescriptorSet& descWrite, VkDescriptorSet descSet, VkDescriptorImageInfo& imgInfo);

            void createCubeSampler();

            /**
             * @brief Points the cube map binding (config::CubeMapBindingIndex)
             *        of every per-frame descriptor set at a texture.
             *
             * Writes the texture's image view together with the cube map
             * sampler as a combined image sampler, in
             * SHADER_READ_ONLY_OPTIMAL layout.
             *
             * No per-frame set may be in use by a pending command buffer, so
             * call this before rendering starts or after the device is idle.
             *
             * @pre createFrameUniformSets() and createCubeSampler() have been
             *      called.
             *
             * @param cubeMapHandle Handle returned by uploadTexture() for a
             *                      cube map texture.
             *
             * @throws std::runtime_error if @p cubeMapHandle is unknown, the
             *         sampler is missing, or no frame sets exist yet.
             */
            void updateCubeMapDescriptorSets(core::gpu::GpuTextureHandle cubeMapHandle);
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

            /**
             * @brief Primary command buffer reused by every submitCopy() call.
             *
             * Allocated once from graphicsCommandPool by
             * createUploadCommandBuffer(); null until then. Declared after
             * graphicsCommandPool, so this wrapper is destroyed first; the
             * pool frees the command buffer itself.
             */
            std::unique_ptr<command::VulkanCommandBuffer> uploadCommandBuffer;

            uint32_t currentImageIndex = UINT32_MAX;

            uint32_t currentFrame = 0;

            bool isFrameRunning = false;

            std::vector<FrameData> frames;
        };
    }
}