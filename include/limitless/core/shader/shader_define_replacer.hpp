#pragma once

#include <unordered_map>
#include <string>
#include <optional>

namespace Limitless {
    class RendererSettings;
    class Shader;

    class ShaderDefineReplacer {
    protected:
        enum class Define {
            GLSLVersion,
            Extensions,
            Settings,
            // contains three def above
            Common,
            // material type
            MaterialDependent
        };

        static inline std::unordered_map<Define, std::string> DEFINE_NAMES =
        {
            { Define::GLSLVersion, "ENGINE::GLSLVERSION"},
            { Define::Extensions, "ENGINE::EXTENSIONS"},
            { Define::Settings, "ENGINE::SETTINGS"},
            { Define::Common, "ENGINE::COMMON"},
            { Define::MaterialDependent, "ENGINE::MATERIALDEPENDENT"},
        };

        static std::string getVersionDefine();
        static std::string getExtensionDefine();
        static std::string getSettingsDefine(std::optional<RendererSettings> settings);
        static std::string getCommonDefine(std::optional<RendererSettings> settings);
    public:
        static void replaceCommon(Shader& shader, std::optional<RendererSettings> settings);
    };
}