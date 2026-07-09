#pragma once

#include <vulkan/command/vulkanCommandPool.hpp>

#include <vulkan/device/vulkanDevice.hpp>
#include <vulkan/device/vulkanInstance.hpp>
#include <vulkan/device/vulkanPhysicalDevice.hpp>

#include <vulkan/synchronization/VulkanFence.hpp>

#include <vulkan/util/glfwWindowExtension.hpp>

#include <memory>

namespace projectv
{
    namespace vulkan
    {
        class VulkanFactory
        {
            public:
                VulkanFactory()
                {
                }

                static std::unique_ptr<device::VulkanInstance> createInstance()
                {
                    return std::make_unique<device::VulkanInstance>();
                }

                static std::unique_ptr<util::IWindowExtension> createWindowExtension()
                {
                    return std::make_unique<util::GlfwWindowExtension>();
                }

                static std::unique_ptr<device::VulkanPhysicalDevice> createPhysicalDevice()
                {
                    return std::make_unique<device::VulkanPhysicalDevice>();
                }

                static std::unique_ptr<device::VulkanDevice> createLogicalDevice()
                {
                    return std::make_unique<device::VulkanDevice>();
                }

                static std::unique_ptr<command::VulkanCommandPool> createCommandPool()
                {
                    return std::make_unique<command::VulkanCommandPool>();
                }

                static std::unique_ptr<sync::VulkanFence> createFence()
                {
                    return std::make_unique<sync::VulkanFence>();
                }
        };
    }
}