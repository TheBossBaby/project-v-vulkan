#pragma once

#include <projectV/core/renderer.hpp>

#include <memory>

namespace projectv::vulkan
{
    /**
     * @brief Creates the Vulkan implementation of core::IRenderer.
     *
     * The only entry point the game needs from this library; callers see
     * just the core::IRenderer interface, never Vulkan types.
     *
     * @return A new, uninitialised renderer. Call init() before use.
     */
    std::unique_ptr<core::IRenderer> createRenderer();
}
