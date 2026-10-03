#include <vulkan/vulkan.h>
#include <projectV/engine/logging/log.hpp>

#include <vulkanValidationLayerManager.hpp>
#include <cstring>
#include <stdexcept>

namespace projectv
{
    namespace vulkan::util
    {
        VulkanValidationLayerManager::VulkanValidationLayerManager()
        {
        }

        bool VulkanValidationLayerManager::enableValidationLayer(const std::vector<const char*>& inValidationLayer)
        {
            if(!isSupported(inValidationLayer))
            {
                engine::LogError(
                    "VulkanValidationLayerManager, required validation layers are not supported");
                throw std::runtime_error(
                    "VulkanValidationLayerManager, required validation layers are not supported");
            }
            engine::LogInfo(
                    "VulkanValidationLayerManager, required validation layers are supported");
            return true;
        }

        bool VulkanValidationLayerManager::isSupported(const std::vector<const char*>& inValidationLayer)
        {
            uint32_t layerCount;
            vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

            std::vector<VkLayerProperties> availableLayers(layerCount);
            vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

            for (const char* layerName : inValidationLayer) 
            {
                bool layerFound = false;

                for (const auto& layerProperties : availableLayers) 
                {
                    if (strcmp(layerName, layerProperties.layerName) == 0) 
                    {
                        layerFound = true;
                        break;
                    }
                }

                if (!layerFound) 
                {
                    return false;
                }
            }

            return true;
        }
    }
}