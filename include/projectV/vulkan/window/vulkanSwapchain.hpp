#pragma once

#include <vulkan/vulkan.h>

#include <vulkan/device/vulkanPhysicalDevice.hpp>
#include <vulkan/window/VulkanImageView.hpp>

#include <span>
#include <vector>
namespace projectv
{
    namespace vulkan::sync 
    {
        class VulkanFence;
        class VulkanSemaphore; 
    }

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

            /**
             * @brief Acquires the next presentable swapchain image.
             *
             * Returns as soon as an image index is known, but the image may
             * still be in use by the presentation engine. @p semaphore is
             * signaled once the image is actually ready, so GPU work that
             * writes to it must wait on @p semaphore.
             *
             * Waits with no timeout.
             *
             * @param semaphore Unsignaled semaphore, signaled when the image
             *                  is ready to be rendered to.
             * @param outImageIndex Receives the index of the acquired image.
             *                      Only valid if the result is VK_SUCCESS or
             *                      VK_SUBOPTIMAL_KHR.
             * @return VK_SUCCESS if an image was acquired.
             *         VK_SUBOPTIMAL_KHR if an image was acquired but the
             *         swapchain no longer matches the surface exactly; the
             *         image is usable, but the swapchain should be recreated.
             *         VK_ERROR_OUT_OF_DATE_KHR if no image was acquired and
             *         the swapchain must be recreated before rendering.
             * @throws std::runtime_error For any other acquisition failure.
             */
            [[nodiscard]]
            VkResult acquireNextImage(const sync::VulkanSemaphore& semaphore, uint32_t& outImageIndex);

            /**
             * @brief Acquires the next presentable swapchain image, also
             *        signaling a fence.
             *
             * Same as acquireNextImage(const sync::VulkanSemaphore&, uint32_t&),
             * except @p fence is also signaled once the image is ready. Use
             * the fence when the CPU needs to know the image is free, for
             * example before writing to it from the host.
             *
             * @param semaphore Unsignaled semaphore, signaled when the image
             *                  is ready to be rendered to.
             * @param fence Unsignaled fence, signaled when the image is ready
             *              to be rendered to.
             * @param outImageIndex Receives the index of the acquired image.
             *                      Only valid if the result is VK_SUCCESS or
             *                      VK_SUBOPTIMAL_KHR.
             * @return VK_SUCCESS, VK_SUBOPTIMAL_KHR or VK_ERROR_OUT_OF_DATE_KHR,
             *         with the same meaning as the semaphore-only overload.
             * @throws std::runtime_error For any other acquisition failure.
             */
            [[nodiscard]]
            VkResult acquireNextImage(const sync::VulkanSemaphore& semaphore, const sync::VulkanFence& fence ,uint32_t& outImageIndex);
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
             * @brief Creates image views for all swapchain images.
             */
            void createImageViews();
            
            /**
            * @brief Destroys the Vulkan swapchain.
            * 
             */
            void destroy();

            /**
             * @brief Shared implementation of both acquireNextImage overloads.
             *
             * Calls vkAcquireNextImageKHR with no timeout and passes
             * VK_ERROR_OUT_OF_DATE_KHR and VK_SUBOPTIMAL_KHR back to the caller
             * instead of treating them as errors.
             *
             * @param semaphore Semaphore to signal, or VK_NULL_HANDLE.
             * @param fence Fence to signal, or VK_NULL_HANDLE. At least one of
             *              @p semaphore and @p fence must be a valid handle.
             * @param outImageIndex Receives the index of the acquired image.
             * @return Result of vkAcquireNextImageKHR.
             * @throws std::runtime_error For any result other than VK_SUCCESS,
             *         VK_SUBOPTIMAL_KHR or VK_ERROR_OUT_OF_DATE_KHR.
             */
            [[nodiscard]]
            VkResult acquireNextImageInternal(VkSemaphore semaphore, VkFence fence, uint32_t& outImageIndex);
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