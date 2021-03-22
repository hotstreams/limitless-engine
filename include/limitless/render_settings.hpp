#pragma once

namespace LimitlessEngine {
    enum class ShadingModel { Phong, BlinnPhong };

    struct RenderSettings {
        // static flags at load time
        ShadingModel shading_model {ShadingModel::BlinnPhong};

        bool normal_mapping {true};
        bool physically_based_rendering {true};

        // dynamic flags
        bool light_radius {false};
    };

    inline RenderSettings render_settings;
}
