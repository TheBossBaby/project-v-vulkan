#pragma once

#include <vulkan/vulkan.h>

#include <stdexcept>

namespace projectv
{
    namespace vulkan::util
    {
        /**
         * @brief Returns the image aspects that make up a depth and/or stencil format.
         *
         * Use it for the aspectMask of an image view, image barrier or clear that
         * targets a depth/stencil image:
         * - depth-only formats (e.g. D32_SFLOAT)       -> VK_IMAGE_ASPECT_DEPTH_BIT
         * - stencil-only format (S8_UINT)              -> VK_IMAGE_ASPECT_STENCIL_BIT
         * - combined formats (e.g. D24_UNORM_S8_UINT)  -> DEPTH | STENCIL
         *
         * @note A view that is sampled in a shader must expose a single aspect, so
         *       the combined result of this helper is meant for attachment views and
         *       barriers, not for sampled views.
         *
         * @param format A depth and/or stencil format, such as one returned by
         *               VulkanPhysicalDevice::findSupportedFormat().
         * @return The aspect flags that exist for @p format.
         * @throws std::runtime_error If @p format is not a depth/stencil format.
         */
        inline VkImageAspectFlags depthStencilAspectFlags(VkFormat format)
        {
            switch (format)
            {
            case VK_FORMAT_D16_UNORM:
            case VK_FORMAT_X8_D24_UNORM_PACK32:
            case VK_FORMAT_D32_SFLOAT:
                return VK_IMAGE_ASPECT_DEPTH_BIT;

            case VK_FORMAT_S8_UINT:
                return VK_IMAGE_ASPECT_STENCIL_BIT;

            case VK_FORMAT_D16_UNORM_S8_UINT:
            case VK_FORMAT_D24_UNORM_S8_UINT:
            case VK_FORMAT_D32_SFLOAT_S8_UINT:
                return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

            default:
                throw std::runtime_error("depthStencilAspectFlags, format is not a depth/stencil format");
            }
        }
    }
}