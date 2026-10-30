#pragma once

#include <vulkan/vulkan.h>

#include <vulkan/types/queueFamilies.hpp>

#include <vector>
namespace projectv
{
    
    namespace vulkan::device
    {
        class VulkanPhysicalDevice
        {
        public:
            /**
            * @brief Construct a new Vulkan Physical Device object
            * 
             */
            VulkanPhysicalDevice();

            /**
            * @brief Destroy the Vulkan Physical Device object
            * 
             */
            ~VulkanPhysicalDevice();

           /**
           * @brief 
           * 
           * @param instance 
           * @param surface 
           * @param requiredDeviceExtensionList 
            */
            void select(VkInstance instance, VkSurfaceKHR surface, const std::vector<const char*>& requiredDeviceExtensionList);

            /**
             * @brief Returns the first candidate format that the selected GPU
             *        supports with the given tiling and features.
             *
             * Candidates are checked in order, so list them from most to least
             * preferred. A typical use is picking a depth format, e.g.
             * { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
             * VK_FORMAT_D24_UNORM_S8_UINT } with
             * VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT.
             *
             * Only VK_IMAGE_TILING_LINEAR and VK_IMAGE_TILING_OPTIMAL are
             * checked; any other tiling never matches.
             *
             * @param candidates Formats to test, in order of preference.
             * @param tiling Image tiling the format will be used with.
             * @param features Format features that must all be supported for
             *                 @p tiling.
             * @return The first format in @p candidates that supports every bit
             *         in @p features.
             * @throws std::runtime_error If no physical device is selected yet,
             *         i.e. select() has not been called.
             * @throws std::runtime_error If no candidate is supported.
             */
            VkFormat findSupportedFormat( const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;

            /**
            * @brief 
            * 
            * @return 
             */
            VkPhysicalDevice handle() const noexcept;
            
            const vulkan::types::QueueFamilies& queueFamilies() const { return queueFamiliesIndex; }
        private:
            /**
            * @brief 
            * 
            * @param device 
            * @return 
            * @return 
            */
            bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface, const std::vector<const char*>& requiredDeviceExtensionList);

            /**
            * @brief 
            * 
            * @param device 
            * @return 
            */
            vulkan::types::QueueFamilies findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
            
            /**
            * @brief Function to check if physical device supports required Device Extensions or not.
            * 
            * @param device Handle to physical device
            * @return True if all required devices extensions are supported else false.
             */
            bool supportsDeviceExtensions(VkPhysicalDevice device, const std::vector<const char*>& deviceExtensions);
        private:
            /**
            * @brief 
            * 
            */
            VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
            
            /**
            * @brief 
            * 
            */
            vulkan::types::QueueFamilies queueFamiliesIndex;
    };        
    }
}