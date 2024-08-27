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

RendererSettings RendererSettings::Builder::build() {
    RendererSettings settings;

    settings.normal_mapping = normal_mapping;

    settings.screen_space_ambient_occlusion = screen_space_ambient_occlusion;
    settings.ssao_settings = ssao_cfg;

    settings.screen_space_reflections = screen_space_reflections;
    settings.ssr_settings = ssr_cfg;

    settings.fast_approximate_antialiasing = fast_approximate_antialiasing;

    settings.cascade_shadow_maps = cascade_shadow_maps;
    settings.csm_resolution = csm_resolution;
    settings.csm_split_count = csm_split_count;
    settings.csm_pcf = csm_pcf;
    settings.csm_micro_shadowing = csm_micro_shadowing;

    settings.bloom = bloom;
    settings.bloom_extract_threshold = bloom_ex_threshold;
    settings.bloom_strength = bloom_str;
    settings.bloom_blur_iteration_count = bloom_blur_it_count;

    settings.specular_aa = specular_aa;
    settings.specular_aa_threshold = specular_threshold;
    settings.specular_aa_variance = specular_variance;

    settings.light_radius = light_radius;
    settings.coordinate_system_axes = coordinate_system_axes;
    settings.bounding_box = bounding_box;

    return settings;
}

RendererSettings::Builder &RendererSettings::Builder::ssao_sample_count(glm::vec2 count) {
    ssao_cfg.sample_count = count;
    return *this;
}

RendererSettings::Builder &RendererSettings::Builder::ssao_spiral_turns(float turns) {
    ssao_cfg.spiral_turns = turns;
    return *this;
}

RendererSettings::Builder &RendererSettings::Builder::ssao_power(float power) {
    ssao_cfg.power = power;
    return *this;
}

RendererSettings::Builder &RendererSettings::Builder::ssao_bias(float bias) {
    ssao_cfg.bias = bias;
    return *this;
}

RendererSettings::Builder &RendererSettings::Builder::csm_texture_resolution(glm::uvec2 resolution) {
    csm_resolution = resolution;
    return *this;
}

RendererSettings::Builder &RendererSettings::Builder::csm_count(uint32_t count) {
    csm_split_count = count;
    return *this;
}

RendererSettings::Builder &RendererSettings::Builder::csm_enable_pcf() {
    csm_pcf = true;
    return *this;
}

RendererSettings::Builder &RendererSettings::Builder::csm_disable_pcf() {
    csm_pcf = false;
    return *this;
}

RendererSettings::Builder &RendererSettings::Builder::csm_enable_micro_shadowing() {
    csm_micro_shadowing = true;
    return *this;
}

RendererSettings::Builder &RendererSettings::Builder::csm_disable_micro_shadowing() {
    csm_micro_shadowing = false;
    return *this;
}

RendererSettings::Builder &RendererSettings::Builder::enable_bloom() {
    bloom = true;
    return *this;
}

RendererSettings::Builder &RendererSettings::Builder::disable_bloom() {
    bloom = false;
    return *this;
}

RendererSettings::Builder &RendererSettings::Builder::bloom_extract_threshold(float threshold) {
    bloom_ex_threshold = threshold;
    return *this;
}

RendererSettings::Builder &RendererSettings::Builder::bloom_strength(float strength) {
    bloom_str = strength;
    return *this;
}

RendererSettings::Builder &RendererSettings::Builder::bloom_blur_iteration_count(uint32_t count) {
    bloom_blur_it_count = count;
    return *this;
}

RendererSettings::Builder &RendererSettings::Builder::enable_specular_aa() {
    specular_aa = true;
    return *this;
}

RendererSettings::Builder &RendererSettings::Builder::disable_specular_aa() {
    specular_aa = false;
    return *this;
}

RendererSettings::Builder &RendererSettings::Builder::specular_aa_threshold(float threshold) {
    specular_threshold = threshold;
    return *this;
}

RendererSettings::Builder &RendererSettings::Builder::specular_aa_variance(float variance) {
    specular_variance = variance;
    return *this;
}

RendererSettings::Builder &RendererSettings::Builder::debug_light_radius() {
    light_radius = true;
    return *this;
}

RendererSettings::Builder &RendererSettings::Builder::debug_coordinate_system_axes() {
    coordinate_system_axes = true;
    return *this;
}

RendererSettings::Builder &RendererSettings::Builder::debug_bounding_box() {
    bounding_box = true;
    return *this;
}
