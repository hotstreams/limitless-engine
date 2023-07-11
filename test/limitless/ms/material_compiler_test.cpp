#include "../catch_amalgamated.hpp"
#include "../opengl_state.hpp"
#include "../util/textures.hpp"

#include <limitless/ms/material_builder.hpp>
#include <limitless/ms/material_compiler.hpp>
#include <limitless/core/context.hpp>
#include <limitless/assets.hpp>
#include <iostream>

using namespace Limitless;
using namespace Limitless::ms;
using namespace LimitlessTest;

void test_MaterialCompiler_compiles_material_with_color(RenderSettings& settings) {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};
    auto material = builder.setName("material")
                                             .add(Property::Color, glm::vec4{1.0f})
                                             .setModelShaders({InstanceType::Model, InstanceType::Skeletal, InstanceType::Instanced})
                                             .build();

    REQUIRE_NOTHROW(assets.compileMaterial(context, settings, material));

    check_opengl_state();
}

void test_MaterialCompiler_compiles_material_with_emissive(RenderSettings& settings) {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};
    auto material = builder.setName("material")
                            .add(Property::EmissiveColor, glm::vec4{1.0f})
                            .setModelShaders({InstanceType::Model, InstanceType::Skeletal, InstanceType::Instanced})
                            .build();

    MaterialCompiler compiler {context, assets, settings};

    REQUIRE_NOTHROW(assets.compileMaterial(context, settings, material));

    check_opengl_state();
}

void test_MaterialCompiler_compiles_material_with_metalic(RenderSettings& settings) {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};
    auto material = builder.setName("material")
                            .add(Property::Metallic, 1.0f)
                            .setModelShaders({InstanceType::Model, InstanceType::Skeletal, InstanceType::Instanced})
                            .build();

    MaterialCompiler compiler {context, assets, settings};

    REQUIRE_NOTHROW(assets.compileMaterial(context, settings, material));

    check_opengl_state();
}

void test_MaterialCompiler_compiles_material_with_roughness(RenderSettings& settings) {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};
    auto material = builder.setName("material")
            .add(Property::Roughness, 1.0f)
            .setModelShaders({InstanceType::Model, InstanceType::Skeletal, InstanceType::Instanced})
            .build();

    MaterialCompiler compiler {context, assets, settings};

    REQUIRE_NOTHROW(assets.compileMaterial(context, settings, material));

    check_opengl_state();
}

void test_MaterialCompiler_compiles_material_with_tesselation(RenderSettings& settings) {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};
    auto material = builder.setName("material")
            .add(Property::TessellationFactor, glm::vec2{8.0f, 8.0f})
            .setModelShaders({InstanceType::Model, InstanceType::Skeletal, InstanceType::Instanced})
            .build();

    MaterialCompiler compiler {context, assets, settings};

    REQUIRE_NOTHROW(assets.compileMaterial(context, settings, material));

    check_opengl_state();
}

void test_MaterialCompiler_compiles_material_with_refraction_ior(RenderSettings& settings) {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};
    auto material = builder.setName("material")
            .add(Property::IoR, 0.66f)
            .setRefraction(true)
            .setModelShaders({InstanceType::Model, InstanceType::Skeletal, InstanceType::Instanced})
            .build();

    MaterialCompiler compiler {context, assets, settings};

    REQUIRE_NOTHROW(assets.compileMaterial(context, settings, material));

    check_opengl_state();
}

void test_MaterialCompiler_compiles_material_with_refration_abs(RenderSettings& settings) {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};
    auto material = builder.setName("material")
            .add(Property::Absorption, 0.66f)
            .setRefraction(true)
            .setModelShaders({InstanceType::Model, InstanceType::Skeletal, InstanceType::Instanced})
            .build();

    MaterialCompiler compiler {context, assets, settings};

    REQUIRE_NOTHROW(assets.compileMaterial(context, settings, material));

    check_opengl_state();
}

void test_MaterialCompiler_compiles_material_with_diffuse(RenderSettings& settings) {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};
    assets.textures.add("fake", Textures::fake());

    MaterialBuilder builder {assets};
    auto material = builder.setName("material")
            .add(Property::Diffuse, assets.textures.at("fake"))
            .setModelShaders({InstanceType::Model, InstanceType::Skeletal, InstanceType::Instanced})
            .build();

    MaterialCompiler compiler {context, assets, settings};

    REQUIRE_NOTHROW(assets.compileMaterial(context, settings, material));

    check_opengl_state();
}

void test_MaterialCompiler_compiles_material_with_normal(RenderSettings& settings) {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};
    assets.textures.add("fake", Textures::fake());

    MaterialBuilder builder {assets};
    auto material = builder.setName("material")
            .add(Property::Normal, assets.textures.at("fake"))
            .setModelShaders({InstanceType::Model, InstanceType::Skeletal, InstanceType::Instanced})
            .build();

    MaterialCompiler compiler {context, assets, settings};

    REQUIRE_NOTHROW(assets.compileMaterial(context, settings, material));

    check_opengl_state();
}

void test_MaterialCompiler_compiles_material_with_emissivemask(RenderSettings& settings) {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};
    assets.textures.add("fake", Textures::fake());

    MaterialBuilder builder {assets};
    auto material = builder.setName("material")
            .add(Property::EmissiveMask, assets.textures.at("fake"))
            .setModelShaders({InstanceType::Model, InstanceType::Skeletal, InstanceType::Instanced})
            .build();

    MaterialCompiler compiler {context, assets, settings};

    REQUIRE_NOTHROW(assets.compileMaterial(context, settings, material));

    check_opengl_state();
}

void test_MaterialCompiler_compiles_material_with_blendmask(RenderSettings& settings) {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};
    assets.textures.add("fake", Textures::fake());

    MaterialBuilder builder {assets};
    auto material = builder.setName("material")
            .add(Property::BlendMask, assets.textures.at("fake"))
            .setModelShaders({InstanceType::Model, InstanceType::Skeletal, InstanceType::Instanced})
            .build();

    MaterialCompiler compiler {context, assets, settings};

    REQUIRE_NOTHROW(assets.compileMaterial(context, settings, material));

    check_opengl_state();
}

void test_MaterialCompiler_compiles_material_with_mettalictexture(RenderSettings& settings) {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};
    assets.textures.add("fake", Textures::fake());

    MaterialBuilder builder {assets};
    auto material = builder.setName("material")
            .add(Property::MetallicTexture, assets.textures.at("fake"))
            .setModelShaders({InstanceType::Model, InstanceType::Skeletal, InstanceType::Instanced})
            .build();

    MaterialCompiler compiler {context, assets, settings};

    REQUIRE_NOTHROW(assets.compileMaterial(context, settings, material));

    check_opengl_state();
}

void test_MaterialCompiler_compiles_material_with_roughnesstexture(RenderSettings& settings) {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};
    assets.textures.add("fake", Textures::fake());

    MaterialBuilder builder {assets};
    auto material = builder.setName("material")
            .add(Property::RoughnessTexture, assets.textures.at("fake"))
            .setModelShaders({InstanceType::Model, InstanceType::Skeletal, InstanceType::Instanced})
            .build();

    MaterialCompiler compiler {context, assets, settings};

    REQUIRE_NOTHROW(assets.compileMaterial(context, settings, material));

    check_opengl_state();
}

void test_MaterialCompiler_compiles_material_with_ao(RenderSettings& settings) {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};
    assets.textures.add("fake", Textures::fake());

    MaterialBuilder builder {assets};
    auto material = builder.setName("material")
            .add(Property::AmbientOcclusionTexture, assets.textures.at("fake"))
            .setModelShaders({InstanceType::Model, InstanceType::Skeletal, InstanceType::Instanced})
            .build();

    MaterialCompiler compiler {context, assets, settings};

    REQUIRE_NOTHROW(assets.compileMaterial(context, settings, material));

    check_opengl_state();
}

void test_MaterialCompiler_compiles_material_with_orm(RenderSettings& settings) {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};
    assets.textures.add("fake", Textures::fake());

    MaterialBuilder builder {assets};
    auto material = builder.setName("material")
            .add(Property::ORM, assets.textures.at("fake"))
            .setModelShaders({InstanceType::Model, InstanceType::Skeletal, InstanceType::Instanced})
            .build();

    MaterialCompiler compiler {context, assets, settings};

    REQUIRE_NOTHROW(assets.compileMaterial(context, settings, material));

    check_opengl_state();
}

void test_MaterialCompiler_compiles_material_with_unlit(RenderSettings& settings) {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};
    auto material = builder.setName("material")
            .add(Property::Color, glm::vec4 {1.0f})
            .setShading(Shading::Unlit)
            .setModelShaders({InstanceType::Model, InstanceType::Skeletal, InstanceType::Instanced})
            .build();

    MaterialCompiler compiler {context, assets, settings};

    REQUIRE_NOTHROW(assets.compileMaterial(context, settings, material));

    check_opengl_state();
}

void test_MaterialCompiler_compiles_material_with_lit(RenderSettings& settings) {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};
    auto material = builder.setName("material")
            .add(Property::Color, glm::vec4 {1.0f})
            .setShading(Shading::Lit)
            .setModelShaders({InstanceType::Model, InstanceType::Skeletal, InstanceType::Instanced})
            .build();

    MaterialCompiler compiler {context, assets, settings};

    REQUIRE_NOTHROW(assets.compileMaterial(context, settings, material));

    check_opengl_state();
}

void test_MaterialCompiler_compiles_material_with_custom_fragment(RenderSettings& settings) {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};
    auto material = builder.setName("material")
            .add(Property::Color, glm::vec4 {1.0f})
            .setFragmentSnippet("")
            .setModelShaders({InstanceType::Model, InstanceType::Skeletal, InstanceType::Instanced})
            .build();

    MaterialCompiler compiler {context, assets, settings};

    REQUIRE_NOTHROW(assets.compileMaterial(context, settings, material));

    check_opengl_state();
}

void test_MaterialCompiler_compiles_material_with_custom_vertex(RenderSettings& settings) {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};
    auto material = builder.setName("material")
            .add(Property::Color, glm::vec4 {1.0f})
            .setVertexSnippet("")
            .setModelShaders({InstanceType::Model, InstanceType::Skeletal, InstanceType::Instanced})
            .build();

    MaterialCompiler compiler {context, assets, settings};

    REQUIRE_NOTHROW(assets.compileMaterial(context, settings, material));

    check_opengl_state();
}

void test_MaterialCompiler_compiles_material_with_custom_tess(RenderSettings& settings) {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};
    auto material = builder.setName("material")
            .add(Property::Color, glm::vec4 {1.0f})
            .setTessellationSnippet("")
            .add(Property::TessellationFactor, glm::vec2 {16.0f})
            .setModelShaders({InstanceType::Model, InstanceType::Skeletal, InstanceType::Instanced})
            .build();

    MaterialCompiler compiler {context, assets, settings};

    REQUIRE_NOTHROW(assets.compileMaterial(context, settings, material));

    check_opengl_state();
}

void test_MaterialCompiler_compiles_material_with_custom_global(RenderSettings& settings) {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};
    auto material = builder.setName("material")
            .add(Property::Color, glm::vec4 {1.0f})
            .setGlobalSnippet("")
            .setModelShaders({InstanceType::Model, InstanceType::Skeletal, InstanceType::Instanced})
            .build();

    MaterialCompiler compiler {context, assets, settings};

    REQUIRE_NOTHROW(assets.compileMaterial(context, settings, material));

    check_opengl_state();
}

void test_MaterialCompiler_compiles_material_with_custom_uniform_value(RenderSettings& settings) {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};
    auto material = builder.setName("material")
            .add(Property::Color, glm::vec4 {1.0f})
            .addUniform(std::make_unique<UniformValue<float>>("custom", 0.5f))
            .setModelShaders({InstanceType::Model, InstanceType::Skeletal, InstanceType::Instanced})
            .build();

    MaterialCompiler compiler {context, assets, settings};

    REQUIRE_NOTHROW(assets.compileMaterial(context, settings, material));

    check_opengl_state();
}

void test_MaterialCompiler_compiles_material_with_custom_uniform_sampler(RenderSettings& settings) {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};
    assets.textures.add("fake", Textures::fake());

    MaterialBuilder builder {assets};
    auto material = builder.setName("material")
            .add(Property::Color, glm::vec4 {1.0f})
            .addUniform(std::make_unique<UniformSampler>("custom", assets.textures.at("fake")))
            .setModelShaders({InstanceType::Model, InstanceType::Skeletal, InstanceType::Instanced})
            .build();

    MaterialCompiler compiler {context, assets, settings};

    REQUIRE_NOTHROW(assets.compileMaterial(context, settings, material));

    check_opengl_state();
}

std::vector<RenderSettings> generateSettings() {
    RenderSettings s1 {};
    s1.normal_mapping = true;
    s1.screen_space_ambient_occlusion = false;
    s1.cascade_shadow_maps = false;
    s1.csm_pcf = false;
    s1.csm_micro_shadowing = false;

    RenderSettings s2 {};
    s2.normal_mapping = true;
    s2.screen_space_ambient_occlusion = true;
    s2.cascade_shadow_maps = false;
    s2.csm_pcf = false;
    s2.csm_micro_shadowing = false;

    RenderSettings s3 {};
    s3.normal_mapping = true;
    s3.screen_space_ambient_occlusion = true;
    s3.cascade_shadow_maps = true;
    s3.csm_pcf = false;
    s3.csm_micro_shadowing = false;

    RenderSettings s4 {};
    s4.normal_mapping = true;
    s4.screen_space_ambient_occlusion = true;
    s4.cascade_shadow_maps = true;
    s4.csm_pcf = true;
    s4.csm_micro_shadowing = false;

    RenderSettings s5 {};
    s5.normal_mapping = true;
    s5.screen_space_ambient_occlusion = true;
    s5.cascade_shadow_maps = true;
    s5.csm_pcf = false;
    s5.csm_micro_shadowing = true;

    RenderSettings s6 {};
    s6.normal_mapping = true;
    s6.screen_space_ambient_occlusion = true;
    s6.cascade_shadow_maps = true;
    s6.csm_pcf = true;
    s6.csm_micro_shadowing = true;

    RenderSettings s7 {};
    s7.normal_mapping = false;
    s7.screen_space_ambient_occlusion = false;
    s7.cascade_shadow_maps = false;
    s7.csm_pcf = false;
    s7.csm_micro_shadowing = false;

    RenderSettings s8 {};
    s8.normal_mapping = false;
    s8.screen_space_ambient_occlusion = true;
    s8.cascade_shadow_maps = false;
    s8.csm_pcf = false;
    s8.csm_micro_shadowing = false;

    RenderSettings s9 {};
    s9.normal_mapping = false;
    s9.screen_space_ambient_occlusion = true;
    s9.cascade_shadow_maps = true;
    s9.csm_pcf = false;
    s9.csm_micro_shadowing = false;

    RenderSettings s10 {};
    s10.normal_mapping = false;
    s10.screen_space_ambient_occlusion = true;
    s10.cascade_shadow_maps = true;
    s10.csm_pcf = true;
    s10.csm_micro_shadowing = false;

    RenderSettings s11 {};
    s11.normal_mapping = false;
    s11.screen_space_ambient_occlusion = true;
    s11.cascade_shadow_maps = true;
    s11.csm_pcf = false;
    s11.csm_micro_shadowing = true;

    RenderSettings s12 {};
    s12.normal_mapping = false;
    s12.screen_space_ambient_occlusion = true;
    s12.cascade_shadow_maps = true;
    s12.csm_pcf = true;
    s12.csm_micro_shadowing = true;

    return {s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12};
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_color for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
            << settings.screen_space_ambient_occlusion
            << settings.cascade_shadow_maps
            << settings.csm_pcf
            << settings.csm_micro_shadowing
            << std::endl;
        test_MaterialCompiler_compiles_material_with_color(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_emissive for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.cascade_shadow_maps
                  << settings.csm_pcf
                  << settings.csm_micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_emissive(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_metalic for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.cascade_shadow_maps
                  << settings.csm_pcf
                  << settings.csm_micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_metalic(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_roughness for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.cascade_shadow_maps
                  << settings.csm_pcf
                  << settings.csm_micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_roughness(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_tesselation for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.cascade_shadow_maps
                  << settings.csm_pcf
                  << settings.csm_micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_tesselation(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_refraction_ior for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.cascade_shadow_maps
                  << settings.csm_pcf
                  << settings.csm_micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_refraction_ior(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_refration_abs for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.cascade_shadow_maps
                  << settings.csm_pcf
                  << settings.csm_micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_refration_abs(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_diffuse for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.cascade_shadow_maps
                  << settings.csm_pcf
                  << settings.csm_micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_diffuse(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_normal for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.cascade_shadow_maps
                  << settings.csm_pcf
                  << settings.csm_micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_normal(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_emissivemask for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.cascade_shadow_maps
                  << settings.csm_pcf
                  << settings.csm_micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_emissivemask(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_blendmask for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.cascade_shadow_maps
                  << settings.csm_pcf
                  << settings.csm_micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_blendmask(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_metallurgical for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.cascade_shadow_maps
                  << settings.csm_pcf
                  << settings.csm_micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_mettalictexture(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_roughnesstexture for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.cascade_shadow_maps
                  << settings.csm_pcf
                  << settings.csm_micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_roughnesstexture(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_ao for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.cascade_shadow_maps
                  << settings.csm_pcf
                  << settings.csm_micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_ao(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_orm for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.cascade_shadow_maps
                  << settings.csm_pcf
                  << settings.csm_micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_orm(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_unlit for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.cascade_shadow_maps
                  << settings.csm_pcf
                  << settings.csm_micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_unlit(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_lit for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.cascade_shadow_maps
                  << settings.csm_pcf
                  << settings.csm_micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_lit(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_custom_fragment for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.cascade_shadow_maps
                  << settings.csm_pcf
                  << settings.csm_micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_custom_fragment(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_custom_vertex for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.cascade_shadow_maps
                  << settings.csm_pcf
                  << settings.csm_micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_custom_vertex(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_custom_tess for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.cascade_shadow_maps
                  << settings.csm_pcf
                  << settings.csm_micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_custom_tess(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_custom_global for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.cascade_shadow_maps
                  << settings.csm_pcf
                  << settings.csm_micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_custom_global(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_custom_uniform_value for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.cascade_shadow_maps
                  << settings.csm_pcf
                  << settings.csm_micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_custom_uniform_value(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_custom_uniform_sampler for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.cascade_shadow_maps
                  << settings.csm_pcf
                  << settings.csm_micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_custom_uniform_sampler(settings);
    }
}

TEST_CASE("MaterialCompiler throws") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};
    auto material = builder.setName("material")
            .add(Property::Color, glm::vec4 {1.0f})
            .setFragmentSnippet("shreker")
            .setModelShaders({InstanceType::Model, InstanceType::Skeletal, InstanceType::Instanced})
            .build();

    RenderSettings settings {};
    REQUIRE_THROWS_AS(assets.compileMaterial(context, settings, material), material_compilation_error);

    // we should reset state because shader compilation got an error
    gl_error_count = 0;
}