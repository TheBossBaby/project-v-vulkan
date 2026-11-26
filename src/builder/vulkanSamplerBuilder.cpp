#include <projectV/engine/logging/log.hpp>

#include <vulkan/builder/vulkanSamplerBuilder.hpp>

#include <stdexcept>

namespace projectv
{
    namespace vulkan
    {
        namespace builder
        {
            VulkanSamplerBuilder& VulkanSamplerBuilder::setFlags(VkSamplerCreateFlags flags)
            {
                m_flags = flags;
                return *this;
            }

            VulkanSamplerBuilder& VulkanSamplerBuilder::setMagFilter(VkFilter magFilter)
            {
                m_magFilter = magFilter;
                return *this;
            }

            VulkanSamplerBuilder& VulkanSamplerBuilder::setMinFilter(VkFilter minFilter)
            {
                m_minFilter = minFilter;
                return *this;
            }

            VulkanSamplerBuilder& VulkanSamplerBuilder::setMipmapMode(VkSamplerMipmapMode mipmapMode)
            {
                m_mipmapMode = mipmapMode;
                return *this;
            }

            VulkanSamplerBuilder& VulkanSamplerBuilder::setAddressMode(VkSamplerAddressMode addressMode)
            {
                return setAddressModes(addressMode, addressMode, addressMode);
            }

            VulkanSamplerBuilder& VulkanSamplerBuilder::setAddressModes(VkSamplerAddressMode addressModeU,
                                                                        VkSamplerAddressMode addressModeV,
                                                                        VkSamplerAddressMode addressModeW)
            {
                m_addressModeU = addressModeU;
                m_addressModeV = addressModeV;
                m_addressModeW = addressModeW;
                return *this;
            }

            VulkanSamplerBuilder& VulkanSamplerBuilder::setMipLodBias(float mipLodBias)
            {
                m_mipLodBias = mipLodBias;
                return *this;
            }

            VulkanSamplerBuilder& VulkanSamplerBuilder::setMaxAnisotropy(float maxAnisotropy)
            {
                m_maxAnisotropy = maxAnisotropy;
                return *this;
            }

            VulkanSamplerBuilder& VulkanSamplerBuilder::setCompareOp(VkCompareOp compareOp)
            {
                m_compareEnable = VK_TRUE;
                m_compareOp     = compareOp;
                return *this;
            }

            VulkanSamplerBuilder& VulkanSamplerBuilder::setLodRange(float minLod, float maxLod)
            {
                m_minLod = minLod;
                m_maxLod = maxLod;
                return *this;
            }

            VulkanSamplerBuilder& VulkanSamplerBuilder::setBorderColor(VkBorderColor borderColor)
            {
                m_borderColor = borderColor;
                return *this;
            }

            rendererResources::VulkanSampler VulkanSamplerBuilder::build(VkDevice device)
            {
                if (device == VK_NULL_HANDLE)
                {
                    engine::LogError("VulkanSamplerBuilder::build, invalid VkDevice");
                    throw std::runtime_error("VulkanSamplerBuilder::build, invalid VkDevice");
                }

                if (m_maxAnisotropy < 1.0f)
                {
                    engine::LogError("VulkanSamplerBuilder::build, maxAnisotropy must be at least 1.0");
                    throw std::runtime_error("VulkanSamplerBuilder::build, maxAnisotropy must be at least 1.0");
                }

                if (m_maxLod < m_minLod)
                {
                    engine::LogError("VulkanSamplerBuilder::build, maxLod must not be less than minLod");
                    throw std::runtime_error("VulkanSamplerBuilder::build, maxLod must not be less than minLod");
                }

                VkSamplerCreateInfo createInfo{};
                createInfo.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
                createInfo.flags                   = m_flags;
                createInfo.magFilter               = m_magFilter;
                createInfo.minFilter               = m_minFilter;
                createInfo.mipmapMode              = m_mipmapMode;
                createInfo.addressModeU            = m_addressModeU;
                createInfo.addressModeV            = m_addressModeV;
                createInfo.addressModeW            = m_addressModeW;
                createInfo.mipLodBias              = m_mipLodBias;
                createInfo.anisotropyEnable        = m_maxAnisotropy > 1.0f ? VK_TRUE : VK_FALSE;
                createInfo.maxAnisotropy           = m_maxAnisotropy;
                createInfo.compareEnable           = m_compareEnable;
                createInfo.compareOp               = m_compareOp;
                createInfo.minLod                  = m_minLod;
                createInfo.maxLod                  = m_maxLod;
                createInfo.borderColor             = m_borderColor;
                createInfo.unnormalizedCoordinates = VK_FALSE;

                return rendererResources::VulkanSampler(device, createInfo);
            }
        }
    }
}
