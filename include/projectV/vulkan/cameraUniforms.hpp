#pragma once

#include <projectV/core/math/math.hpp>

namespace projectv
{
    namespace vulkan
    {
        struct CameraUniforms
        {
            core::math::Matrix4 viewProj;
            core::math::Matrix4 invViewProj;
            core::math::Vector3 cameraPos;
        };
    }
}
