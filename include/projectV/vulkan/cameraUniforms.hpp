#pragma once

#include <projectV/core/math/math.hpp>

#include <cstddef>

namespace projectv
{
    namespace vulkan
    {
        struct CameraUniforms
        {
            core::math::Matrix4 viewProj;
            core::math::Matrix4 invViewProj;
            core::math::Vector4 cameraPos;
        };

        static_assert(sizeof(CameraUniforms) == 144, "CameraUniforms must match the std140 layout in the shader");
        static_assert(offsetof(CameraUniforms, cameraPos) == 128, "cameraPos must sit at std140 offset 128");
    }
}
