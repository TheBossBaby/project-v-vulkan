#include <vulkan/util/vulkanCheck.hpp>
#include <vulkan/config/vulkanConfig.hpp>
#include <vulkan/util/vulkanDebugMessenger.hpp>
#include <vulkanInstance.hpp>

#include <string>
#include <memory>
#include <vector>

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

        void VulkanInstance::create(const vulkan::util::IWindowExtension& windowExtension, vulkan::util::VulkanValidationLayerManager& validationLayerManager)
        {
            auto windowExtensions = windowExtension.Extensions();
            std::vector<const char*> extensions(windowExtensions.begin(), windowExtensions.end());

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

            // Must outlive vkCreateInstance, since it is chained via pNext.
            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

            m_validationEnabled = config::EnableValidationLayers &&
                validationLayerManager.enableValidationLayer(config::ValidationLayers);

            if(m_validationEnabled)
            {
                createInfo.enabledLayerCount = static_cast<uint32_t>(config::ValidationLayers.size());
                createInfo.ppEnabledLayerNames = config::ValidationLayers.data();

                extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

                // Catches messages from vkCreateInstance/vkDestroyInstance themselves.
                util::VulkanDebugMessenger::populateCreateInfo(debugCreateInfo);
                createInfo.pNext = &debugCreateInfo;
            }
            else
                createInfo.enabledLayerCount = 0;

            createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
            createInfo.ppEnabledExtensionNames = extensions.data();

            vkCheck(vkCreateInstance(&createInfo, nullptr, &instance), 
            "Failed to create Vulkan Instance");
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

        bool VulkanInstance::validationEnabled() const noexcept
        {
            return m_validationEnabled;
        }
    }
}
