#include <projectV/engine/logging/log.hpp>

#include <vulkan/builder/vulkanShaderModuleBuilder.hpp>

#include <cstdint>
#include <stdexcept>

namespace projectv
{
    namespace vulkan::builder
    {
        VulkanShaderModuleBuilder& VulkanShaderModuleBuilder::setCode(std::span<const std::uint8_t> code)
        {
            m_code = code;
            return *this;
        }

        VulkanShaderModuleBuilder& VulkanShaderModuleBuilder::setFlags(VkShaderModuleCreateFlags flags)
        {
            m_flags = flags;
            return *this;
        }

        rendererResources::VulkanShaderModule VulkanShaderModuleBuilder::build(VkDevice device)
        {
            if (m_code.empty())
            {
                engine::LogError("VulkanShaderModuleBuilder::build, no shader code set");
                throw std::runtime_error("VulkanShaderModuleBuilder::build, no shader code set");
            }

            if (m_code.size() % sizeof(std::uint32_t) != 0)
            {
                engine::LogError("VulkanShaderModuleBuilder::build, shader code size must be aligned to 32-bit SPIR-V words");
                throw std::runtime_error("VulkanShaderModuleBuilder::build, shader code size must be aligned to 32-bit SPIR-V words");
            }

            const auto address =
                reinterpret_cast<std::uintptr_t>(m_code.data());

            if (address % alignof(std::uint32_t) != 0)
            {
                engine::LogError(
                    "VulkanShaderModuleBuilder::build, shader code is not properly aligned");

                throw std::runtime_error(
                    "VulkanShaderModuleBuilder::build, shader code is not properly aligned");
            }

            VkShaderModuleCreateInfo createInfo{};
            createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.flags    = m_flags;
            createInfo.codeSize = m_code.size();
            createInfo.pCode    = reinterpret_cast<const std::uint32_t*>(m_code.data());

            return rendererResources::VulkanShaderModule(device, createInfo);
        }
    }
}
