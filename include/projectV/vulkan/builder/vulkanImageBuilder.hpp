#pragma once

#include <vulkan/vulkan.h>

#include <vulkan/rendererResources/vulkanImage.hpp>


namespace projectv
{
    namespace vulkan
    {
        namespace builder
        {
            /**
             * @brief Builder for assembling and creating a VkImage with bound device memory.
             *
             * Accumulates image creation parameters and the desired memory
             * properties through a fluent API, then builds a VulkanImage that
             * owns both the VkImage and its backing VkDeviceMemory via build().
             *
             * The defaults describe a single-sampled 2D image with one mip
             * level and one array layer, optimal tiling and device-local
             * memory. A format, an extent and usage flags must always be set.
             */
            class VulkanImageBuilder
            {
            public:
                /**
                 * @brief Sets additional image creation flags.
                 * @param createFlags VkImageCreateFlags bitmask (default: 0),
                 *        e.g. VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT for cube maps.
                 * @return Reference to this builder, for chaining.
                 */
                VulkanImageBuilder& setCreateFlags(VkImageCreateFlags createFlags);

                /**
                 * @brief Sets the dimensionality of the image.
                 * @param imageType Image type (default: VK_IMAGE_TYPE_2D).
                 * @return Reference to this builder, for chaining.
                 */
                VulkanImageBuilder& setImageType(VkImageType imageType);

                /**
                 * @brief Sets the format of the image's texels.
                 * @param format Texel format, e.g. a depth format returned by
                 *        VulkanPhysicalDevice::findSupportedFormat(). Must be set.
                 * @return Reference to this builder, for chaining.
                 */
                VulkanImageBuilder& setFormat(VkFormat format);

                /**
                 * @brief Sets the size of the image.
                 * @param width  Image width, in texels.
                 * @param height Image height, in texels.
                 * @param depth  Image depth, in texels (default: 1). Must be 1
                 *               unless the image type is VK_IMAGE_TYPE_3D.
                 * @return Reference to this builder, for chaining.
                 */
                VulkanImageBuilder& setExtent(uint32_t width, uint32_t height, uint32_t depth = 1);

                /**
                 * @brief Sets the number of mip levels.
                 * @param mipLevels Mip level count (default: 1, i.e. no mipmaps).
                 * @return Reference to this builder, for chaining.
                 */
                VulkanImageBuilder& setMipLevels(uint32_t mipLevels);

                /**
                 * @brief Sets the number of array layers.
                 * @param arrayLayers Layer count (default: 1). Use 6 together with
                 *        VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT for a cube map.
                 * @return Reference to this builder, for chaining.
                 */
                VulkanImageBuilder& setArrayLayers(uint32_t arrayLayers);

                /**
                 * @brief Sets the number of samples per texel.
                 * @param samples Sample count (default: VK_SAMPLE_COUNT_1_BIT).
                 *        Higher counts are used for multisampled (MSAA) targets.
                 * @return Reference to this builder, for chaining.
                 */
                VulkanImageBuilder& setSamples(VkSampleCountFlagBits samples);

                /**
                 * @brief Sets how texels are laid out in memory.
                 * @param tiling Image tiling (default: VK_IMAGE_TILING_OPTIMAL).
                 *        VK_IMAGE_TILING_LINEAR supports far fewer formats and usages.
                 * @return Reference to this builder, for chaining.
                 */
                VulkanImageBuilder& setTiling(VkImageTiling tiling);

                /**
                 * @brief Sets how the image will be used.
                 * @param usageFlags VkImageUsageFlags bitmask
                 *        (e.g. VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT). Must be set.
                 * @return Reference to this builder, for chaining.
                 */
                VulkanImageBuilder& setUsageFlags(VkImageUsageFlags usageFlags);

                /**
                 * @brief Sets the queue family sharing mode.
                 * @param sharingMode Sharing mode (default: VK_SHARING_MODE_EXCLUSIVE).
                 * @return Reference to this builder, for chaining.
                 *
                 * @note VK_SHARING_MODE_CONCURRENT also needs a list of queue
                 *       families, which this builder does not set yet.
                 */
                VulkanImageBuilder& setSharingMode(VkSharingMode sharingMode);

                /**
                 * @brief Sets the layout the image starts in.
                 * @param initialLayout VK_IMAGE_LAYOUT_UNDEFINED (default) or
                 *        VK_IMAGE_LAYOUT_PREINITIALIZED; no other layout is valid.
                 * @return Reference to this builder, for chaining.
                 */
                VulkanImageBuilder& setInitialLayout(VkImageLayout initialLayout);

                /**
                 * @brief Sets the memory properties the backing allocation must have.
                 * @param properties VkMemoryPropertyFlags bitmask
                 *        (default: VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT).
                 * @return Reference to this builder, for chaining.
                 */
                VulkanImageBuilder& setMemoryProperties(VkMemoryPropertyFlags properties);

                /**
                 * @brief Creates the image, allocates matching device memory
                 *        and binds the two together.
                 *
                 * If allocation or binding fails, any partially created
                 * resources are released before the exception propagates.
                 *
                 * @param device         Logical device to create the image on.
                 * @param physicalDevice Physical device queried for memory types.
                 * @return A VulkanImage owning the new image and its memory.
                 *
                 * @throws std::runtime_error if the format, extent or usage flags
                 *         were not set, if image creation, memory allocation or
                 *         binding fails, or if no suitable memory type exists.
                 */
                [[nodiscard]]
                rendererResources::VulkanImage build(VkDevice device, VkPhysicalDevice physicalDevice);

            private:
                /**
                 * @brief Allocates device memory satisfying the image's
                 *        requirements and the configured memory properties.
                 * @param device          Logical device to allocate on.
                 * @param physicalDevice  Physical device queried for memory types.
                 * @param memRequirements Memory requirements of the image, as
                 *                        returned by vkGetImageMemoryRequirements.
                 * @return The allocated memory, not yet bound to the image.
                 *
                 * @throws std::runtime_error if no suitable memory type exists
                 *         or the allocation fails.
                 */
                VkDeviceMemory allocateImageMemory(VkDevice device, VkPhysicalDevice physicalDevice, const VkMemoryRequirements& memRequirements);

            private:
                /// Image creation flags.
                VkImageCreateFlags    m_createFlags      = 0;

                /// Dimensionality of the image.
                VkImageType           m_imageType        = VK_IMAGE_TYPE_2D;

                /// Texel format.
                VkFormat              m_format           = VK_FORMAT_UNDEFINED;

                /// Size of the image, in texels.
                VkExtent3D            m_extent           = { 0, 0, 1 };

                /// Number of mip levels.
                uint32_t              m_mipLevels        = 1;

                /// Number of array layers.
                uint32_t              m_arrayLayers      = 1;

                /// Number of samples per texel.
                VkSampleCountFlagBits m_samples          = VK_SAMPLE_COUNT_1_BIT;

                /// Layout of texels in memory.
                VkImageTiling         m_tiling           = VK_IMAGE_TILING_OPTIMAL;

                /// Image usage flags.
                VkImageUsageFlags     m_usageFlags       = 0;

                /// Queue family sharing mode.
                VkSharingMode         m_sharingMode      = VK_SHARING_MODE_EXCLUSIVE;

                /// Layout the image starts in.
                VkImageLayout         m_initialLayout    = VK_IMAGE_LAYOUT_UNDEFINED;

                /// Required properties of the backing memory.
                VkMemoryPropertyFlags m_memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            };
        }
    }
}
