#pragma once

#include <vulkan/vulkan.h>

namespace projectv
{
    namespace vulkan
    {
        namespace rendererResources
        {
            /**
             * @brief RAII wrapper around a VkPipeline (graphics pipeline).
             *
             * Owns a VkPipeline handle and the VkDevice it was created from,
             * and guarantees the pipeline is destroyed via vkDestroyPipeline
             * when the wrapper goes out of scope. Non-copyable, move-only.
             *
             * A VulkanGraphicsPipeline does not own the VkPipelineLayout or
             * VkRenderPass it was built against; the caller is responsible
             * for keeping those alive for as long as this pipeline is used.
             */
            class VulkanGraphicsPipeline
            {
            public:
                /**
                 * @brief Constructs an empty wrapper with no owned pipeline.
                 *
                 * Useful for deferred initialization; the object is invalid
                 * (operator bool() returns false) until move-assigned from
                 * a valid instance.
                 */
                VulkanGraphicsPipeline() = default;

                /**
                 * @brief Creates a VkPipeline and takes ownership of it.
                 *
                 * @param device     Logical device used to create the pipeline.
                 *                   Stored internally and reused for destruction.
                 * @param createInfo Graphics pipeline creation parameters, as
                 *                   passed to vkCreateGraphicsPipelines.
                 *
                 * @throws std::runtime_error if vkCreateGraphicsPipelines fails.
                 */
                VulkanGraphicsPipeline(VkDevice device, const VkGraphicsPipelineCreateInfo& createInfo);

                /**
                 * @brief Destroys the owned pipeline, if any.
                 */
                ~VulkanGraphicsPipeline();

                /// Non-copyable: a VkPipeline has single-owner destruction semantics.
                VulkanGraphicsPipeline(const VulkanGraphicsPipeline&) = delete;

                /// Non-copyable: a VkPipeline has single-owner destruction semantics.
                VulkanGraphicsPipeline& operator=(const VulkanGraphicsPipeline&) = delete;

                /**
                 * @brief Move-constructs, transferring ownership from another wrapper.
                 * @param other Source wrapper; left in an empty, valid-to-destroy state.
                 */
                VulkanGraphicsPipeline(VulkanGraphicsPipeline&& other) noexcept;

                /**
                 * @brief Move-assigns, destroying any currently owned pipeline
                 *        and transferring ownership from another wrapper.
                 * @param other Source wrapper; left in an empty, valid-to-destroy state.
                 * @return Reference to this object.
                 */
                VulkanGraphicsPipeline& operator=(VulkanGraphicsPipeline&& other) noexcept;

                /**
                 * @brief Returns the underlying VkPipeline handle.
                 * @return The owned handle, or VK_NULL_HANDLE if none is owned.
                 */
                VkPipeline handle() const noexcept;

                /**
                 * @brief Checks whether this wrapper owns a valid pipeline.
                 * @return true if a pipeline is owned, false otherwise.
                 */
                explicit operator bool() const noexcept;

            private:
                /**
                 * @brief Destroys the owned pipeline and resets internal state
                 *        to VK_NULL_HANDLE. Safe to call multiple times.
                 */
                void destroy() noexcept;

                /// Logical device used to create/destroy m_pipeline.
                VkDevice   m_logicalDevice = VK_NULL_HANDLE;

                /// Owned pipeline handle.
                VkPipeline m_pipeline      = VK_NULL_HANDLE;
            };
        }
    }
}
