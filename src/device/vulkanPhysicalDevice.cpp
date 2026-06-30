#include <vulkan/util/vulkanCheck.hpp>
#include <vulkanPhysicalDevice.hpp>

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

        VkPhysicalDevice VulkanPhysicalDevice::handle() const noexcept
        {
            return physicalDevice;
        }
        
        bool VulkanPhysicalDevice::isDeviceSuitable(VkPhysicalDevice device)
        {
            VkPhysicalDeviceProperties deviceProperties;
            VkPhysicalDeviceFeatures deviceFeatures;
            
            vkGetPhysicalDeviceProperties(device, &deviceProperties);
            vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

            return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
           deviceFeatures.geometryShader;
        }
    }
}