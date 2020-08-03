#pragma once

namespace GraphicsEngine {
    enum class ShadingModel { Phong, BlinnPhong };

    struct RenderSettings {
        // static flags at load time
        ShadingModel shading_model {ShadingModel::BlinnPhong};

        bool normal_mapping {false};
        bool physically_based_rendering {false};

        // dynamic flags
        bool light_radius {true};
    };

    inline RenderSettings render_settings;
}