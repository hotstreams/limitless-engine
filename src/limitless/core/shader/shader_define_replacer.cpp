#include <limitless/core/shader/shader_define_replacer.hpp>
#include <limitless/core/context_initializer.hpp>
#include <limitless/core/keyline_extensions.hpp>
#include <limitless/renderer/render_settings_shader_definer.hpp>
#include <limitless/core/shader/shader.hpp>
#include <limitless/renderer/render_settings.hpp>

using namespace Limitless;

std::string ShaderDefineReplacer::getVersionDefine() {
    return "#version " + std::to_string(ContextInitializer::major_version) + std::to_string(ContextInitializer::minor_version) + "0 core\n";
}

std::string ShaderDefineReplacer::getExtensionDefine() {
    std::string extensions;

    if (ContextInitializer::isExtensionSupported(shader_storage_buffer_object)) {
        extensions.append(extension_shader_storage_buffer_object);
    }

    if (ContextInitializer::isExtensionSupported(shading_language_420pack)) {
        extensions.append(extension_shading_language_420pack);
    }

    if (ContextInitializer::isExtensionSupported(explicit_uniform_location)) {
        extensions.append(extension_explicit_uniform_location);
    }

    if (ContextInitializer::isExtensionSupported(bindless_texture)) {
        extensions.append(extension_bindless_texture);
        extensions.append(bindless_texture_define);
        extensions.append(bindless_samplers);
    }

    return extensions;
}

std::string ShaderDefineReplacer::getSettingsDefine(std::optional<RenderSettings> settings) {
    return settings ? RenderSettingsShaderDefiner::getDefine(*settings) : std::string {};
}

std::string ShaderDefineReplacer::getCommonDefine(std::optional<RenderSettings> settings) {
    std::string define = getVersionDefine();
    define.append(getExtensionDefine());
    define.append(getSettingsDefine(settings));
    return define;
}

void ShaderDefineReplacer::replaceCommon(Shader& shader, std::optional<RenderSettings> settings) {
    shader.replaceKey(DEFINE_NAMES.at(Define::GLSLVersion), getVersionDefine());
    shader.replaceKey(DEFINE_NAMES.at(Define::Extensions), getExtensionDefine());
    shader.replaceKey(DEFINE_NAMES.at(Define::Settings), getExtensionDefine());
    shader.replaceKey(DEFINE_NAMES.at(Define::Common), getCommonDefine(settings));
}
