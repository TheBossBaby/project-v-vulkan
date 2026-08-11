#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/rendererResources/vulkanShaderModule.hpp>

#include <cstdint>
#include <span>

namespace projectv
{
    namespace vulkan::builder
    {
        /**
         * @brief Builder for assembling and creating a VkShaderModule.
         *
         * Accumulates SPIR-V bytecode through a fluent API, then builds a
         * fully-formed VulkanShaderModule via build(). The bytecode is
         * typically sourced from projectv::core::Shader::data, as populated
         * by an IShaderLoader implementation.
         */
        class VulkanShaderModuleBuilder
        {
        public:
            /**
             * @brief Sets the raw SPIR-V bytecode to compile the module from.
             *
             * The builder only borrows @p code; it does not copy it. The
             * referenced bytes must remain valid until build() returns, since
             * build() reads them to populate VkShaderModuleCreateInfo::pCode.
             *
             * @param code SPIR-V bytecode, as bytes. Size must be a multiple of 4.
             * @return Reference to this builder, for chaining.
             */
            VulkanShaderModuleBuilder& setCode(std::span<const std::uint8_t> code);

            /**
             * @brief Sets additional shader module creation flags.
             * @param flags VkShaderModuleCreateFlags bitmask.
             * @return Reference to this builder, for chaining.
             */
            VulkanShaderModuleBuilder& setFlags(VkShaderModuleCreateFlags flags);

            /**
             * @brief Assembles the configured state into a
             *        VkShaderModuleCreateInfo and creates the shader module.
             *
             * @param device Logical device to create the shader module on.
             * @return A VulkanShaderModule owning the newly created handle.
             *
             * @throws std::runtime_error if no code was set, the code size is
             *         not a multiple of 4, or shader module creation fails.
             */
            rendererResources::VulkanShaderModule build(VkDevice device);

        private:
            /// Non-owning view of the SPIR-V bytecode to build from.
            std::span<const std::uint8_t> m_code;

            /// Additional shader module creation flags.
            VkShaderModuleCreateFlags m_flags = 0;
        };
    }
}
