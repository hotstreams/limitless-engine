#pragma once

#include <limitless/pipeline/shader_pass_types.hpp>
#include <glm/vec2.hpp>

namespace Limitless {
    enum class RenderPipeline {
        Forward,
        Deferred
    };

    class RenderSettings {
    public:
        RenderPipeline pipeline = RenderPipeline::Deferred;

        bool normal_mapping = true;

        bool screen_space_ambient_occlusion = false;
        bool fast_approximate_antialiasing = true;
        bool depth_of_field = false;

        bool directional_cascade_shadow_mapping = true;
        glm::uvec2 directional_shadow_resolution = { 1024 * 4, 1024 * 4 };
        uint8_t directional_split_count = 3; // [2; 4]
        bool directional_pcf = true;

        bool micro_shadowing = false;

        // debug
        bool light_radius = true;
        bool coordinate_system_axes = false;
        bool bounding_box = false;
    };
}
