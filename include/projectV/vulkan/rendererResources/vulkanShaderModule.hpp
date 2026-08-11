#pragma once

#include <vulkan/vulkan.h>

namespace projectv
{
    namespace vulkan
    {
        namespace rendererResources
        {
            /**
             * @brief RAII wrapper around a VkShaderModule.
             *
             * Owns a VkShaderModule handle and the VkDevice it was created from,
             * and guarantees the shader module is destroyed via vkDestroyShaderModule
             * when the wrapper goes out of scope. Non-copyable, move-only.
             */
            class VulkanShaderModule
            {
            public:
                /**
                 * @brief Constructs an empty wrapper with no owned shader module.
                 *
                 * Useful for deferred initialization; the object is invalid
                 * (operator bool() returns false) until move-assigned from
                 * a valid instance.
                 */
                VulkanShaderModule() = default;

                /**
                 * @brief Creates a VkShaderModule and takes ownership of it.
                 *
                 * @param device     Logical device used to create the shader module.
                 *                   Stored internally and reused for destruction.
                 * @param createInfo Shader module creation parameters, as passed to
                 *                   vkCreateShaderModule. pCode is expected to point
                 *                   at SPIR-V bytecode, typically sourced from
                 *                   projectv::core::Shader::data.
                 *
                 * @throws std::runtime_error if vkCreateShaderModule fails.
                 */
                VulkanShaderModule(VkDevice device, const VkShaderModuleCreateInfo& createInfo);

                /**
                 * @brief Destroys the owned shader module, if any.
                 */
                ~VulkanShaderModule();

                /// Non-copyable: a VkShaderModule has single-owner destruction semantics.
                VulkanShaderModule(const VulkanShaderModule&) = delete;

                /// Non-copyable: a VkShaderModule has single-owner destruction semantics.
                VulkanShaderModule& operator=(const VulkanShaderModule&) = delete;

                /**
                 * @brief Move-constructs, transferring ownership from another wrapper.
                 * @param other Source wrapper; left in an empty, valid-to-destroy state.
                 */
                VulkanShaderModule(VulkanShaderModule&& other) noexcept;

                /**
                 * @brief Move-assigns, destroying any currently owned shader module
                 *        and transferring ownership from another wrapper.
                 * @param other Source wrapper; left in an empty, valid-to-destroy state.
                 * @return Reference to this object.
                 */
                VulkanShaderModule& operator=(VulkanShaderModule&& other) noexcept;

                /**
                 * @brief Returns the underlying VkShaderModule handle.
                 * @return The owned handle, or VK_NULL_HANDLE if none is owned.
                 */
                VkShaderModule handle() const noexcept;

                /**
                 * @brief Checks whether this wrapper owns a valid shader module.
                 * @return true if a shader module is owned, false otherwise.
                 */
                explicit operator bool() const noexcept;

            private:
                /**
                 * @brief Destroys the owned shader module and resets internal state
                 *        to VK_NULL_HANDLE. Safe to call multiple times.
                 */
                void destroy() noexcept;

                /// Logical device used to create/destroy m_shaderModule.
                VkDevice       m_logicalDevice = VK_NULL_HANDLE;

                /// Owned shader module handle.
                VkShaderModule m_shaderModule  = VK_NULL_HANDLE;
            };
        }
    }
}
