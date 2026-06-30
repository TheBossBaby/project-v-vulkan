#include <projectV/engine/logging/log.hpp>

#include <vulkan/util/vulkanCheck.hpp>
#include <vulkan/types/queueFamilies.hpp>

#include <vulkanPhysicalDevice.hpp>

#include <format>
#include <vector>
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

        void VulkanPhysicalDevice::select(VkInstance instance)
        {
            uint32_t deviceCount = 0;
            vkCheck(
                vkEnumeratePhysicalDevices(
                    instance,
                    &deviceCount,
                    nullptr),
                "Failed to enumerate physical devices");

            if (deviceCount == 0)
            {
                throw std::runtime_error(
                    "No Vulkan capable GPU found.");
            }
            
            std::vector<VkPhysicalDevice> devices(deviceCount);
            vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

            for (const auto& device : devices)
            {
                if (isDeviceSuitable(device))
                {
                    physicalDevice = device;
                    break;
                }
            }

            if (physicalDevice == VK_NULL_HANDLE) throw std::runtime_error("failed to find a suitable GPU!");
            
        }

        vulkan::types::QueueFamilies VulkanPhysicalDevice::findQueueFamilies(VkPhysicalDevice device)
        {   
            vulkan::types::QueueFamilies families;
            uint32_t queueFamilyCount = 0;

            vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
            
            std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

            int index = 0;
            for (const auto& queueFamily : queueFamilies) 
            {
                if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
                {
                    families.graphics = index;
                    break;
                }

                index++;
            }

            return families;
        }

        VkPhysicalDevice VulkanPhysicalDevice::handle() const noexcept
        {
            return physicalDevice;
        }
        
        bool VulkanPhysicalDevice::isDeviceSuitable(VkPhysicalDevice device)
        {
            vulkan::types::QueueFamilies queueFamilies = findQueueFamilies(device);
            if(!queueFamilies.isComplete()) 
            {
                engine::LogError("VulkanPhysicalDevice::isDeviceSuitable - No valid queue family found.");
                return false;
            }
    
            VkPhysicalDeviceProperties deviceProperties;
            VkPhysicalDeviceFeatures deviceFeatures;
            
            vkGetPhysicalDeviceProperties(device, &deviceProperties);
            vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
            
            engine::LogInfo(std::format("VulkanPhysicalDevice::isDeviceSuitable, Selected GPU: {}", deviceProperties.deviceName));

            return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
           deviceFeatures.geometryShader;
        }
    }
}