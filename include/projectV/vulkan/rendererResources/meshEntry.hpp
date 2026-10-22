#pragma once

#include <vulkan/rendererResources/vulkanBuffer.hpp>

namespace projectv
{
    namespace vulkan
    {
        /**
        * @brief GPU-side buffers for a single mesh, ready to bind and draw.
        *
        * Move-only, since it owns VulkanBuffer instances.
        */
        struct MeshEntry
        {
            /// Buffer holding the mesh's vertex data.
            rendererResources::VulkanBuffer vertexBuffer;

            /// Buffer holding the mesh's index data.
            rendererResources::VulkanBuffer indexBuffer;

            /// Number of indices in indexBuffer, as passed to vkCmdDrawIndexed.
            std::uint32_t                   indexCount = 0;
        };
    }
}
