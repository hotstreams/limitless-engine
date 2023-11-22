#pragma once

#include <string>

namespace Limitless {
    class RenderSettings;
    class Shader;

    class RenderSettingsShaderDefiner {
    public:
        static std::string getDefine(const RenderSettings& settings);
    };
}