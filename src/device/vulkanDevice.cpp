#include <projectV/engine/logging/log.hpp>

#include <vulkan/types/queueFamilies.hpp>
#include <vulkan/util/vulkanCheck.hpp>

#include <vulkanDevice.hpp>

#include <cassert>
#include <format>
#include <unordered_set>
#include <vector>

namespace projectv
{
    namespace vulkan::device
    {
        VulkanDevice::VulkanDevice()
        {
        }
        
        VulkanDevice::~VulkanDevice()
        {
            destroy();
        }

        void VulkanDevice::create(VkPhysicalDevice physicalDevice, const vulkan::types::QueueFamilies& queueFamilies)
        {
            assert(queueFamilies.isComplete());

            constexpr uint32_t QUEUE_COUNT = 1;
            constexpr float QUEUE_PRIORITY = 1.0f;

            std::vector<VkDeviceQueueCreateInfo> queueCreateInfoList;
            std::unordered_set<uint32_t> uniqueQueueFamilies = {queueFamilies.graphics.value(), queueFamilies.presentation.value()};

            for (uint32_t queueFamily : uniqueQueueFamilies) 
            {
                VkDeviceQueueCreateInfo queueCreateInfo{};
                queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queueCreateInfo.queueFamilyIndex = queueFamily;
                queueCreateInfo.queueCount = QUEUE_COUNT;
                queueCreateInfo.pQueuePriorities = &QUEUE_PRIORITY;
                queueCreateInfoList.push_back(queueCreateInfo);
            }

            VkPhysicalDeviceFeatures deviceFeatures{};

            VkDeviceCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            createInfo.pQueueCreateInfos = queueCreateInfoList.data();
            createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfoList.size());

            createInfo.pEnabledFeatures = &deviceFeatures;
            createInfo.enabledExtensionCount = 0;
            createInfo.enabledLayerCount = 0;

            vkCheck(vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice), 
                "VulkanDevice::create, failed to create logical device!");

            acquireQueue(queueFamilies.graphics.value(), m_graphicsQueue);    
            acquireQueue(queueFamilies.presentation.value(), m_presentQueue);    
        }

        void VulkanDevice::destroy()
        {
            if (logicalDevice != VK_NULL_HANDLE)
            {
                vkDestroyDevice(logicalDevice, nullptr);
                logicalDevice = VK_NULL_HANDLE;
                engine::LogInfo("VulkanDevice::destroy, Logical device destroyed");
            }
        }

        VkDevice VulkanDevice::handle() const noexcept
        {
            return logicalDevice;
        }

        VulkanQueue &VulkanDevice::graphicsQueue()
        {
            return m_graphicsQueue;
        }

        VulkanQueue &VulkanDevice::presentQueue()
        {
            return m_presentQueue;
        }

        void VulkanDevice::acquireQueue(const uint32_t queueFamilyIndex, VulkanQueue& vulkanQueue)
        {
            constexpr uint32_t QUEUE_INDEX = 0;
            VkQueue queueHandle  = VK_NULL_HANDLE;

            vkGetDeviceQueue(logicalDevice, queueFamilyIndex, QUEUE_INDEX, &queueHandle );
            vulkanQueue.create(queueHandle);

            engine::LogInfo(std::format( "VulkanDevice::acquireQueue Queue Family {} Handle : {}",
                queueFamilyIndex,
                static_cast<const void*>(vulkanQueue.handle())));
        }
    }
}