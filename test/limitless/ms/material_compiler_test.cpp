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
                                             .setModelShaders({ModelShader::Model, ModelShader::Skeletal, ModelShader::Instanced})
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
                            .setModelShaders({ModelShader::Model, ModelShader::Skeletal, ModelShader::Instanced})
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
                            .setModelShaders({ModelShader::Model, ModelShader::Skeletal, ModelShader::Instanced})
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
            .setModelShaders({ModelShader::Model, ModelShader::Skeletal, ModelShader::Instanced})
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
            .setModelShaders({ModelShader::Model, ModelShader::Skeletal, ModelShader::Instanced})
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
            .setModelShaders({ModelShader::Model, ModelShader::Skeletal, ModelShader::Instanced})
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
            .setModelShaders({ModelShader::Model, ModelShader::Skeletal, ModelShader::Instanced})
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
            .setModelShaders({ModelShader::Model, ModelShader::Skeletal, ModelShader::Instanced})
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
            .setModelShaders({ModelShader::Model, ModelShader::Skeletal, ModelShader::Instanced})
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
            .setModelShaders({ModelShader::Model, ModelShader::Skeletal, ModelShader::Instanced})
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
            .setModelShaders({ModelShader::Model, ModelShader::Skeletal, ModelShader::Instanced})
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
            .setModelShaders({ModelShader::Model, ModelShader::Skeletal, ModelShader::Instanced})
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
            .setModelShaders({ModelShader::Model, ModelShader::Skeletal, ModelShader::Instanced})
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
            .setModelShaders({ModelShader::Model, ModelShader::Skeletal, ModelShader::Instanced})
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
            .setModelShaders({ModelShader::Model, ModelShader::Skeletal, ModelShader::Instanced})
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
            .setModelShaders({ModelShader::Model, ModelShader::Skeletal, ModelShader::Instanced})
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
            .setModelShaders({ModelShader::Model, ModelShader::Skeletal, ModelShader::Instanced})
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
            .setModelShaders({ModelShader::Model, ModelShader::Skeletal, ModelShader::Instanced})
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
            .setModelShaders({ModelShader::Model, ModelShader::Skeletal, ModelShader::Instanced})
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
            .setModelShaders({ModelShader::Model, ModelShader::Skeletal, ModelShader::Instanced})
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
            .setModelShaders({ModelShader::Model, ModelShader::Skeletal, ModelShader::Instanced})
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
            .setModelShaders({ModelShader::Model, ModelShader::Skeletal, ModelShader::Instanced})
            .build();

    MaterialCompiler compiler {context, assets, settings};

    REQUIRE_NOTHROW(assets.compileMaterial(context, settings, material));

    check_opengl_state();
}

std::vector<RenderSettings> generateSettings() {
    RenderSettings s1 {};
    s1.normal_mapping = true;
    s1.screen_space_ambient_occlusion = false;
    s1.directional_cascade_shadow_mapping = false;
    s1.directional_pcf = false;
    s1.micro_shadowing = false;

    RenderSettings s2 {};
    s2.normal_mapping = true;
    s2.screen_space_ambient_occlusion = true;
    s2.directional_cascade_shadow_mapping = false;
    s2.directional_pcf = false;
    s2.micro_shadowing = false;

    RenderSettings s3 {};
    s3.normal_mapping = true;
    s3.screen_space_ambient_occlusion = true;
    s3.directional_cascade_shadow_mapping = true;
    s3.directional_pcf = false;
    s3.micro_shadowing = false;

    RenderSettings s4 {};
    s4.normal_mapping = true;
    s4.screen_space_ambient_occlusion = true;
    s4.directional_cascade_shadow_mapping = true;
    s4.directional_pcf = true;
    s4.micro_shadowing = false;

    RenderSettings s5 {};
    s5.normal_mapping = true;
    s5.screen_space_ambient_occlusion = true;
    s5.directional_cascade_shadow_mapping = true;
    s5.directional_pcf = false;
    s5.micro_shadowing = true;

    RenderSettings s6 {};
    s6.normal_mapping = true;
    s6.screen_space_ambient_occlusion = true;
    s6.directional_cascade_shadow_mapping = true;
    s6.directional_pcf = true;
    s6.micro_shadowing = true;

    RenderSettings s7 {};
    s7.normal_mapping = false;
    s7.screen_space_ambient_occlusion = false;
    s7.directional_cascade_shadow_mapping = false;
    s7.directional_pcf = false;
    s7.micro_shadowing = false;

    RenderSettings s8 {};
    s8.normal_mapping = false;
    s8.screen_space_ambient_occlusion = true;
    s8.directional_cascade_shadow_mapping = false;
    s8.directional_pcf = false;
    s8.micro_shadowing = false;

    RenderSettings s9 {};
    s9.normal_mapping = false;
    s9.screen_space_ambient_occlusion = true;
    s9.directional_cascade_shadow_mapping = true;
    s9.directional_pcf = false;
    s9.micro_shadowing = false;

    RenderSettings s10 {};
    s10.normal_mapping = false;
    s10.screen_space_ambient_occlusion = true;
    s10.directional_cascade_shadow_mapping = true;
    s10.directional_pcf = true;
    s10.micro_shadowing = false;

    RenderSettings s11 {};
    s11.normal_mapping = false;
    s11.screen_space_ambient_occlusion = true;
    s11.directional_cascade_shadow_mapping = true;
    s11.directional_pcf = false;
    s11.micro_shadowing = true;

    RenderSettings s12 {};
    s12.normal_mapping = false;
    s12.screen_space_ambient_occlusion = true;
    s12.directional_cascade_shadow_mapping = true;
    s12.directional_pcf = true;
    s12.micro_shadowing = true;

    return {s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12};
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_color for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
            << settings.screen_space_ambient_occlusion
            << settings.directional_cascade_shadow_mapping
            << settings.directional_pcf
            << settings.micro_shadowing
            << std::endl;
        test_MaterialCompiler_compiles_material_with_color(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_emissive for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.directional_cascade_shadow_mapping
                  << settings.directional_pcf
                  << settings.micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_emissive(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_metalic for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.directional_cascade_shadow_mapping
                  << settings.directional_pcf
                  << settings.micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_metalic(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_roughness for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.directional_cascade_shadow_mapping
                  << settings.directional_pcf
                  << settings.micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_roughness(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_tesselation for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.directional_cascade_shadow_mapping
                  << settings.directional_pcf
                  << settings.micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_tesselation(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_refraction_ior for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.directional_cascade_shadow_mapping
                  << settings.directional_pcf
                  << settings.micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_refraction_ior(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_refration_abs for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.directional_cascade_shadow_mapping
                  << settings.directional_pcf
                  << settings.micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_refration_abs(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_diffuse for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.directional_cascade_shadow_mapping
                  << settings.directional_pcf
                  << settings.micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_diffuse(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_normal for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.directional_cascade_shadow_mapping
                  << settings.directional_pcf
                  << settings.micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_normal(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_emissivemask for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.directional_cascade_shadow_mapping
                  << settings.directional_pcf
                  << settings.micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_emissivemask(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_blendmask for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.directional_cascade_shadow_mapping
                  << settings.directional_pcf
                  << settings.micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_blendmask(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_metallurgical for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.directional_cascade_shadow_mapping
                  << settings.directional_pcf
                  << settings.micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_mettalictexture(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_roughnesstexture for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.directional_cascade_shadow_mapping
                  << settings.directional_pcf
                  << settings.micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_roughnesstexture(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_ao for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.directional_cascade_shadow_mapping
                  << settings.directional_pcf
                  << settings.micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_ao(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_unlit for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.directional_cascade_shadow_mapping
                  << settings.directional_pcf
                  << settings.micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_unlit(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_lit for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.directional_cascade_shadow_mapping
                  << settings.directional_pcf
                  << settings.micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_lit(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_custom_fragment for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.directional_cascade_shadow_mapping
                  << settings.directional_pcf
                  << settings.micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_custom_fragment(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_custom_vertex for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.directional_cascade_shadow_mapping
                  << settings.directional_pcf
                  << settings.micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_custom_vertex(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_custom_tess for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.directional_cascade_shadow_mapping
                  << settings.directional_pcf
                  << settings.micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_custom_tess(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_custom_global for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.directional_cascade_shadow_mapping
                  << settings.directional_pcf
                  << settings.micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_custom_global(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_custom_uniform_value for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.directional_cascade_shadow_mapping
                  << settings.directional_pcf
                  << settings.micro_shadowing
                  << std::endl;
        test_MaterialCompiler_compiles_material_with_custom_uniform_value(settings);
    }
}

TEST_CASE("test_MaterialCompiler_compiles_material_with_custom_uniform_sampler for all render settings") {
    for (auto& settings: generateSettings()) {
        std::cout << "With: "
                  << settings.screen_space_ambient_occlusion
                  << settings.directional_cascade_shadow_mapping
                  << settings.directional_pcf
                  << settings.micro_shadowing
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
            .setModelShaders({ModelShader::Model, ModelShader::Skeletal, ModelShader::Instanced})
            .build();

    RenderSettings settings {};
    REQUIRE_THROWS_AS(assets.compileMaterial(context, settings, material), material_compilation_error);

    check_opengl_state();
}