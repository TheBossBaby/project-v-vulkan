#include <projectV/engine/logging/log.hpp>

#include <vulkan/window/VulkanImageView.hpp>
#include <vulkan/util/vulkanCheck.hpp>

namespace projectv
{
    namespace vulkan::window
    {
        VulkanImageView::~VulkanImageView()
        {
            destroy();
        }

        VulkanImageView::VulkanImageView(VulkanImageView&& other) noexcept
            : m_imageView(other.m_imageView),
            m_logicalDevice(other.m_logicalDevice)
        {
            other.m_imageView = VK_NULL_HANDLE;
            other.m_logicalDevice = VK_NULL_HANDLE;
        }

        VulkanImageView& VulkanImageView::operator=(VulkanImageView&& other) noexcept
        {
            if (this != &other)
            {
                // Release any image view currently owned by this object.
                destroy();

                // Transfer ownership.
                m_imageView = other.m_imageView;
                m_logicalDevice = other.m_logicalDevice;

                // Leave the moved-from object empty.
                other.m_imageView = VK_NULL_HANDLE;
                other.m_logicalDevice = VK_NULL_HANDLE;
            }

            return *this;
        }

        void VulkanImageView::create(VkDevice logicalDevice, VkImage image, VkImageViewType viewType, VkFormat format, VkComponentMapping components, VkImageSubresourceRange subresourceRange)
        {
            m_logicalDevice = logicalDevice;

            engine::LogInfo("VulkanImageView::create, Creating ImageView.");

            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = image;
            createInfo.viewType = viewType;
            createInfo.format = format;

            createInfo.components = components;

            createInfo.subresourceRange = subresourceRange;

            vkCheck(vkCreateImageView(m_logicalDevice, &createInfo, nullptr, &m_imageView),
                "VulkanImageView::create, failed to create image views!");
        }

        VkImageView VulkanImageView::handle() const noexcept
        {
            return m_imageView;
        }

        void VulkanImageView::destroy()
        {
            if(m_logicalDevice != VK_NULL_HANDLE)
            {
                engine::LogInfo("VulkanImageView::destroy, Destroying ImageView.");
                vkDestroyImageView(m_logicalDevice, m_imageView, nullptr);

                m_imageView = VK_NULL_HANDLE;
                m_logicalDevice = VK_NULL_HANDLE;
            }
        }
    }
}
