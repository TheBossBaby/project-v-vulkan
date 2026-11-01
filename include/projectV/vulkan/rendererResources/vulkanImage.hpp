#pragma once

#include <vulkan/vulkan.h>

namespace projectv
{
    namespace vulkan
    {
        namespace rendererResources
        {
            /**
             * @brief RAII wrapper around a VkImage and its bound VkDeviceMemory.
             *
             * Owns both handles and releases them on destruction. Move-only.
             * Typically created via builder::VulkanImageBuilder.
             *
             * Also records the format, extent and allocation size the image
             * was created with.
             */
            class VulkanImage
            {
            public:
                /**
                 * @brief Constructs an empty wrapper with no owned image.
                 *
                 * Useful for deferred initialization; the object is invalid
                 * (operator bool() returns false) until move-assigned from
                 * a valid instance.
                 */
                VulkanImage() = default;

                /**
                 * @brief Takes ownership of an existing image and its memory.
                 * @param device       Logical device the image was created on.
                 * @param image        Image handle to own.
                 * @param deviceMemory Device memory bound to @p image, to own.
                 * @param format       Format the image was created with.
                 * @param extent       Size the image was created with, in texels.
                 * @param deviceSize   Size of the memory allocation backing
                 *                     @p image, in bytes.
                 */
                VulkanImage(VkDevice device, VkImage image, VkDeviceMemory deviceMemory, VkFormat format, VkExtent3D extent, VkDeviceSize deviceSize);

                /**
                 * @brief Destroys the owned image and frees its memory, if any.
                 */
                ~VulkanImage();

                /// Non-copyable: a VkImage has single-owner destruction semantics.
                VulkanImage(const VulkanImage&) = delete;

                /// Non-copyable: a VkImage has single-owner destruction semantics.
                VulkanImage& operator=(const VulkanImage&) = delete;

                /**
                 * @brief Move-constructs, transferring ownership from another wrapper.
                 * @param other Source wrapper; left in an empty, valid-to-destroy state.
                 */
                VulkanImage(VulkanImage&& other) noexcept;

                /**
                 * @brief Move-assigns, destroying any currently owned image
                 *        and transferring ownership from another wrapper.
                 * @param other Source wrapper; left in an empty, valid-to-destroy state.
                 * @return Reference to this object.
                 */
                VulkanImage& operator=(VulkanImage&& other) noexcept;

                /**
                 * @brief Returns the underlying VkImage handle.
                 * @return The owned handle, or VK_NULL_HANDLE if none is owned.
                 */
                VkImage handle() const noexcept;

                /**
                 * @brief Checks whether this wrapper owns a valid image.
                 * @return true if an image is owned, false otherwise.
                 */
                explicit operator bool() const noexcept;

                /**
                 * @brief Returns the format the image was created with.
                 * @return The image format, or VK_FORMAT_UNDEFINED if no image is owned.
                 */
                [[nodiscard]]
                VkFormat format() const noexcept;

                /**
                 * @brief Returns the size the image was created with.
                 * @return The image extent, in texels, or {0, 0, 1} if no image is owned.
                 */
                [[nodiscard]]
                VkExtent3D extent() const noexcept;

                /**
                 * @brief Returns the size of the memory allocation backing the image.
                 *
                 * This is the size the driver requires, including any padding
                 * for alignment or tiling, so it can be larger than
                 * width * height * bytes per texel.
                 *
                 * @return Allocation size in bytes, or 0 if no image is owned.
                 */
                VkDeviceSize getDeviceSize() const;

            private:
                /**
                 * @brief Destroys the owned image, frees its memory and resets
                 *        all state to empty. Safe to call multiple times.
                 */
                void destroy() noexcept;

                /// Logical device used to create/destroy m_image.
                VkDevice         m_logicalDevice  = VK_NULL_HANDLE;

                /// Owned image handle.
                VkImage          m_image          = VK_NULL_HANDLE;

                /// Owned device memory bound to m_image.
                VkDeviceMemory   m_deviceMemory   = VK_NULL_HANDLE;

                /// Format the image was created with.
                VkFormat         m_format         = VK_FORMAT_UNDEFINED;

                /// Size of the image, in texels.
                VkExtent3D       m_extent         = {0, 0, 1};

                /// Size of the memory allocation backing m_image, in bytes.
                VkDeviceSize     m_deviceSize     = 0;
            };
        }
    }
}
