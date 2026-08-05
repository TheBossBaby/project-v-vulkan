#pragma once

#include <vulkan/vulkan.h>
#include <span>
#include <vector>

#include <vulkan/rendererResources/vulkanFramebuffer.hpp>
#include <vulkan/window/VulkanImageView.hpp>

namespace projectv
{
    namespace vulkan
    {
        namespace builder
        {
            /**
             * @brief Builder for assembling and creating a VkFramebuffer.
             *
             * Accumulates attachment image views and dimensions through a fluent
             * API, then builds a fully-formed VulkanFramebuffer via build().
             * Must be bound to a compatible render pass before building.
             */
            class VulkanFramebufferBuilder
            {
            public:
                /**
                 * @brief Sets the render pass this framebuffer must be compatible with.
                 * @param renderPass Render pass handle, as passed to vkCreateFramebuffer.
                 * @return Reference to this builder, for chaining.
                 */
                VulkanFramebufferBuilder& setRenderPass(VkRenderPass renderPass);

                /**
                 * @brief Appends a single attachment image view.
                 *
                 * Attachments are stored in the order they are added, and must
                 * match the attachment order expected by the bound render pass.
                 *
                 * @param imageView Image view to attach.
                 * @return Reference to this builder, for chaining.
                 */
                VulkanFramebufferBuilder& addAttachment(VkImageView imageView);

                /**
                 * @brief Replaces the full attachment list at once.
                 * @param imageViews Ordered list of attachment image views.
                 * @return Reference to this builder, for chaining.
                 */
                VulkanFramebufferBuilder& setAttachments(std::span<const window::VulkanImageView> imageViews);

                /**
                 * @brief Sets the framebuffer dimensions.
                 * @param width  Framebuffer width, in pixels.
                 * @param height Framebuffer height, in pixels.
                 * @return Reference to this builder, for chaining.
                 */
                VulkanFramebufferBuilder& setExtent(uint32_t width, uint32_t height);

                /**
                 * @brief Sets the number of layers in the framebuffer.
                 * @param layers Layer count (default: 1, i.e. non-layered).
                 * @return Reference to this builder, for chaining.
                 */
                VulkanFramebufferBuilder& setLayers(uint32_t layers);

                /**
                 * @brief Sets additional framebuffer creation flags.
                 * @param flags VkFramebufferCreateFlags bitmask.
                 * @return Reference to this builder, for chaining.
                 */
                VulkanFramebufferBuilder& setFlags(VkFramebufferCreateFlags flags);

                /**
                 * @brief Assembles all configured state into a
                 *        VkFramebufferCreateInfo and creates the framebuffer.
                 *
                 * @param device Logical device to create the framebuffer on.
                 * @return A VulkanFramebuffer owning the newly created handle.
                 *
                 * @throws std::runtime_error if the render pass was not set,
                 *         or if framebuffer creation fails.
                 */
                rendererResources::VulkanFramebuffer build(VkDevice device);

            private:
                /// Render pass this framebuffer must be compatible with.
                VkRenderPass m_renderPass = VK_NULL_HANDLE;

                /// Ordered attachment image views.
                std::vector<VkImageView> m_attachments;

                /// Framebuffer width, in pixels.
                uint32_t m_width  = 0;

                /// Framebuffer height, in pixels.
                uint32_t m_height = 0;

                /// Number of layers in the framebuffer.
                uint32_t m_layers = 1;

                /// Additional framebuffer creation flags.
                VkFramebufferCreateFlags m_flags = 0;
            };
        }
    }
}