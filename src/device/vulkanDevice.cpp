#include <projectV/engine/logging/log.hpp>

#include <vulkan/types/queueFamilies.hpp>
#include <vulkan/util/vulkanCheck.hpp>

#include <vulkanDevice.hpp>

#include <cassert>
#include <format>

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
            constexpr uint32_t QUEUE_INFO_COUNT = 1;
            constexpr float QUEUE_PRIORITY = 1.0f;

            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamilies.graphics.value();
            queueCreateInfo.queueCount = QUEUE_COUNT;
            queueCreateInfo.pQueuePriorities = &QUEUE_PRIORITY;

            VkPhysicalDeviceFeatures deviceFeatures{};

            VkDeviceCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            createInfo.pQueueCreateInfos = &queueCreateInfo;
            createInfo.queueCreateInfoCount = QUEUE_INFO_COUNT;

            createInfo.pEnabledFeatures = &deviceFeatures;
            createInfo.enabledExtensionCount = 0;
            createInfo.enabledLayerCount = 0;

            vkCheck(vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice), 
                "VulkanDevice::create, failed to create logical device!");

            acquireGraphicsQueue(queueFamilies);    
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

        void VulkanDevice::acquireGraphicsQueue(const vulkan::types::QueueFamilies& queueFamilies)
        {
            assert(queueFamilies.isComplete());

            constexpr uint32_t QUEUE_INDEX = 0;
            VkQueue queueHandle  = VK_NULL_HANDLE;

            vkGetDeviceQueue(logicalDevice, queueFamilies.graphics.value(), QUEUE_INDEX, &queueHandle );

            m_graphicsQueue.create(queueHandle );
            engine::LogInfo(std::format( "VulkanDevice::acquireGraphicsQueue m_graphicsQueue Handle : {}", 
                static_cast<const void*>(m_graphicsQueue.handle())));
        }
    }
}