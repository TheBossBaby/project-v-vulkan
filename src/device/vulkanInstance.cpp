#include <windowExtension.hpp>
#include <vulkanCheck.hpp>
#include <projectV/vulkan/config/vulkanConfig.hpp>
#include <vulkanInstance.hpp>

#include <memory>

namespace projectv
{
    namespace vulkan::device
    {
        VulkanInstance::VulkanInstance()
        {
        }
        
        VulkanInstance::~VulkanInstance()
        {
            destroy();
        }

        void VulkanInstance::create(const IWindowExtension& windowExtension)
        {
            auto extension = windowExtension->Extensions();

            VkApplicationInfo appInfo{};
            appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            appInfo.pApplicationName = "Project V";
            appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
            appInfo.pEngineName = "Project V";
            appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
            appInfo.apiVersion = config::VulkanVersion;

            VkInstanceCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            createInfo.pApplicationInfo = &appInfo;
            createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
            createInfo.ppEnabledExtensionNames = extensions.data();
            createInfo.enabledLayerCount = 0;

            vkCheck(vkCreateInstance(&createInfo, nullptr, &instance), 
            "Failed to create Vulkan Instance")
        }

        void VulkanInstance::destroy()
        {
            if (instance != VK_NULL_HANDLE)
            {
                vkDestroyInstance(instance, nullptr);
                instance = VK_NULL_HANDLE;
            }
        }

        VkInstance VulkanInstance::handle() const noexcept
        {
            return instance;
        }
    }
}
