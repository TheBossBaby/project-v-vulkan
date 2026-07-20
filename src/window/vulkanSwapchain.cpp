#include <vulkanSwapchain.hpp>

#include <projectV/engine/logging/log.hpp>

#include <vulkan/config/vulkanConfig.hpp>

#include <vulkan/util/vulkanCheck.hpp>

#include <algorithm>
#include <format>
#include <stdexcept>
namespace projectv
{
    namespace vulkan::window
    {
        VulkanSwapchain::VulkanSwapchain()
        {
        }

        VulkanSwapchain::~VulkanSwapchain()
        {
            destroy();
        }

        VkSwapchainKHR VulkanSwapchain::handle() const noexcept
        {
            return m_swapchain;
        }

        void VulkanSwapchain::create(const device::VulkanPhysicalDevice&  physicalDevice, VkDevice  logicalDevice, VkSurfaceKHR surface, uint32_t width, uint32_t height)
        {
            engine::LogInfo("VulkanSwapchain::create, Creating swapchain.");
            m_physicalDevice = physicalDevice.handle();
            m_logicalDevice = logicalDevice;
            m_surface = surface;

            check();

            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_physicalDevice, m_surface);

            VkSurfaceFormatKHR surfaceFormat = selectSwapSurfaceFormat(swapChainSupport.formats);
            VkPresentModeKHR presentMode = selectSwapPresentMode(swapChainSupport.presentModes);
            VkExtent2D extent = selectSwapExtent(swapChainSupport.capabilities, width, height);
            uint32_t imageCount = selectImageCount(swapChainSupport.capabilities);

            auto indices = physicalDevice.queueFamilies();
            uint32_t queueFamilyIndices[] = {indices.graphics.value(), indices.presentation.value()};

            VkSwapchainCreateInfoKHR createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            createInfo.surface = m_surface;
            createInfo.minImageCount = imageCount;
            createInfo.imageFormat = surfaceFormat.format;
            createInfo.imageColorSpace = surfaceFormat.colorSpace;
            createInfo.imageExtent = extent;
            createInfo.imageArrayLayers = config::SwapchainImageArrayLayers;
            createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

            if (indices.graphics != indices.presentation) 
            {
                createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
                createInfo.queueFamilyIndexCount = 2;
                createInfo.pQueueFamilyIndices = queueFamilyIndices;
            } 
            else 
            {
                createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
                createInfo.queueFamilyIndexCount = 0; // Optional
                createInfo.pQueueFamilyIndices = nullptr; // Optional
            }

            createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
            createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            createInfo.presentMode = presentMode;
            createInfo.clipped = VK_TRUE;
            createInfo.oldSwapchain = VK_NULL_HANDLE;

            vkCheck(
                vkCreateSwapchainKHR(m_logicalDevice, &createInfo, nullptr, &m_swapchain),
                "VulkanSwapchain::create, Failed to create Swapchain !"
            );
            engine::LogInfo("VulkanSwapchain::create, Swapchain created successfully.");
        }

        bool VulkanSwapchain::check()
        {
            bool swapChainAdequate = false;

            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_physicalDevice, m_surface);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();

            if(!swapChainAdequate)
            {
                engine::LogError("VulkanSwapchain::check, Insufficent Swapchain support for selected Physical Device");
                throw std::runtime_error("VulkanSwapchain::check, Insufficent Swapchain support for selected Physical Device");
            }

            return swapChainAdequate;
        }

        SwapChainSupportDetails VulkanSwapchain::querySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
        {
            SwapChainSupportDetails details;

            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);

            uint32_t formatCount;
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

            if (formatCount != 0)
            {
                details.formats.resize(formatCount);
                vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.formats.data());
            }

            uint32_t presentModeCount;
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

            if (presentModeCount != 0)
            {
                details.presentModes.resize(presentModeCount);
                vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, details.presentModes.data());
            }

            engine::LogTrace(std::format(
                "VulkanSwapchain::querySwapChainSupport, Found {} surface formats and {} presentation modes.",
                details.formats.size(),
                details.presentModes.size()));

            return details;
        }

        VkSurfaceFormatKHR VulkanSwapchain::selectSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
        {
            for (const auto& availableFormat : availableFormats) 
            {
                if (availableFormat.format == config::SwapchainFormat && availableFormat.colorSpace == config::SwapchainColorSpace) 
                {
                    engine::LogInfo("VulkanSwapchain::selectSwapSurfaceFormat, Selected preferred surface format.");
                    return availableFormat;
                }
            }

            engine::LogWarn("VulkanSwapchain::selectSwapSurfaceFormat, Preferred surface format unavailable. Using first supported format.");
            return availableFormats[0];
        }

        VkPresentModeKHR VulkanSwapchain::selectSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
        {
            for (const auto& availablePresentMode : availablePresentModes) 
            {
                if (availablePresentMode == config::SwapchainPresentMode) 
                {
                    engine::LogInfo("VulkanSwapchain::selectSwapPresentMode, Using MAILBOX presentation mode.");
                    return availablePresentMode;
                }
            }

            engine::LogWarn("VulkanSwapchain::selectSwapPresentMode, MAILBOX unsupported. Falling back to FIFO.");
            return config::FallBackSwapchainPresentMode;
        }

        VkExtent2D VulkanSwapchain::selectSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, uint32_t width, uint32_t height)
        {
            if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) 
            {
                return capabilities.currentExtent;
            } 
            else 
            {
                VkExtent2D actualExtent = { width, height };
                actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
                actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

                return actualExtent;
            }
        }

        uint32_t VulkanSwapchain::selectImageCount(const VkSurfaceCapabilitiesKHR &capabilities)
        {
            uint32_t imageCount = capabilities.minImageCount + 1;

            //0 : Means there is no maximum image count
            if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) 
                imageCount = capabilities.maxImageCount;

            engine::LogTrace(std::format( "VulkanSwapchain::selectImageCount, Selected {} swapchain images.", imageCount));
            return imageCount;
        }

        void VulkanSwapchain::destroy()
        {
            if(m_physicalDevice != VK_NULL_HANDLE && m_logicalDevice != VK_NULL_HANDLE && m_surface != VK_NULL_HANDLE)
            {
                engine::LogInfo("VulkanSwapchain::destroy, Destroying swapchain.");
                vkDestroySwapchainKHR(m_logicalDevice, m_swapchain, nullptr);

                m_physicalDevice = VK_NULL_HANDLE;
                m_logicalDevice = VK_NULL_HANDLE;
                m_surface = VK_NULL_HANDLE;
            }
        }
    }
}
