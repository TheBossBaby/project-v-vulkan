#pragma once
#include <vulkan/vulkan.h>

#include <span>
#include <vector>
namespace projectv
{
    namespace vulkan::window
    {
        /**
         * @brief RAII wrapper around a Vulkan image view.
         *
         * Owns a single VkImageView and automatically destroys it when the object
         * goes out of scope.
         */
        class VulkanImageView
        {
        public:
            /**
             * @brief Constructs an empty image view.
             */
            VulkanImageView() = default;

            /**
             * @brief Destroys the image view if one exists.
             */
            ~VulkanImageView();

            VulkanImageView(const VulkanImageView&) = delete;
            
            VulkanImageView& operator=(const VulkanImageView&) = delete;

            /**
             * @brief Move constructor.
             *
             * Transfers ownership of the underlying Vulkan image view.
             *
             * @param other Image view to move from.
             */
            VulkanImageView(VulkanImageView&& other) noexcept;

            /**
             * @brief Move assignment operator.
             *
             * Releases any currently owned image view before taking ownership
             * of the source object's image view.
             *
             * @param other Image view to move from.
             *
             * @return Reference to this object.
             */
            VulkanImageView& operator=(VulkanImageView&& other) noexcept;

            /**
             * @brief Creates the Vulkan image view.
             *
             * @param logicalDevice Logical device used to create the image view.
             * @param image Vulkan image to create the view for.
             * @param viewType Image view type.
             * @param format Image format.
             * @param components Component swizzle mapping.
             * @param subresourceRange Image subresource range.
             */
            void create(
                VkDevice logicalDevice,
                VkImage image,
                VkImageViewType viewType,
                VkFormat format,
                VkComponentMapping components,
                VkImageSubresourceRange subresourceRange);

            /**
             * @brief Returns the underlying Vulkan image view handle.
             *
             * @return Wrapped VkImageView handle.
             */
            [[nodiscard]]
            VkImageView handle() const noexcept;

        private:
            /**
             * @brief Destroys the Vulkan image view.
             */
            void destroy();

        private:
            VkImageView m_imageView = VK_NULL_HANDLE;

            VkDevice m_logicalDevice = VK_NULL_HANDLE;
        };
    }
}