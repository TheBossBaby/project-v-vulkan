#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkanRenderPass.hpp>

#include <deque>
#include <vector>
#include <optional>

namespace projectv
{
    namespace vulkan::builder
    {
        /**
         * @brief Builder for assembling and creating a VkRenderPass.
         *
         * Accumulates attachments, subpasses, and subpass dependencies through
         * a fluent API, then builds a fully-formed VulkanRenderPass via build().
         * Internal subpass storage uses std::deque so that references handed
         * out to subpass attachment lists remain stable as more subpasses are added.
         */
        class VulkanRenderPassBuilder
        {
        public:
            /**
             * @brief Adds a raw attachment description.
             * @param desc Fully-specified VkAttachmentDescription.
             * @return Index of the newly added attachment, for use in subpass refs.
             */
            uint32_t addAttachment(const VkAttachmentDescription& desc);

            /**
             * @brief Adds a color attachment with common defaults.
             *
             * @param format      Pixel format of the attachment.
             * @param finalLayout Layout the attachment must be in after the render
             *                    pass ends (default: ready for presentation).
             * @param samples     Sample count for multisampling (default: 1).
             * @param loadOp      Operation performed on load (default: clear).
             * @param storeOp     Operation performed on store (default: store).
             * @return Index of the newly added attachment, for use in subpass refs.
             */
            uint32_t addColorAttachment(
                VkFormat format,
                VkImageLayout finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT,
                VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_STORE);

            /**
             * @brief Adds a depth/stencil attachment with common defaults.
             *
             * @param format      Depth (or depth/stencil) format of the attachment.
             * @param finalLayout Layout the attachment must be in after the render
             *                    pass ends (default: depth/stencil attachment optimal).
             * @param samples     Sample count for multisampling (default: 1).
             * @return Index of the newly added attachment, for use in subpass refs.
             */
            uint32_t addDepthAttachment(
                VkFormat format,
                VkImageLayout finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);

            /**
             * @brief Begins a new subpass.
             * @param bindPoint Pipeline bind point for the subpass
             *                  (default: graphics).
             * @return Index of the newly created subpass, for use with
             *         addColorRef, addInputRef, etc.
             */
            uint32_t beginSubpass(VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS);

            /**
             * @brief Adds a color attachment reference to a subpass.
             * @param subpassIndex    Index returned by beginSubpass().
             * @param attachmentIndex Index returned by an addXAttachment() call.
             * @param layout          Layout the attachment should be in during
             *                        this subpass (default: color attachment optimal).
             */
            void addColorRef(uint32_t subpassIndex, uint32_t attachmentIndex,
                VkImageLayout layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

            /**
             * @brief Adds an input attachment reference to a subpass.
             * @param subpassIndex    Index returned by beginSubpass().
             * @param attachmentIndex Index returned by an addXAttachment() call.
             * @param layout          Layout the attachment should be in during
             *                        this subpass (default: shader read-only optimal).
             */
            void addInputRef(uint32_t subpassIndex, uint32_t attachmentIndex,
                VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

            /**
             * @brief Adds a resolve attachment reference to a subpass, used to
             *        resolve a multisampled color attachment.
             * @param subpassIndex    Index returned by beginSubpass().
             * @param attachmentIndex Index returned by an addXAttachment() call.
             * @param layout          Layout the attachment should be in during
             *                        this subpass (default: color attachment optimal).
             */
            void addResolveRef(uint32_t subpassIndex, uint32_t attachmentIndex,
                VkImageLayout layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

            /**
             * @brief Sets the depth/stencil attachment reference for a subpass.
             *
             * Only one depth/stencil reference can be set per subpass; calling
             * this again for the same subpass overwrites the previous value.
             *
             * @param subpassIndex    Index returned by beginSubpass().
             * @param attachmentIndex Index returned by addDepthAttachment().
             * @param layout          Layout the attachment should be in during
             *                        this subpass (default: depth/stencil attachment optimal).
             */
            void setDepthRef(uint32_t subpassIndex, uint32_t attachmentIndex,
                VkImageLayout layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

            /**
             * @brief Marks an attachment as preserved (untouched) during a subpass.
             * @param subpassIndex    Index returned by beginSubpass().
             * @param attachmentIndex Index of the attachment to preserve.
             */
            void addPreserveRef(uint32_t subpassIndex, uint32_t attachmentIndex);

            /**
             * @brief Adds a raw subpass dependency.
             * @param dependency Fully-specified VkSubpassDependency.
             */
            void addDependency(const VkSubpassDependency& dependency);

            /**
             * @brief Adds a subpass dependency from its individual fields.
             *
             * @param srcSubpass       Producer subpass index (or VK_SUBPASS_EXTERNAL).
             * @param dstSubpass       Consumer subpass index (or VK_SUBPASS_EXTERNAL).
             * @param srcStageMask     Pipeline stages the source subpass must complete.
             * @param dstStageMask     Pipeline stages the destination subpass waits at.
             * @param srcAccessMask    Memory access types performed by the source.
             * @param dstAccessMask    Memory access types performed by the destination.
             * @param dependencyFlags  Additional dependency flags (default: none).
             */
            void addDependency(
                uint32_t srcSubpass, uint32_t dstSubpass,
                VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
                VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask,
                VkDependencyFlags dependencyFlags = 0);

            /**
             * @brief Assembles all accumulated attachments, subpasses, and
             *        dependencies into a VkRenderPassCreateInfo and creates
             *        the render pass.
             *
             * @param device Logical device to create the render pass on.
             * @return A VulkanRenderPass owning the newly created handle.
             *
             * @throws std::runtime_error if render pass creation fails
             *         (propagated from VulkanRenderPass's constructor).
             */
            
            vulkan::VulkanRenderPass build(VkDevice device);
        private:
            /// Per-subpass accumulator for attachment references before build().
            struct SubpassInfo
            {
                VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
                std::vector<VkAttachmentReference> colorRefs;
                std::vector<VkAttachmentReference> inputRefs;
                std::vector<VkAttachmentReference> resolveRefs;
                std::optional<VkAttachmentReference> depthRef;
                std::vector<uint32_t> preserveRefs;
            };

            /// All attachment descriptions added so far, indexed by attachment index.
            std::vector<VkAttachmentDescription> m_attachments;

            /// Per-subpass state; deque: push_back never invalidates existing element addresses.
            std::deque<SubpassInfo> m_subpasses;

            /// All subpass dependencies added so far.
            std::vector<VkSubpassDependency> m_dependencies;
        };
    }
}