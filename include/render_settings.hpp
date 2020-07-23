#pragma once

namespace GraphicsEngine {
    enum class ShadingModel { Phong, BlinnPhong, CelShading };

    struct RenderSettings {
        ShadingModel shading_model {ShadingModel::BlinnPhong};

        bool normal_mapping {false};
        bool physically_based_rendering {false};
    };

    inline RenderSettings render_settings;
}