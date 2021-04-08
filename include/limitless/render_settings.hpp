#pragma once

#include <glm/vec2.hpp>

namespace LimitlessEngine {
    enum class ShadingModel { Phong, BlinnPhong };

    struct RenderSettings {
        // material settings
        static constexpr auto SHADING_MODEL {ShadingModel::BlinnPhong};
        static constexpr auto PHYSICALLY_BASED_RENDER {true};
        static constexpr auto NORMAL_MAPPING {true};

        // shadows settings
        static constexpr auto DIRECTIONAL_CSM {true};
        static constexpr auto DIRECTIONAL_SHADOW_RESOLUTION = glm::uvec2{ 1024, 1024 };
        static constexpr auto DIRECTIONAL_SPLIT_COUNT {4}; // four is max

        static constexpr auto DIRECTIONAL_PFC {true};

        // king of debug settings
        static constexpr auto LIGHT_RADIUS {false};
        static constexpr auto COORDINATE_SYSTEM_AXES {true};
    };
}
