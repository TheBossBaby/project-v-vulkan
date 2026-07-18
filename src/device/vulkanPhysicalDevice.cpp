#include <projectV/engine/logging/log.hpp>

#include <vulkan/util/vulkanCheck.hpp>


#include <vulkanPhysicalDevice.hpp>

#include <format>
#include <unordered_set>

namespace projectv
{
    namespace vulkan::device
    {
        VulkanPhysicalDevice::VulkanPhysicalDevice()
        {
        }
        
        VulkanPhysicalDevice::~VulkanPhysicalDevice()
        {
        }

        void VulkanPhysicalDevice::select(VkInstance instance, VkSurfaceKHR surface, const std::vector<const char*>& requiredDeviceExtensionList)
        {
            uint32_t deviceCount = 0;
            vkCheck(
                vkEnumeratePhysicalDevices(
                    instance,
                    &deviceCount,
                    nullptr),
                "VulkanPhysicalDevice::select, Failed to enumerate physical devices");

            if (deviceCount == 0)
            {
                throw std::runtime_error(
                    "VulkanPhysicalDevice::select, No Vulkan capable GPU found.");
            }
            
            std::vector<VkPhysicalDevice> devices(deviceCount);
            vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

            for (const auto& device : devices)
            {
                if (isDeviceSuitable(device, surface, requiredDeviceExtensionList))
                {
                    physicalDevice = device;
                    break;
                }
            }

            if (physicalDevice == VK_NULL_HANDLE) throw std::runtime_error("VulkanPhysicalDevice::select, failed to find a suitable GPU!");
            
        }

        VkPhysicalDevice VulkanPhysicalDevice::handle() const noexcept
        {
            return physicalDevice;
        }

        bool VulkanPhysicalDevice::isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface, const std::vector<const char*>& requiredDeviceExtensionList)
        {
            queueFamiliesIndex = findQueueFamilies(device, surface);
            if(!queueFamiliesIndex.isComplete()) 
            {
                engine::LogError("VulkanPhysicalDevice::isDeviceSuitable - No valid queue family found.");
                return false;
            }

            if(!supportsDeviceExtensions(device, requiredDeviceExtensionList))
            {
                engine::LogError("VulkanPhysicalDevice::isDeviceSuitable - Required Device Extensions are not supported.");
                return false;
            }

            engine::LogInfo(std::format("VulkanPhysicalDevice::isDeviceSuitable, graphics queue : {}, presentation queue : {}", queueFamiliesIndex.graphics.value(), queueFamiliesIndex.presentation.value()));
    
            VkPhysicalDeviceProperties deviceProperties;
            VkPhysicalDeviceFeatures deviceFeatures;
            
            vkGetPhysicalDeviceProperties(device, &deviceProperties);
            vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
            
            engine::LogInfo(std::format("VulkanPhysicalDevice::isDeviceSuitable, Selected GPU: {}", deviceProperties.deviceName));

            return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
           deviceFeatures.geometryShader;
        }

        vulkan::types::QueueFamilies VulkanPhysicalDevice::findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
        {   
            vulkan::types::QueueFamilies families;
            uint32_t queueFamilyCount = 0;
            VkBool32 presentationSupport = false;

            vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
            
            std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

            uint32_t queueFamilyIndex = 0;
            for (const auto& queueFamily : queueFamilies) 
            {
                vkCheck(
                    vkGetPhysicalDeviceSurfaceSupportKHR(
                        device, 
                        queueFamilyIndex , 
                        surface, 
                        &presentationSupport),
                    "VulkanPhysicalDevice::select, Failed to query queue family presentation support!");

                if(presentationSupport)
                {
                    families.presentation = queueFamilyIndex ; //In some physical devices Graphics Queue and Presentation Queue could be same.
                }

                if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
                {
                    families.graphics = queueFamilyIndex ;
                }

                if (families.isComplete())
                {
                    engine::LogInfo("VulkanPhysicalDevice::findQueueFamilies, Found required queue families.");
                    break;
                }

                queueFamilyIndex ++;
            }

            return families;
        }   

        bool VulkanPhysicalDevice::supportsDeviceExtensions(VkPhysicalDevice device, const std::vector<const char*>& deviceExtensions)
        {
            uint32_t extensionCount;
            vkCheck(
                vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr),
                "Failed to enumerate device extensions");

            std::vector<VkExtensionProperties> availableExtensions(extensionCount);
            vkCheck(
                vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data()),
                "Failed to enumerate device extensions");

            std::unordered_set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

            for (const auto& extension : availableExtensions) 
            {
                requiredExtensions.erase(extension.extensionName);
            }

            return requiredExtensions.empty();
        }
    }
}