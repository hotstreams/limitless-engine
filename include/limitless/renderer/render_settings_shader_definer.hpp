#pragma once

#include <string>

namespace Limitless {
    class RendererSettings;
    class Shader;

    class RenderSettingsShaderDefiner {
    public:
        static std::string getDefine(const RendererSettings& settings);
    };
}