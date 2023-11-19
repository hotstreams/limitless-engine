#pragma once

namespace Limitless {
    class RenderSettings;
    class Shader;

    class RenderSettingsShaderDefiner {
    private:
        const RenderSettings& settings;
    public:
        explicit RenderSettingsShaderDefiner(const RenderSettings& settings) noexcept;

        void define(Shader& shader);
    };
}