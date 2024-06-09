#include <limitless/renderer/renderer_settings.hpp>

using namespace Limitless;

RendererSettings::Builder& Limitless::RendererSettings::Builder::enable_normal_mapping() {
    normal_mapping = true;
    return *this;
}

RendererSettings::Builder &RendererSettings::Builder::disable_normal_mapping() {
    normal_mapping = false;
    return *this;
}

RendererSettings::Builder &RendererSettings::Builder::enable_ssao() {
    screen_space_ambient_occlusion = true;
    return *this;
}

RendererSettings::Builder &RendererSettings::Builder::disable_ssao() {
    screen_space_ambient_occlusion = false;
    return *this;
}

RendererSettings::Builder &RendererSettings::Builder::ssao_settings(SSAO::Settings settings) {
    ssao_cfg = settings;
    return *this;
}

RendererSettings::Builder &RendererSettings::Builder::enable_ssr() {
    screen_space_reflections = true;
    return *this;
}

RendererSettings::Builder &RendererSettings::Builder::disable_ssr() {
    screen_space_reflections = false;
    return *this;
}

RendererSettings::Builder &RendererSettings::Builder::ssr_settings(SSR::Settings settings) {
    ssr_cfg = settings;
    return *this;
}

RendererSettings::Builder &RendererSettings::Builder::enable_fxaa() {
    fast_approximate_antialiasing = true;
    return *this;
}

RendererSettings::Builder &RendererSettings::Builder::disable_fxaa() {
    fast_approximate_antialiasing = false;
    return *this;
}

RendererSettings::Builder &RendererSettings::Builder::enable_csm() {
    cascade_shadow_maps = true;
    return *this;
}

RendererSettings::Builder &RendererSettings::Builder::disable_csm() {
    cascade_shadow_maps= false;
    return *this;
}

RendererSettings::Builder &RendererSettings::Builder::csm_settings() {
    return *this;
}

RendererSettings RendererSettings::Builder::build() {
    return {};
}
