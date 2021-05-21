#pragma once

#include <limitless/pipeline/shader_pass_types.hpp>
#include <glm/vec2.hpp>

namespace Limitless {
    enum class ShadingModel {
        Phong,
        BlinnPhong
    };

    class RenderSettings {
    public:
        // compile time for now
        // forward / deferred / forward+
        static constexpr auto renderer = ShaderPass::Forward;

        // material settings
        ShadingModel shading_model = ShadingModel::BlinnPhong;
        bool physically_based_render = true;
        bool normal_mapping = true;

        // lighting settings
        // static constexpr auto MAX_POINT_LIGHTS_INFLUENCE {-1}; // -1 for unlimited
        // static constexpr auto HIGH_DYNAMIC_RANGE {true};

        // shadows settings
        bool directional_csm = true;
        glm::uvec2 directional_shadow_resolution = { 1024 * 4, 1024 * 4 };
        uint8_t directional_split_count = 3; // four is max
        bool directional_pcf = true;

        // king of debug settings
        bool light_radius = false;
        bool coordinate_system_axes = false;
        bool bounding_box = false;
    };
}
