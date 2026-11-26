#pragma once

#include <vulkan/vulkan.h>

#include <vulkan/rendererResources/vulkanSampler.hpp>

namespace projectv
{
    namespace vulkan
    {
        namespace builder
        {
            /**
             * @brief Builder for assembling and creating a VkSampler.
             *
             * Accumulates sampler parameters through a fluent API, then builds
             * a VulkanSampler that owns the new VkSampler via build().
             *
             * The defaults describe a trilinear sampler for mipmapped textures:
             * linear magnification, minification and mip filtering, repeat
             * addressing on every axis, every mip level reachable
             * (maxLod = VK_LOD_CLAMP_NONE), and no anisotropy or comparison.
             * Use clamp-to-edge addressing for cubemaps and other textures that
             * must not wrap.
             */
            class VulkanSamplerBuilder
            {
            public:
                /**
                 * @brief Sets additional sampler creation flags.
                 * @param flags VkSamplerCreateFlags bitmask (default: 0).
                 * @return Reference to this builder, for chaining.
                 */
                VulkanSamplerBuilder& setFlags(VkSamplerCreateFlags flags);

                /**
                 * @brief Sets the filter used when the texture is magnified.
                 * @param magFilter Filter (default: VK_FILTER_LINEAR).
                 * @return Reference to this builder, for chaining.
                 */
                VulkanSamplerBuilder& setMagFilter(VkFilter magFilter);

                /**
                 * @brief Sets the filter used when the texture is minified.
                 * @param minFilter Filter (default: VK_FILTER_LINEAR).
                 * @return Reference to this builder, for chaining.
                 */
                VulkanSamplerBuilder& setMinFilter(VkFilter minFilter);

                /**
                 * @brief Sets how texels are blended between mip levels.
                 * @param mipmapMode Mipmap mode (default: VK_SAMPLER_MIPMAP_MODE_LINEAR).
                 * @return Reference to this builder, for chaining.
                 */
                VulkanSamplerBuilder& setMipmapMode(VkSamplerMipmapMode mipmapMode);

                /**
                 * @brief Sets the same addressing mode for the U, V and W axes.
                 * @param addressMode Addressing mode (default: VK_SAMPLER_ADDRESS_MODE_REPEAT),
                 *        e.g. VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE for cubemaps.
                 * @return Reference to this builder, for chaining.
                 */
                VulkanSamplerBuilder& setAddressMode(VkSamplerAddressMode addressMode);

                /**
                 * @brief Sets the addressing mode of each axis separately.
                 * @param addressModeU Addressing mode for U (x).
                 * @param addressModeV Addressing mode for V (y).
                 * @param addressModeW Addressing mode for W (z).
                 * @return Reference to this builder, for chaining.
                 */
                VulkanSamplerBuilder& setAddressModes(VkSamplerAddressMode addressModeU,
                                                      VkSamplerAddressMode addressModeV,
                                                      VkSamplerAddressMode addressModeW);

                /**
                 * @brief Sets the bias added to the computed mip level.
                 * @param mipLodBias Bias (default: 0.0).
                 * @return Reference to this builder, for chaining.
                 */
                VulkanSamplerBuilder& setMipLodBias(float mipLodBias);

                /**
                 * @brief Sets anisotropic filtering.
                 *
                 * @param maxAnisotropy Maximum anisotropy (default: 1.0, which
                 *        disables anisotropic filtering). A value above 1.0
                 *        enables it; it must not exceed the device's
                 *        VkPhysicalDeviceLimits::maxSamplerAnisotropy, and the
                 *        samplerAnisotropy device feature must be enabled when
                 *        the logical device is created.
                 * @return Reference to this builder, for chaining.
                 */
                VulkanSamplerBuilder& setMaxAnisotropy(float maxAnisotropy);

                /**
                 * @brief Enables depth comparison, e.g. for shadow maps.
                 *
                 * Comparison is disabled by default. Calling this enables it with
                 * the given operator; such a sampler is used with depth images
                 * and shadow sampler types in shaders.
                 *
                 * @param compareOp Comparison operator.
                 * @return Reference to this builder, for chaining.
                 */
                VulkanSamplerBuilder& setCompareOp(VkCompareOp compareOp);

                /**
                 * @brief Sets the range of mip levels that may be sampled.
                 * @param minLod Lowest mip level (default: 0.0).
                 * @param maxLod Highest mip level (default: VK_LOD_CLAMP_NONE, so
                 *        every mip level of the image can be used). Must not be
                 *        less than minLod.
                 * @return Reference to this builder, for chaining.
                 */
                VulkanSamplerBuilder& setLodRange(float minLod, float maxLod);

                /**
                 * @brief Sets the colour returned outside the image with
                 *        VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER.
                 * @param borderColor Border colour
                 *        (default: VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK).
                 * @return Reference to this builder, for chaining.
                 */
                VulkanSamplerBuilder& setBorderColor(VkBorderColor borderColor);

                /**
                 * @brief Assembles all accumulated state into a
                 *        VkSamplerCreateInfo and creates the sampler.
                 *
                 * @param device Logical device to create the sampler on.
                 * @return A VulkanSampler owning the newly created handle.
                 *
                 * @throws std::runtime_error if the device is invalid, if
                 *         maxAnisotropy is less than 1.0, if maxLod is less than
                 *         minLod, or if sampler creation fails.
                 */
                [[nodiscard]]
                rendererResources::VulkanSampler build(VkDevice device);

            private:
                /// Sampler creation flags.
                VkSamplerCreateFlags m_flags         = 0;

                /// Filter used for magnification.
                VkFilter             m_magFilter     = VK_FILTER_LINEAR;

                /// Filter used for minification.
                VkFilter             m_minFilter     = VK_FILTER_LINEAR;

                /// How texels are blended between mip levels.
                VkSamplerMipmapMode  m_mipmapMode    = VK_SAMPLER_MIPMAP_MODE_LINEAR;

                /// Addressing mode for U (x).
                VkSamplerAddressMode m_addressModeU  = VK_SAMPLER_ADDRESS_MODE_REPEAT;

                /// Addressing mode for V (y).
                VkSamplerAddressMode m_addressModeV  = VK_SAMPLER_ADDRESS_MODE_REPEAT;

                /// Addressing mode for W (z).
                VkSamplerAddressMode m_addressModeW  = VK_SAMPLER_ADDRESS_MODE_REPEAT;

                /// Bias added to the computed mip level.
                float                m_mipLodBias    = 0.0f;

                /// Maximum anisotropy; 1.0 disables anisotropic filtering.
                float                m_maxAnisotropy = 1.0f;

                /// Whether depth comparison is enabled.
                VkBool32             m_compareEnable = VK_FALSE;

                /// Comparison operator, used when m_compareEnable is VK_TRUE.
                VkCompareOp          m_compareOp     = VK_COMPARE_OP_ALWAYS;

                /// Lowest mip level that may be sampled.
                float                m_minLod        = 0.0f;

                /// Highest mip level that may be sampled.
                float                m_maxLod        = VK_LOD_CLAMP_NONE;

                /// Colour returned outside the image with clamp-to-border.
                VkBorderColor        m_borderColor   = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
            };
        }
    }
}
