#include <limitless/renderer/renderer_settings.hpp>
#include <algorithm>

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
    ambient_occlusion_mode_ = AmbientOcclusionMode::SAO;
    return *this;
}

RendererSettings::Builder &RendererSettings::Builder::disable_ssao() {
    ambient_occlusion_mode_ = AmbientOcclusionMode::None;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::enable_hbao() {
    ambient_occlusion_mode_ = AmbientOcclusionMode::HBAO;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::ambient_occlusion_mode(AmbientOcclusionMode mode) {
    ambient_occlusion_mode_ = mode;
    return *this;
}

RendererSettings::Builder &RendererSettings::Builder::ssao_settings(SSAO::Settings settings) {
    ssao_cfg = settings;
    ssao_filament_quality_.reset();
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::ssao_filament_quality(SsaoFilamentQuality q) {
    ssao_cfg = SSAO::Settings::filamentQualityPreset(q);
    ssao_filament_quality_ = q;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::hbao_settings(HbaoSettings settings) {
    hbao_cfg = settings;
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

RendererSettings::Builder& RendererSettings::Builder::fxaa_settings(FxaaSettings settings) {
    fxaa_cfg = settings;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::fxaa_quality(FxaaQualityPreset preset) {
    fxaa_cfg.quality = preset;
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

    settings.ambient_occlusion_mode = ambient_occlusion_mode_;
    settings.ssao_settings = ssao_cfg;
    settings.ssao_filament_quality_preset = ssao_filament_quality_;
    settings.hbao_settings = hbao_cfg;

    settings.screen_space_reflections = screen_space_reflections;
    settings.ssr_settings = ssr_cfg;

    settings.fast_approximate_antialiasing = fast_approximate_antialiasing;
    settings.fxaa = fxaa_cfg;

    settings.cascade_shadow_maps = cascade_shadow_maps;
    settings.csm_resolution = csm_resolution;
    settings.csm_split_count = csm_split_count;
    settings.csm_stable = csm_stable_;
    settings.csm_focus_shadow_casters = csm_focus_shadow_casters_;
    settings.csm_depth_clamp = csm_depth_clamp_;
    settings.csm_pcf = csm_pcf;
    settings.csm_normal_bias_scale = csm_normal_bias_scale_;
    settings.csm_receiver_plane_bias = csm_receiver_plane_bias_;
    settings.csm_receiver_plane_bias_scale = csm_receiver_plane_bias_scale_;
    settings.csm_constant_bias = csm_constant_bias_;
    settings.csm_micro_shadowing = csm_micro_shadowing;

    settings.bloom = bloom;
    settings.bloom_extract_threshold = bloom_ex_threshold;
    settings.bloom_strength = bloom_str;
    settings.bloom_blur_iteration_count = bloom_blur_it_count;
    settings.bloom_buffer_height = bloom_buffer_height_;
    settings.bloom_fireflies_reduction = bloom_fireflies_reduction_;
    settings.bloom_highlight = bloom_highlight_;

    settings.postprocess.tonemapper = tonemapper;
    settings.postprocess.exposure = exposure;
    settings.postprocess.gamma = gamma;
    settings.postprocess.white_balance_temperature = white_balance_temperature;
    settings.postprocess.white_balance_tint = white_balance_tint;
    settings.postprocess.contrast = contrast;
    settings.postprocess.saturation = saturation;
    settings.postprocess.lift = lift;
    settings.postprocess.grade_gamma = grade_gamma;
    settings.postprocess.gain = gain;
    settings.postprocess.lut_enabled = lut_enabled;
    settings.postprocess.lut_texture = lut_texture;
    settings.postprocess.lut_intensity = lut_intensity;
    settings.postprocess.composite_white_balance = composite_white_balance_;
    settings.postprocess.composite_color_grading = composite_color_grading_;
    settings.postprocess.composite_display_gamma = composite_display_gamma_;

    settings.height_fog.include_in_composite_shader = height_fog_include_in_composite_shader_;
    settings.height_fog.enabled = height_fog_enabled_;
    settings.height_fog.color = height_fog_color_;
    settings.height_fog.density = height_fog_density_;
    settings.height_fog.height = height_fog_height_;
    settings.height_fog.falloff = height_fog_falloff_;
    settings.height_fog.start_distance = height_fog_start_distance_;

    settings.wind_quality = wind_quality_;

    settings.specular_aa = specular_aa;
    settings.specular_aa_threshold = specular_threshold;
    settings.specular_aa_variance = specular_variance;

    settings.light_radius = light_radius;
    settings.coordinate_system_axes = coordinate_system_axes;
    settings.bounding_box = bounding_box;

    settings.light_tile_size = light_tile_size_;
    settings.debug_light_tiles = debug_light_tiles_;

    settings.indirect_draw = indirect_draw_;
    settings.geometry_pool_batching = geometry_pool_batching_;
    settings.persistent_buffer_mapping = persistent_buffer_mapping_;
    settings.triple_buffer_indirect = triple_buffer_indirect_;
    settings.global_model_instance_ssbo = global_model_instance_ssbo_;

    return settings;
}

RendererSettings::Builder& RendererSettings::Builder::wind_quality(WindQuality quality) {
    wind_quality_ = quality;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::enable_height_fog() {
    height_fog_include_in_composite_shader_ = true;
    height_fog_enabled_ = true;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::disable_height_fog() {
    height_fog_include_in_composite_shader_ = false;
    height_fog_enabled_ = false;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::composite_shader_white_balance(bool enabled) {
    composite_white_balance_ = enabled;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::composite_shader_color_grading(bool enabled) {
    composite_color_grading_ = enabled;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::composite_shader_display_gamma(bool enabled) {
    composite_display_gamma_ = enabled;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::height_fog_color(glm::vec3 color) {
    height_fog_color_ = color;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::height_fog_density(float density) {
    height_fog_density_ = density;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::height_fog_height(float height) {
    height_fog_height_ = height;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::height_fog_falloff(float falloff) {
    height_fog_falloff_ = falloff;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::height_fog_start_distance(float distance) {
    height_fog_start_distance_ = distance;
    return *this;
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

RendererSettings::Builder& RendererSettings::Builder::csm_stable(bool enabled) {
    csm_stable_ = enabled;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::csm_focus_shadow_casters(bool enabled) {
    csm_focus_shadow_casters_ = enabled;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::csm_depth_clamp(bool enabled) {
    csm_depth_clamp_ = enabled;
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

RendererSettings::Builder& RendererSettings::Builder::csm_normal_bias_scale(float scale) {
    csm_normal_bias_scale_ = scale;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::csm_receiver_plane_bias(bool enabled) {
    csm_receiver_plane_bias_ = enabled;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::csm_receiver_plane_bias_scale(float scale) {
    csm_receiver_plane_bias_scale_ = scale;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::csm_constant_bias(float bias_world_units) {
    csm_constant_bias_ = bias_world_units;
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

RendererSettings::Builder& RendererSettings::Builder::bloom_buffer_height(uint32_t height_px) {
    bloom_buffer_height_ = std::max(2u, height_px);
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::bloom_fireflies_reduction(bool enabled) {
    bloom_fireflies_reduction_ = enabled;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::bloom_highlight(float highlight) {
    bloom_highlight_ = highlight;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::postprocess_tonemapper(Tonemapper tonemapper_) {
    tonemapper = tonemapper_;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::postprocess_exposure(float exposure_) {
    exposure = exposure_;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::postprocess_gamma(float gamma_) {
    gamma = gamma_;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::postprocess_white_balance(float temperature, float tint) {
    white_balance_temperature = temperature;
    white_balance_tint = tint;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::postprocess_contrast(float contrast_) {
    contrast = contrast_;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::postprocess_saturation(float saturation_) {
    saturation = saturation_;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::postprocess_lift_gamma_gain(glm::vec3 lift_, glm::vec3 gamma_, glm::vec3 gain_) {
    lift = lift_;
    grade_gamma = gamma_;
    gain = gain_;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::postprocess_lut(const std::string& texture_name, float intensity) {
    lut_enabled = true;
    lut_texture = texture_name;
    lut_intensity = intensity;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::postprocess_disable_lut() {
    lut_enabled = false;
    lut_texture.clear();
    lut_intensity = 1.0f;
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

RendererSettings::Builder& RendererSettings::Builder::light_tile_size(uint32_t tile_size_px) {
    // keep sane values; must be >= 1, and we want reasonable tile sizes
    light_tile_size_ = std::max(1u, tile_size_px);
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::debug_light_tiles() {
    debug_light_tiles_ = true;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::enable_indirect_draw() {
    indirect_draw_ = true;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::disable_indirect_draw() {
    indirect_draw_ = false;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::enable_geometry_pool_batching() {
    geometry_pool_batching_ = true;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::disable_geometry_pool_batching() {
    geometry_pool_batching_ = false;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::enable_persistent_buffer_mapping() {
    persistent_buffer_mapping_ = true;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::disable_persistent_buffer_mapping() {
    persistent_buffer_mapping_ = false;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::enable_triple_buffer_indirect() {
    triple_buffer_indirect_ = true;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::disable_triple_buffer_indirect() {
    triple_buffer_indirect_ = false;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::enable_global_model_instance_ssbo() {
    global_model_instance_ssbo_ = true;
    return *this;
}

RendererSettings::Builder& RendererSettings::Builder::disable_global_model_instance_ssbo() {
    global_model_instance_ssbo_ = false;
    return *this;
}

bool RendererSettings::sameCompositeShaderDefines(const RendererSettings& o) const noexcept {
    return bloom == o.bloom
        && height_fog.include_in_composite_shader == o.height_fog.include_in_composite_shader
        && postprocess.lut_enabled == o.postprocess.lut_enabled
        && postprocess.composite_white_balance == o.postprocess.composite_white_balance
        && postprocess.composite_color_grading == o.postprocess.composite_color_grading
        && postprocess.composite_display_gamma == o.postprocess.composite_display_gamma;
}
