#pragma once

#include <vulkan/vulkan.h>

#include <vulkan/device/vulkanPhysicalDevice.hpp>
#include <vulkan/window/VulkanImageView.hpp>

#include <span>
#include <vector>
namespace projectv
{
    namespace vulkan::window
    {
        /**
         * @brief Swapchain capabilities supported by a physical device.
         *
         * Contains the information required to create a swapchain for a
         * particular surface.
         */
        struct SwapChainSupportDetails
        {
            /// Surface capabilities and limits.
            VkSurfaceCapabilitiesKHR capabilities;

            /// Supported surface formats.
            std::vector<VkSurfaceFormatKHR> formats;

            /// Supported presentation modes.
            std::vector<VkPresentModeKHR> presentModes;
        };
        
        class VulkanSwapchain
        {
        public:
            VulkanSwapchain();

            ~VulkanSwapchain();
            
            /**
             * @brief Returns the underlying Vulkan swapchain handle.
             *
             * @return Wrapped VkSwapchainKHR handle.
             */
            [[nodiscard]]
            VkSwapchainKHR handle() const noexcept;

            /**
            * @brief Returns the underlying Vulkan format of swapchain.
            * 
            * @return Format of swapchain
             */
            [[nodiscard]]
            const VkSurfaceFormatKHR& format() const noexcept;

            /**
            * @brief Returns the extent of swapchain.
            * 
            * @return Extent of swapchain
             */
            [[nodiscard]]
            const VkExtent2D& extent() const noexcept;

            /**
            * @brief Get access to list of swapchain images.
            * 
            * @return Access to vkImages of swapchain list.
             */
            std::span<const VkImage> images() const noexcept;

            /**
             * @brief Returns the swapchain image views.
             *
             * Provides read-only access to the image views created for each
             * swapchain image.
             *
             * @return Read-only span of swapchain image views.
             */
            [[nodiscard]]
            std::span<const VulkanImageView> imageViews() const noexcept;

            /**
             * @brief Creates the Vulkan swapchain.
             *
             * Queries swapchain support, selects the preferred configuration,
             * and creates the underlying VkSwapchainKHR object.
             *
             * @param physicalDevice Selected physical device.
             * @param logicalDevice Logical device.
             * @param surface Window presentation surface.
             */
            void create(const device::VulkanPhysicalDevice& physicalDevice, VkDevice  logicalDevice, VkSurfaceKHR surface, uint32_t width, uint32_t height);
        private:
            /**
             * @brief Verifies that swapchain creation is supported.
             *
             * @return True if the selected device supports a valid swapchain.
             *
             * @throws std::runtime_error if no compatible formats or presentation
             * modes are available.
             */
            bool check();

            /**
             * @brief Queries swapchain support for a physical device.
             *
             * @param physicalDevice Physical device to query.
             * @param surface Presentation surface.
             *
             * @return Supported swapchain capabilities.
             */
            SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface); //This method could be kept in Physical Device or a Util file

            /**
            * @brief Selects the preferred surface format.
            * 
            * @param availableFormats List of available Formats
            * @return
             */
            VkSurfaceFormatKHR selectSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

            /**
            * @brief Selects the preferred presentation mode.
            * 
            * @param availablePresentModes List of available presentation mode.
            * @return 
             */
            VkPresentModeKHR selectSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

            /**
            * @brief Selects the swapchain image extent.
            * 
            * @param capabilities Capabilities of swapchain which the physical device and surface has.
            * @param width Width of swapchain
            * @param height Height of swapchain
            * @return Extent of swapchain
             */
            VkExtent2D selectSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height);

            /**
            * @brief Selects the number of swapchain images.
            * 
            * @param capabilities Capabilities of swapchain which the physical device and surface has.
            * @return Number of images in swapchain.
             */
            uint32_t selectImageCount(const VkSurfaceCapabilitiesKHR& capabilities);

            /**
            * @brief Retrive swapchain images
            * 
             */
            void retrieveImages();

            /**
            * @brief Create a Image Views object
            * 
             */
            void createImageViews();
            
            /**
            * @brief Destroys the Vulkan swapchain.
            * 
             */
            void destroy();

        private:
            VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
            
            VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;

            VkDevice m_logicalDevice = VK_NULL_HANDLE;

            VkSurfaceKHR m_surface = VK_NULL_HANDLE;

            VkSurfaceFormatKHR m_format = {};

            VkExtent2D m_extent = {};

            std::vector<VkImage> m_images;

            std::vector<VulkanImageView> m_imageViews;
        };
    }
}