#pragma once

#include <projectV/core/texture.hpp>

#include <vulkan/vulkan.h>

#include <array>
#include <cstddef>

namespace projectv
{
    namespace vulkan::util
    {
        namespace detail
        {
            struct TextureFormatMapping
            {
                core::TextureFormat coreFormat;

                VkFormat            vkFormat;
            };

            struct TextureTypeMapping
            {
                core::TextureType coreType;

                VkImageType   vkType;
            };

            struct TextureViewTypeMapping
            {
                core::TextureType coreType;

                VkImageViewType   vkImageViewType;
            };

            /// Indexed by core::TextureFormat; entry i must describe format i.
            inline constexpr std::array<TextureFormatMapping, static_cast<std::size_t>(core::TextureFormat::MAX)> kFormatMappings =
            {{
                { core::TextureFormat::Unknown,            VK_FORMAT_UNDEFINED              },
                { core::TextureFormat::R8G8B8A8_UNorm,     VK_FORMAT_R8G8B8A8_UNORM         },
                { core::TextureFormat::R16G16B16A16_Float, VK_FORMAT_R16G16B16A16_SFLOAT    },
                { core::TextureFormat::R32G32B32A32_Float, VK_FORMAT_R32G32B32A32_SFLOAT    },
                { core::TextureFormat::D24S8,              VK_FORMAT_D24_UNORM_S8_UINT      },
                { core::TextureFormat::R8G8B8A8_SRGB,      VK_FORMAT_R8G8B8A8_SRGB          },
                { core::TextureFormat::E5B9G9R9_UFloat,    VK_FORMAT_E5B9G9R9_UFLOAT_PACK32 },
                { core::TextureFormat::ASTC_4x4_UNorm,     VK_FORMAT_ASTC_4x4_UNORM_BLOCK   },
                { core::TextureFormat::ASTC_4x4_SRGB,      VK_FORMAT_ASTC_4x4_SRGB_BLOCK    }
            }};

            /// Indexed by core::TextureType; entry i must describe type i.
            inline constexpr std::array<TextureViewTypeMapping, static_cast<std::size_t>(core::TextureType::MAX)> kTextureViewTypeMappings =
            {{
                { core::TextureType::Texture2D, VK_IMAGE_VIEW_TYPE_2D   },
                { core::TextureType::CubeMap,   VK_IMAGE_VIEW_TYPE_CUBE }
            }};

            /// Indexed by core::TextureType; entry i must describe type i.
            inline constexpr std::array<TextureTypeMapping, static_cast<std::size_t>(core::TextureType::MAX)> kTextureTypeMappings =
            {{
                { core::TextureType::Texture2D, VK_IMAGE_TYPE_2D },
                { core::TextureType::CubeMap,   VK_IMAGE_TYPE_2D }
            }};

            template <typename Mappings, typename Member>
            constexpr bool isIndexedByCoreEnum(const Mappings& mappings, Member member)
            {
                for (std::size_t i = 0; i < mappings.size(); ++i)
                {
                    if (static_cast<std::size_t>(mappings[i].*member) != i)
                    {
                        return false;
                    }
                }
                return true;
            }

            // Lookups index the tables directly, so a missing or reordered
            // entry would silently return the wrong Vulkan value.
            static_assert(isIndexedByCoreEnum(kFormatMappings, &TextureFormatMapping::coreFormat),
                "kFormatMappings must list every core::TextureFormat, in enum order");

            static_assert(isIndexedByCoreEnum(kTextureViewTypeMappings, &TextureViewTypeMapping::coreType),
                "kTextureViewTypeMappings must list every core::TextureType, in enum order");
            
            static_assert(isIndexedByCoreEnum(kTextureTypeMappings, &TextureTypeMapping::coreType),
                "kTextureTypeMappings must list every core::TextureType, in enum order");
        }

        /**
         * @brief Converts a core texture format to the matching Vulkan format.
         *
         * @param format Core texture format to convert.
         * @return The matching VkFormat. core::TextureFormat::Unknown, and any
         *         value outside the enum's range, map to VK_FORMAT_UNDEFINED.
         */
        [[nodiscard]]
        constexpr VkFormat toVulkanFormat(core::TextureFormat format)
        {
            const auto index = static_cast<std::size_t>(format);
            if (index >= detail::kFormatMappings.size())
            {
                return VK_FORMAT_UNDEFINED;
            }
            return detail::kFormatMappings[index].vkFormat;
        }

        /**
         * @brief Converts a core texture type to the matching image view type.
         *
         * @param type Core texture type to convert.
         * @return The matching VkImageViewType, or VK_IMAGE_VIEW_TYPE_MAX_ENUM
         *         if @p type is outside the enum's range.
         */
        [[nodiscard]]
        constexpr VkImageViewType toVulkanImageViewType(core::TextureType type)
        {
            const auto index = static_cast<std::size_t>(type);
            if (index >= detail::kTextureViewTypeMappings.size())
            {
                return VK_IMAGE_VIEW_TYPE_MAX_ENUM;
            }
            return detail::kTextureViewTypeMappings[index].vkImageViewType;
        }

        [[nodiscard]]
        constexpr VkImageType toVulkanImageType(core::TextureType type)
        {
            const auto index = static_cast<std::size_t>(type);
            if (index >= detail::kTextureTypeMappings.size())
            {
                return VK_IMAGE_TYPE_MAX_ENUM;
            }
            return detail::kTextureTypeMappings[index].vkType;
        }
    }
}
