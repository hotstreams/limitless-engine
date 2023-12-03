#include "assets.hpp"

#include <limitless/ms/material_builder.hpp>
#include <limitless/fx/effect_builder.hpp>
#include <limitless/fx/emitters/sprite_emitter.hpp>
#include <limitless/fx/emitters/mesh_emitter.hpp>
#include <limitless/fx/emitters/beam_emitter.hpp>
#include <limitless/loaders/texture_loader.hpp>
#include <limitless/loaders/asset_manager.hpp>
#include <limitless/models/skeletal_model.hpp>
#include <limitless/skybox/skybox.hpp>
#include <limitless/text/font_atlas.hpp>
#include <limitless/core/uniform/uniform_time.hpp>
#include <iostream>

using namespace Limitless::ms;
using namespace Limitless::fx;
using namespace Limitless;

void DemoAssets::loadLightingScene() {
    {
        const fs::path assets_dir {getAssetsDir()};
        Material::builder()
            .name("PBR")
            .metallic(TextureLoader::load(*this, assets_dir / "textures/rustediron2_metallic.png"))
            .roughness(TextureLoader::load(*this, assets_dir / "textures/rustediron2_roughness.png"))
            .diffuse(TextureLoader::load(*this, assets_dir / "textures/rustediron2_basecolor.png", {::TextureLoaderFlags::Space::sRGB}))
            .normal(TextureLoader::load(*this, assets_dir / "textures/rustediron2_normal.png"))
            .shading(Shading::Lit)
            .models({::InstanceType::Instanced, ::InstanceType::Model})
            .build(*this);
    }

    {
        const fs::path assets_dir {getAssetsDir()};
        Material::builder()
            .name("floor")
            .shading(Shading::Lit)
            .diffuse(TextureLoader::load(*this, assets_dir / "textures/wood.jpg", {::TextureLoaderFlags::Space::sRGB}))
            .normal(TextureLoader::load(*this, assets_dir / "textures/wood_normal.png"))
            .two_sided(true)
            .models({::InstanceType::Instanced, ::InstanceType::Model})
            .build(*this);
    }
}

void DemoAssets::loadMaterialsScene() {
    const fs::path assets_dir {getAssetsDir()};

    Material::builder()
        .name("color")
        .color({1.0f, 0.5f, 1.0f, 1.0f})
        .shading(Shading::Unlit)
        .build(*this);

    Material::builder()
        .name("albedo")
        .diffuse(TextureLoader::load(*this, assets_dir / "textures/albedo.jpg", {TextureLoaderFlags::Space::sRGB }))
        .shading(Shading::Unlit)
        .build(*this);

    Material::builder()
        .name("emissive_color")
        .emissive_color({5.0f, 5.1f, 0.0f})
        .shading(Shading::Unlit)
        .build(*this);

    Material::builder()
        .name("emissive_mask")
        .emissive_color({2.5f, 0.0f, 5.0f})
        .color({0.0f, 0.0f, 0.0f, 1.0f})
        .emissive_mask(TextureLoader::load(*this, assets_dir / "textures/open_mask.jpg", {TextureLoaderFlags::Filter::Nearest}))
        .shading(Shading::Unlit)
        .blending(::Blending::Additive)
        .two_sided(true)
        .build(*this);

    Material::builder()
        .name("blend_mask")
        .blend_mask(TextureLoader::load(*this, assets_dir / "textures/blend_mask.jpg", {TextureLoaderFlags::Filter::Nearest}))
        .color({1.0f, 0.0f, 0.0f, 1.0f})
        .shading(Shading::Unlit)
        .two_sided(true)
        .build(*this);

    Material::builder()
        .name("basic1")
        .metallic(0.3f)
        .roughness(TextureLoader::load(*this, assets_dir / "textures/stonework_roughness.png"))
        .diffuse(TextureLoader::load(*this, assets_dir / "textures/stonework_albedo.png"))
        .ao(TextureLoader::load(*this, assets_dir / "textures/stonework_ao.png"))
        .normal(TextureLoader::load(*this, assets_dir / "textures/stonework_normal.png"))
        .shading(Shading::Lit)
        .build(*this);

    Material::builder()
        .name("basic2")
        .metallic(TextureLoader::load(*this, assets_dir / "textures/metall_metallic.png"))
        .roughness(TextureLoader::load(*this, assets_dir / "textures/metall_roughness.png"))
        .diffuse(TextureLoader::load(*this, assets_dir / "textures/metall_albedo.png"))
        .ao(TextureLoader::load(*this, assets_dir / "textures/metall_ao.png"))
        .normal(TextureLoader::load(*this, assets_dir / "textures/metall_normal.png"))
        .shading(Shading::Lit)
        .build(*this);

    Material::builder()
        .name("basic3")
        .metallic(TextureLoader::load(*this, assets_dir / "textures/green_metallic.png"))
        .roughness(TextureLoader::load(*this, assets_dir / "textures/green_roughness.png"))
        .diffuse(TextureLoader::load(*this, assets_dir / "textures/green_albedo.png"))
        .ao(TextureLoader::load(*this, assets_dir / "textures/green_ao.png"))
        .normal(TextureLoader::load(*this, assets_dir / "textures/green_normal.png"))
        .shading(Shading::Lit)
        .build(*this);

    Material::builder()
            .name("basic4")
            .metallic(TextureLoader::load(*this, assets_dir / "textures/charcoal_metallic.png"))
            .roughness( TextureLoader::load(*this, assets_dir / "textures/charcoal_roughness.png"))
            .diffuse( TextureLoader::load(*this, assets_dir / "textures/charcoal_albedo.png"))
            .normal( TextureLoader::load(*this, assets_dir / "textures/charcoal_normal.png"))
            .shading(Shading::Lit)
            .two_sided(true)
            .build(*this);

    Material::builder()
            .name("basic5")
            .metallic(TextureLoader::load(*this, assets_dir / "textures/pirate_metallic.png"))
            .roughness( TextureLoader::load(*this, assets_dir / "textures/pirate_roughness.png"))
            .diffuse( TextureLoader::load(*this, assets_dir / "textures/pirate_albedo.png"))
            .normal( TextureLoader::load(*this, assets_dir / "textures/pirate_normal.png"))
            .shading(Shading::Lit)
            .two_sided(true)
            .build(*this);

    Material::builder()
            .name("basic6")
            .metallic(TextureLoader::load(*this, assets_dir / "textures/ornate_metallic.png"))
            .roughness( TextureLoader::load(*this, assets_dir / "textures/ornate_roughness.png"))
            .diffuse( TextureLoader::load(*this, assets_dir / "textures/ornate_albedo.png"))
            .normal( TextureLoader::load(*this, assets_dir / "textures/ornate_normal.png"))
            .shading(Shading::Lit)
            .build(*this);

    Material::builder().name("basic7")
            .metallic(TextureLoader::load(*this, assets_dir / "textures/streaked_metallic.png"))
            .roughness( TextureLoader::load(*this, assets_dir / "textures/streaked_roughness.png"))
            .diffuse( TextureLoader::load(*this, assets_dir / "textures/streaked_albedo.png"))
            .normal( TextureLoader::load(*this, assets_dir / "textures/streaked_normal.png"))
            .shading(Shading::Lit)
            .build(*this);

    Material::builder().name("basic8")
            .metallic(TextureLoader::load(*this, assets_dir / "textures/fabric_metallic.png"))
            .roughness( TextureLoader::load(*this, assets_dir / "textures/fabric_roughness.png"))
            .diffuse( TextureLoader::load(*this, assets_dir / "textures/fabric_albedo.png"))
            .normal( TextureLoader::load(*this, assets_dir / "textures/fabric_normal.png"))
            .shading(Shading::Lit)
            .build(*this);

    Material::builder().name("refraction")
            .blending(Blending::Translucent)
            .refraction(true)
            .ior(1.5)
            .time()
            .custom("noise", TextureLoader::load(*this, assets_dir / "textures/true_noise.tga"))
            .fragment("mctx.IoR = texture(noise, vec2(getVertexUV().x, getVertexUV().y + time * 0.1)).r;"
                                "mctx.IoR = clamp(mctx.IoR, 0.0, 1.0);"
            )
            .shading(Shading::Unlit)
            .build(*this);

    Material::builder().name("blending1")
            .color(glm::vec4(2.0f, 0.0f, 0.0f, 0.5f))
            .shading(Shading::Unlit)
            .blending(Blending::Translucent)
            .two_sided(true)
            .build(*this);

    Material::builder().name("decal")
            .diffuse(TextureLoader::load(*this, assets_dir / "textures/metall_albedo.png"))
            .normal(TextureLoader::load(*this, assets_dir / "textures/normal_4.png"))
            .shading(Shading::Lit)
            .blending(Blending::Translucent)
            .models({InstanceType::Decal, InstanceType::Model})
            .build(*this);

    Material::builder().name("blending2")
            .color(glm::vec4(0.0f, 2.0f, 0.0f, 0.4f))
            .shading(Shading::Unlit)
            .blending(Blending::Translucent)
            .two_sided(true)
            .build(*this);

    Material::builder().name("blending3")
            .color(glm::vec4(0.0f, 0.0f, 2.0f, 0.3f))
            .shading(Shading::Unlit)
            .blending(Blending::Translucent)
            .two_sided(true)
            .build(*this);

    Material::builder().name("blending4")
            .color(glm::vec4(1.0f, 0.5f, 0.5f, 1.0f))
            .shading(Shading::Unlit)
            .blending(Blending::Additive)
            .two_sided(true)
            .build(*this);

    Material::builder().name("blending5")
            .color(glm::vec4(2.0f, 0.3f, 0.3f, 1.0f))
            .shading(Shading::Unlit)
            .blending(Blending::Modulate)
            .two_sided(true)
            .build(*this);

    Material::builder().name("lava")
            .diffuse( TextureLoader::load(*this, assets_dir / "textures/lava.png", {
                TextureLoaderFlags::Space::sRGB
            }))
            .emissive_color( glm::vec4(2.5f, 0.9f, 0.1f, 1.0f))
            .color(glm::vec4(2.5f, 0.9f, 0.1f, 1.0f))
            .custom("noise", TextureLoader::load(*this, assets_dir / "textures/true_noise.tga"))
            .time()
            .fragment(
                    "vec2 panned = vec2(getVertexUV().x + time * 0.1, getVertexUV().y + time * 0.05);"
                    "vec2 uv = getVertexUV() + texture(noise, panned).r;"
                    "mctx.color.rgb = getMaterialDiffuse(uv).rgb;"
                    "mctx.emissive_color *= texture(noise, panned).g;"
            )
            .shading(Shading::Unlit)
            .build(*this);

    Material::builder().name("ice")
            .emissive_color( glm::vec4(1.0f))
            .color(glm::vec4(1.0f))
            .diffuse( TextureLoader::load(*this, assets_dir / "textures/ice.jpg", {
                    TextureLoaderFlags::Space::sRGB
            }))
            .custom("snow", TextureLoader::load(*this, assets_dir / "textures/snow.jpg"))
            .time()
            .fragment(
                    "vec2 uv = getVertexUV() + time * 0.05;"
                    "mctx.color.rgb += texture(snow, uv).rgb * abs(cos(time * 0.5));"
                    "mctx.emissive_color *= texture(snow, uv).r;"
            )
            .shading(Shading::Unlit)
            .build(*this);

    Material::builder().name("poison")
            .emissive_color( glm::vec4(0.1f, 4.0f, 0.1f, 1.0f))
            .color(glm::vec4(0.1f, 4.0f, 0.1f, 1.0f))
            .diffuse( TextureLoader::load(*this, assets_dir / "textures/poison.jpg", {
                    TextureLoaderFlags::Space::sRGB
            }))
            .custom("noise", TextureLoader::load(*this, assets_dir / "textures/true_noise.tga"))
            .time()
            .fragment(
            "vec2 panned = vec2(getVertexUV().x + time * 0.05, getVertexUV().y);"
                    "vec2 uv = vec2(getVertexUV().x, getVertexUV().y  + texture(noise, panned).g);"
                    "mctx.color.rgb = getMaterialDiffuse(uv).rgb;"
                    "mctx.emissive_color *= getMaterialDiffuse(uv).g;"
            )
            .shading(Shading::Unlit)
            .build(*this);

    Material::builder()
            .name("fireball")
            .vertex("vec2 uv_1 = vec2(uv.x + time * 0.05, uv.y + time * 0.0);\n"
                              "vec2 uv_2 = vec2(uv.x - time * 0.05, uv.y - time * 0.0);\n"
                              " \n"
                              "float s = texture(fire_mask, uv_1).r;\n"
                              "float t = texture(fire_mask, uv_2).r;\n"
                              "\n"
                              "vertex_position.xyz -= getVertexNormal() * texture(fire_mask, uv + vec2(s, t)).r * 0.6;")
            .fragment("vec2 uv = getVertexUV();"
                                "vec2 uv_1 = vec2(uv.x + time * 0.05, uv.y + time * 0.0);\n"
                                "vec2 uv_2 = vec2(uv.x - time * 0.05, uv.y - time * 0.0);\n"
                                " \n"
                                "float s = texture(fire_mask, uv_1).r;\n"
                                "float t = texture(fire_mask, uv_2).r;\n"
                                "\n"
                                "mctx.emissive_color = getMaterialDiffuse(uv + vec2(s, t)).rgb;\n"
                                "\n"
                                "if (mctx.emissive_color.r <= 0.2) discard;")
            .time()
            .custom("fire_mask", TextureLoader::load(*this, assets_dir / "textures/fireball_mask.png"))
            .diffuse( TextureLoader::load(*this, assets_dir / "textures/rock_lava.png"))
            .color(glm::vec4{15.0f, 5.0f, 0.0f, 1.0f})
            .emissive_color( glm::vec4{1.0f})
            .shading(Shading::Unlit)
            .blending(Blending::Additive)
            .two_sided(true)
            .build(*this);

    Material::builder().name("bump_mapping")
            .metallic(TextureLoader::load(*this, assets_dir / "textures/brickwork_metallic.png"))
            .roughness( TextureLoader::load(*this, assets_dir / "textures/brickwork_roughness.png"))
            .diffuse( TextureLoader::load(*this, assets_dir / "textures/brickwork_albedo.png"))
            .normal( TextureLoader::load(*this, assets_dir / "textures/brickwork_normal.png"))
            .ao(TextureLoader::load(*this, assets_dir / "textures/brickwork_ao.png"))
            .custom("height", TextureLoader::load(*this, assets_dir / "textures/brickwork_height.png"))
            .shading(Shading::Lit)
            .vertex(
                    "vertex_position += getVertexNormal() * texture(height, getVertexUV()).xyz * 0.25;"
            )
            .build(*this);

    Material::builder().name("fresnel")
            .emissive_color(glm::vec4(5.0, 0.7, 0.3, 1.0))
            .time()
            .shading(Shading::Unlit)
            .diffuse( TextureLoader::load(*this, assets_dir / "textures/albedo.jpg", {
                    TextureLoaderFlags::Space::sRGB
            }))
            .global(
                    "#include \"../functions/fresnel.glsl\""
            )
            .fragment(
                    "mctx.emissive_color *= fresnel(getVertexNormal(), getCameraPosition() - getVertexPosition(), 5.0);"
            )
            .build(*this);

    Material::builder().name("hue_shift")
            .time()
            .shading(Shading::Unlit)
            .diffuse( TextureLoader::load(*this, assets_dir / "textures/albedo.jpg", {
                    TextureLoaderFlags::Space::sRGB
            }))
            .color(glm::vec4(1.0))
            .global(
                    "#include \"../functions/hue_shift.glsl\""
            )
            .fragment(
                    "mctx.color.rgb = hue_shift(mctx.color.rgb, time);"
            )
            .build(*this);
}

void DemoAssets::loadEffectsScene() {
    {
        const fs::path assets_dir {ENGINE_ASSETS_DIR};

        Material::builder()
                .name("blink")
                .color(glm::vec4(1.0f))
                .custom("mask", TextureLoader::load(*this, assets_dir / "textures/fireball_mask.png"))
                .diffuse( TextureLoader::load(*this, assets_dir / "textures/blink.jpg"))
                .shading(Shading::Unlit)
                .blending(Blending::Translucent)
                .two_sided(true)
                .fragment("vec2 uv = getVertexUV();"
                                    "float tile = getParticleProperties().z;\n"
                                    "float erode = getParticleProperties().y;\n"
                                    "float dist = getParticleProperties().x;\n"
                                    "\n"
                                    "float r = texture(mask, uv * tile).r;\n"
                                    "\n"
                                    "vec2 uv1 = vec2(0.0, dist) * r + uv;\n"
                                    "\n"
                                    "float m_a = getMaterialDiffuse(uv1).r;\n"
                                    "\n"
                                    "mctx.color.a = mix(0.0, m_a, pow(r, erode));"
                )
                .model(InstanceType::Effect)
                .build(*this);
    }

    {
        EffectBuilder builder {*this};
        builder .create("blink")
                    .createEmitter<SpriteEmitter>("smoke_purple")
                    .setSpawnMode(::EmitterSpawn::Mode::Burst)
                    .setBurstCount(std::make_unique<ConstDistribution<uint32_t>>(100))
                    .setMaxCount(100)
                    .setSpawnRate(100.0f)
                    .addInitialSize(std::make_unique<RangeDistribution<float>>(512.0f, 1024.0f))
                    .addSizeByLife(std::make_unique<ConstDistribution<float>>(2048.0f))
                    .addInitialRotation(std::make_unique<RangeDistribution<glm::vec3>>(glm::vec3(0.0f), glm::vec3(6.28f)))
                    .addInitialMeshLocation(meshes.at("sphere"), glm::vec3(1.0f), glm::vec3(0.0f))
                    .addInitialVelocity(std::make_unique<RangeDistribution<glm::vec3>>(glm::vec3(-0.05f, 0.01f, -0.05f), glm::vec3(0.05f)))
                    .addInitialAcceleration(std::make_unique<RangeDistribution<glm::vec3>>(glm::vec3(-0.01f, 0.2f, -0.01f), glm::vec3(0.01f, 0.2f, 0.01f)))
                    .addLifetime(std::make_unique<ConstDistribution<float>>(2.0f))
                    .setMaterial(materials.at("blink"))
                    .addInitialColor(std::make_unique<ConstDistribution<glm::vec4>>(glm::vec4(0.54f * 2.0f, 0.0f, 1.0f * 2.0f, 1.0f)))
                    .addColorByLife(std::make_unique<ConstDistribution<glm::vec4>>(glm::vec4(0.54f * 4.0f, 0.0f, 1.0f * 4.0f, 1.0f)))
                    .addCustomMaterial(std::make_unique<RangeDistribution<float>>(0.0f, 0.3f), std::make_unique<RangeDistribution<float>>(0.0f, 2.5f), std::make_unique<RangeDistribution<float>>(0.0, 5.0f), nullptr)
                    .addCustomMaterialByLife(std::make_unique<ConstDistribution<float>>(1.0f), std::make_unique<ConstDistribution<float>>(10.0f), nullptr, nullptr)
                .createEmitter<SpriteEmitter>("smoke_black")
                    .setSpawnMode(::EmitterSpawn::Mode::Burst)
                    .setBurstCount(std::make_unique<ConstDistribution<uint32_t>>(50))
                    .setMaxCount(50)
                    .setSpawnRate(50.0f)
                    .addInitialSize(std::make_unique<RangeDistribution<float>>(64.0f, 128.0f))
                    .addSizeByLife(std::make_unique<ConstDistribution<float>>(256.0f))
                    .addInitialRotation(std::make_unique<RangeDistribution<glm::vec3>>(glm::vec3(0.0f), glm::vec3(6.28f)))
                    .addInitialLocation(std::make_unique<RangeDistribution<glm::vec3>>(glm::vec3(-0.5f), glm::vec3(0.5f)))
                    .addLifetime(std::make_unique<ConstDistribution<float>>(2.0f))
                    .setMaterial(materials.at("blink"))
                    .addInitialColor(std::make_unique<ConstDistribution<glm::vec4>>(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)))
                    .addCustomMaterial(std::make_unique<RangeDistribution<float>>(0.0f, 0.3f), std::make_unique<RangeDistribution<float>>(0.0f, 3.0f), std::make_unique<RangeDistribution<float>>(0.0, 5.0f), nullptr)
                    .addCustomMaterialByLife(std::make_unique<ConstDistribution<float>>(1.0f), std::make_unique<ConstDistribution<float>>(10.0f), nullptr, nullptr)
                .build();
    }

    {
        const fs::path assets_dir{ENGINE_ASSETS_DIR};
        Material::builder()
                .name("shield")
                .shading(::Shading::Unlit)
                .blending(::Blending::Additive)
                .model(::InstanceType::Effect)
                .color(glm::vec4(1.0f))
                .custom("maintexture", TextureLoader::load(*this, assets_dir / "textures/shield_texture.jpg"))
                .custom("noise", TextureLoader::load(*this, assets_dir / "textures/noise.png"))
                .custom("vertex_offset_freq", 2.0f)
                .custom("vertex_offset_dir", glm::vec3(0.2))
                .fragment("vec2 uv = getVertexUV();"
                            "float fres = fresnel(getVertexNormal(), getCameraPosition() - getVertexPosition(), 3.5);"
                            "mctx.color.rgb *= 1.0 - texture(maintexture, uv).r;"
                            "mctx.color.rgb *= fres * vec3(33.0 / 255.0 * 15.0f, 99.0 / 255.0 * 15.0f, 1000.0 / 255.0 * 25.0f);"
                )
                .vertex(
                        "vertex_position.xyz += sin(getParticleTime() * vertex_offset_freq) * getVertexNormal() * vertex_offset_dir *texture(noise, getParticleTime() + uv).r;")

                .global("#include \"../functions/fresnel.glsl\"")
                .build(*this);
    }

    {
        EffectBuilder builder {*this};
        builder .create("shield")
                .createEmitter<MeshEmitter>("shield")
                    .addInitialSize(std::make_unique<ConstDistribution<glm::vec3>>(glm::vec3(0.5f)))
                    .setMaterial(materials.at("shield"))
                    .setMesh(meshes.at("sphere"))
                    .addTime()
                    .setMaxCount(1)
                .build();
    }

    {
        const fs::path assets_dir {ENGINE_ASSETS_DIR};
        Material::builder()
                .name("fireball_material")
                .fragment("vec2 uv = getVertexUV();"
                                    "uv.y -= 0.1;\n"
                                    "vec2 panned = vec2(uv.x + getParticleTime() * 0.5 + getParticleProperties().x, "
                                    "uv.y + getParticleTime() * 0.8 + getParticleProperties().y);\n"
                                    "uv += texture(noise, panned).g * 0.3;\n"
                                    "\n"
                                    "vec2 offset_panned1 = vec2(uv.x + getParticleTime() * 0.66, uv.y + getParticleTime() * 0.33);\n"
                                    "float offset1 = texture(noise, offset_panned1).r;\n"
                                    "\n"
                                    "vec2 offset_panned2 = vec2(uv.x + getParticleTime() * 0.45, uv.y + getParticleTime() * 0.71);\n"
                                    "float offset2 = texture(noise, offset_panned2).r;\n"
                                    "\n"
                                    "float r = offset1 * offset2;\n"
                                    "\n"
                                    "mctx.color.rgb = getMaterialDiffuse(uv).rgb;\n"
                                    "mctx.color.rgb *= clamp((1.0 - getParticleProperties().z) * r, 0, 1);"
                )
                .custom("noise", TextureLoader::load(*this, assets_dir /"textures/true_noise.tga"))
                .diffuse( TextureLoader::load(*this, assets_dir / "textures/true_fire.tga"))
                .color(glm::vec4(1.0f))
                .shading(Shading::Unlit)
                .blending(Blending::Additive)
                .global("#include \"../functions/tone_mapping.glsl\"")
                .model(::InstanceType::Effect)
                .build(*this);

        Material::builder().name("fireball_sparks")
                .fragment("mctx.emissive_color *= circle(getVertexUV(), 0.7);"
                                    "mctx.color.rgb *= circle(getVertexUV(), 0.7);")
                .global("#include \"../functions/circle.glsl\"")

                .emissive_color( glm::vec4{5.0f, 1.5f, 1.0f, 1.0f})
                .color(glm::vec4(1.0))
                .shading(Shading::Unlit)
                .blending(Blending::Additive)
                .model(::InstanceType::Effect)
                .build(*this);
    }
    {
        EffectBuilder builder {*this};
        builder.create("fireball")
                .createEmitter<::SpriteEmitter>("fire")
                    .addInitialRotation(std::make_unique<RangeDistribution<glm::vec3>>(glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{6.28f, 0.0f, 0.0f}))
                    .addLifetime(std::make_unique<RangeDistribution<float>>(0.5f, 0.7f))
                    .addInitialSize(std::make_unique<ConstDistribution<float>>(512.0f))
                    .addSizeByLife(std::make_unique<ConstDistribution<float>>(1024.0f))
                    .addCustomMaterial(std::make_unique<RangeDistribution<float>>(0.0f, 0.9f), std::make_unique<RangeDistribution<float>>(0.0f, 0.9f), std::make_unique<ConstDistribution<float>>(0.0f), nullptr)
                    .addCustomMaterialByLife(nullptr, nullptr, std::make_unique<ConstDistribution<float>>(1.0f), nullptr)
                    .setMaterial(materials.at("fireball_material"))
                    .setSpawnMode(::EmitterSpawn::Mode::Spray)
                    .addInitialVelocity(std::make_unique<RangeDistribution<glm::vec3>>(glm::vec3(1.0, -1.0, -1.0), glm::vec3(2.0, 1.0, 1.0)))
                    .setMaxCount(1000)
                    .setSpawnRate(200.0f)
                .createEmitter<SpriteEmitter>("sparks")
                    .addLifetime(std::make_unique<RangeDistribution<float>>(0.5f, 0.7f))
                    .addInitialSize(std::make_unique<ConstDistribution<float>>(30.0f))
                    .addSizeByLife(std::make_unique<ConstDistribution<float>>(0.0f))
                    .setMaterial(materials.at("fireball_sparks"))
                    .addInitialVelocity(std::make_unique<RangeDistribution<glm::vec3>>(glm::vec3(1.0, -1.0, -1.0), glm::vec3(2.0, 1.0, 1.0)))
                    .addInitialColor(std::make_unique<RangeDistribution<glm::vec4>>(glm::vec4(1.5, 1.0, 1.0, 1.0), glm::vec4(2.5, 2.5, 2.5, 2.5)))
                    .addColorByLife(std::make_unique<ConstDistribution<glm::vec4>>(glm::vec4(1.5, 0.5, 0.0, 1.0)))
                    .addInitialMeshLocation(meshes.at("sphere"), glm::vec3(0.5f), glm::vec3(0.0f))
                    .setMaxCount(200)
                    .setSpawnRate(200.0f)
                .build();
    }

    {
        Material::builder()
                .name("explosion")
                .shading(Shading::Unlit)
                .blending(Blending::Additive)
                .emissive_color(glm::vec4(2.0, 1.0, 0.3, 1.0))
                .color( glm::vec4(1.0, 1.0, 0.3, 1.0))
                .fragment("mctx.emissive_color *= circle(getVertexUV(), 0.7) * getParticleColor().rgb;"
                                    "mctx.color.rgb *= circle(getVertexUV(), 0.7);")
                .global("#include \"../functions/circle.glsl\"")
                .model(InstanceType::Effect)
                .build(*this);
    }

    {
        EffectBuilder builder{*this};
        builder.create("explosion")
                .createEmitter<SpriteEmitter>("generate")
                    .addInitialVelocity(std::make_unique<RangeDistribution<glm::vec3>>(glm::vec3{-1.0f, 0.0f, -1.0f}, glm::vec3{1.0f}))
                    .addInitialAcceleration(std::make_unique<RangeDistribution < glm::vec3>>(glm::vec3{0.0f}, glm::vec3{0.0f, 5.0f, 0.0f}))
                    .addLifetime(std::make_unique<RangeDistribution<float>>(0.2f, 1.0f))
                    .addInitialSize(std::make_unique<RangeDistribution<float>>(1.0f, 25.0f))
                    .addSizeByLife(std::make_unique<ConstDistribution<float>>(0.0f))
                    .addInitialColor(std::make_unique<RangeDistribution<glm::vec4>>(glm::vec4{0.0f}, glm::vec4{2.0f}))
                    .setMaterial(materials.at("explosion"))
                    .setSpawnMode(EmitterSpawn::Mode::Burst)
                    .setBurstCount(std::make_unique<ConstDistribution<uint32_t>>(250))
                    .setMaxCount(250)
                    .setSpawnRate(1.0f)
                    .setLocalSpace(true)
                .build();
    }

    {
        Material::builder().name("hurricane")
                .blending(Blending::Additive)
                .emissive_color(glm::vec4(5.0, 0.0, 5.0, 1.0))
                .color(glm::vec4(1.0, 0.0, 1.0, 1.0))
                .fragment("mctx.emissive_color *= circle(getVertexUV(), 0.5);"
                                    "mctx.color.rgb = vec3(0.0);")
                .global("#include \"../functions/circle.glsl\"")
                .model(InstanceType::Effect)
                .shading(Shading::Unlit)
                .build(*this);
    }

    {
        EffectBuilder builder{*this};
        builder .create("hurricane")
                .createEmitter<SpriteEmitter>("generate")
                    .addInitialVelocity(std::make_unique<RangeDistribution<glm::vec3>>(glm::vec3{-1.0f, 0.5f, -1.0f}, glm::vec3{1.0f}))
                    .addInitialAcceleration(std::make_unique<RangeDistribution<glm::vec3>>(glm::vec3{0.0f}, glm::vec3{0.0f, 5.0f, 0.0f}))
                    .addLifetime(std::make_unique<RangeDistribution<float>>(0.5f, 1.0f))
                    .addInitialSize(std::make_unique<RangeDistribution<float>>(5.0f, 25.0f))
                    .addSizeByLife(std::make_unique<ConstDistribution <float>>(0.0f))
                    .setMaterial(materials.at("hurricane"))
                    .setSpawnMode(EmitterSpawn::Mode::Spray)
                    .addInitialMeshLocation(meshes.at("sphere"), glm::vec3(1.0f), glm::vec3(0.0f))
                    .setMaxCount(1000)
                    .setSpawnRate(1000.0f)
                    .setLocalSpace(true)
                .build();
    }

    {
        Material::builder()
                .name("drop")
                .emissive_color( glm::vec4(0.3f, 1.5f, 0.5f, 1.0f))
                .color(glm::vec4(0.3f, 1.5f, 0.5f, 1.0f))
                .fragment("mctx.emissive_color *= circle(getVertexUV(), 0.7) * getParticleColor().rgb;"
                                    "mctx.color.rgb *= circle(getVertexUV(), 0.7) * getParticleColor().rgb;")
                .shading(Shading::Unlit)
                .blending(Blending::Additive)
                .global("#include \"../functions/circle.glsl\"")
                .model(InstanceType::Effect)
                .build(*this);
    }

    {
        EffectBuilder builder{*this};
        builder.create("modeldrop")
                .createEmitter<SpriteEmitter>("sparks")
                    .addLifetime(std::make_unique<ConstDistribution<float>>(0.8f))
                    .addInitialSize(std::make_unique<ConstDistribution<float>>(5.0f))
                    .addInitialVelocity(std::make_unique<RangeDistribution<glm::vec3>>(glm::vec3(-0.15f, -0.025f, -0.15f), glm::vec3(0.15f, -0.15f, 0.15f)))
                    .addInitialAcceleration(std::make_unique<RangeDistribution<glm::vec3>>(glm::vec3(-0.15f, 0.05f, -0.15f), glm::vec3(0.15f, 0.1f, 0.15f)))
                    .addInitialColor(std::make_unique<RangeDistribution<glm::vec4>>(glm::vec4{1.0f, 0.5f, 0.8f, 1.0f}, glm::vec4{2.0f, 1.0f, 1.5f, 1.0f}))
                    .addInitialMeshLocation(dynamic_cast<SkeletalModel&>(*models.at("bob")).getMeshes().at(3))
                    .setMaterial(materials.at("drop"))
                    .setMaxCount(1000)
                    .setSpawnRate(1000.0f)
                .build();
    }

    {

        Material::builder()
                .name("beam_lightning")
                .emissive_color( glm::vec4(2.8f, 2.8f, 4.8f, 1.0f))
                .fragment("mctx.emissive_color *= getParticleColor().rgb;")
                .shading(Shading::Unlit)
                .blending(Blending::Opaque)
                .model(InstanceType::Effect)
                .build(*this);
    }

    {
        EffectBuilder builder{*this};
        builder.create("lightning")
                .createEmitter<BeamEmitter>("beam")
                    .setSpawnMode(::EmitterSpawn::Mode::Burst)
                    .setBurstCount(std::make_unique<ConstDistribution < uint32_t>>(15))
                    .setMaxCount(15)
                    .addTime()
                    .setSpawnRate(15.0f)
                    .addInitialColor(std::make_unique<RangeDistribution<glm::vec4>>(glm::vec4(1.0f), glm::vec4(2.0f)))
                    .addLifetime(std::make_unique<ConstDistribution<float>>(5.0f))
                    .addInitialSize(std::make_unique<RangeDistribution<float>>(6.0f, 8.0f))
                    .addBeamInitialTarget(std::make_unique<ConstDistribution<glm::vec3>>(glm::vec3{29.0f, 3.0f, 16.0f}))
                    .addBeamInitialDisplacement(std::make_unique<RangeDistribution<float>>(0.2f, 0.4f))
                    .addBeamInitialOffset(std::make_unique<RangeDistribution<float>>(0.1f, 0.2f))
                    .addBeamInitialRebuild(std::make_unique<RangeDistribution<float>>(0.2f, 0.3f))
                    .setMaterial(materials.at("beam_lightning"))
                    .addBeamSpeed(std::make_unique<RangeDistribution<float>>(0.2f, 0.3f))
                .build();
    }

    {
        Material::builder()
                .name("skeleton4ik")
                .emissive_color( glm::vec4(0.3f, 1.5f, 0.5f, 1.0f))
                .color(glm::vec4(0.3f, 1.5f, 0.5f, 1.0f))
                .fragment("mctx.emissive_color *= circle(getVertexUV(), 0.7);"
                                 "mctx.color.rgb *= circle(getVertexUV(), 0.7);")
                .shading(Shading::Unlit)
                .blending(Blending::Additive)
                .global("#include \"../functions/circle.glsl\"")
                .model(InstanceType::Effect)
                .build(*this);
    }

    {
        EffectBuilder builder{*this};
        builder.create("skeleton")
                .createEmitter<SpriteEmitter>("sparks")
                    .addLifetime(std::make_unique<ConstDistribution < float>>(0.8f))
                    .addInitialSize(std::make_unique<ConstDistribution < float>>(5.0f))
                    .addInitialMeshLocation(models.at("skeleton"), glm::vec3(0.025f), glm::vec3(-M_PI_2, 0.0f, M_PI_2))
                    .setMaterial(materials.at("skeleton4ik"))
                    .setMaxCount(4000)
                    .setSpawnRate(4000.0f)
                .build();
    }

    {
        const fs::path assets_dir {ENGINE_ASSETS_DIR};
        Material::builder()
                .name("aura")
                .shading(::Shading::Unlit)
                .blending(::Blending::Additive)
                .two_sided(true)
                .diffuse( TextureLoader::load(*this, assets_dir / "textures/aura.png"))
                .model(::InstanceType::Effect)
                .build(*this);

        Material::builder().name("aura_sparks")
                .shading(::Shading::Unlit)
                .blending(::Blending::Additive)
                .two_sided(true)
                .blend_mask(TextureLoader::load(*this, assets_dir / "textures/pirate.jpg", {
                        TextureLoaderFlags::Origin::TopLeft,
                        TextureLoaderFlags::Filter::Nearest
                }))
                .emissive_color( glm::vec4(3.5f, 0.0f, 0.0f, 1.0f))
                .model(::InstanceType::Effect)
                .build(*this);

        Material::builder().name("aura_beam")
                .shading(::Shading::Unlit)
                .blending(::Blending::Additive)
                .two_sided(true)
                .emissive_color( glm::vec4(3.5f, 0.0f, 0.0f, 1.0f))
                .model(::InstanceType::Effect)
                .build(*this);
    }

    {
        EffectBuilder builder{*this};
        builder.create("aura")
                .createEmitter<MeshEmitter>("aura")
                    .setMesh(meshes.at("plane"))
                    .setMaterial(materials.at("aura"))
                    .setLocalSpace(true)
                    .setSpawnMode(::EmitterSpawn::Mode::Spray)
                    .setMaxCount(1)
                    .setSpawnRate(5.0f)
                    .addInitialSize(std::make_unique<ConstDistribution < glm::vec3>>(glm::vec3(2.0f)))
                    .addRotationRate(std::make_unique<ConstDistribution < glm::vec3>>(glm::vec3(0.0f, 1.0f, 0.0f)))
                    .createEmitter<SpriteEmitter>("sparks")
                    .addLifetime(std::make_unique<ConstDistribution < float>>(1.5f))
                    .addInitialSize(std::make_unique<ConstDistribution < float>>(100.0f))
                    .addInitialVelocity(std::make_unique<RangeDistribution < glm::vec3>>(glm::vec3(0.0f, 0.1f, 0.0f), glm::vec3(0.0f, 0.5f, 0.0f)))
                    .addInitialLocation(std::make_unique<RangeDistribution < glm::vec3>>(glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(1.0f, 0.0f, 1.0f)))
                    .setMaterial(materials.at("aura_sparks"))
                    .setMaxCount(20)
                    .setSpawnRate(15.0f)
                .createEmitter<BeamEmitter>("beam")
                    .setSpawnMode(::EmitterSpawn::Mode::Spray)
                    .setMaxCount(10)
                    .addTime()
                    .setSpawnRate(10.0f)
                    .addLifetime(std::make_unique<RangeDistribution < float>>(1.0f, 2.0f))
                    .addInitialSize(std::make_unique<ConstDistribution < float>>(10.0f))
                    .addInitialLocation(std::make_unique<RangeDistribution < glm::vec3>>(glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(1.0f, 0.0f, 1.0f)))
                    .addBeamInitialTarget(std::make_unique<RangeDistribution < glm::vec3>>(glm::vec3(22.5f, 2.0f, 3.5f), glm::vec3(23.5f, 2.0f, 4.5f)))
                    .addBeamInitialDisplacement(std::make_unique<RangeDistribution < float>>(0.2f, 0.4f))
                    .addBeamInitialOffset(std::make_unique<RangeDistribution < float>>(0.1f, 0.2f))
                    .addBeamInitialRebuild(std::make_unique<RangeDistribution < float>>(0.2f, 0.3f))
                    .setMaterial(materials.at("aura_beam"))
                    .addBeamSpeed(std::make_unique<RangeDistribution < float>>(0.2f, 0.3f))
                .build();
    }
}

void DemoAssets::loadModelsScene() {
    {
        const fs::path assets_dir{ENGINE_ASSETS_DIR};

        models.add("thanos", ModelLoader::loadModel(*this, assets_dir / "models/thanos/thanos.fbx"));

        auto& thanos = dynamic_cast<SkeletalModel&>(*models.at("thanos"));

        auto& materials = thanos.getMaterials();

        auto yellowenv = 
                Material::builder()
                    .name("yellowenv")
                    .color(glm::vec4(0.546171f, 0.532152f, 0.546140f, 1.0f))
                    .emissive_color( glm::vec4(0.546171f, 0.532152f, 0.546140f, 1.0f))
                    .diffuse( TextureLoader::load(*this, assets_dir / "models/thanos/bodyyellow_m.tga"))
                    .normal( TextureLoader::load(*this, assets_dir / "models/thanos/body_n.tga"))
                    .custom("emissive_map", TextureLoader::load(*this, assets_dir / "models/thanos/yellow_env.tga"))
                    .fragment("mctx.emissive_color = texture(emissive_map, getVertexUV()).rgb;")
                    .shading(Shading::Lit)
                    .model(InstanceType::Skeletal)
                    .build(*this);

        auto grayenv = Material::builder()
                .name("grayenv")
                .color(glm::vec4(0.636099f, 0.753402f, 0.750975f, 1.0f))
                .emissive_color( glm::vec4(0.636099f, 0.753402f, 0.750975f, 1.0f))
                .diffuse( TextureLoader::load(*this, assets_dir / "models/thanos/bodygray_m.tga"))
                .normal( TextureLoader::load(*this, assets_dir / "models/thanos/body_n.tga"))
                .custom("emissive_map", TextureLoader::load(*this, assets_dir / "models/thanos/gray_env.tga"))
                .fragment("mctx.emissive_color = texture(emissive_map, getVertexUV()).rgb;")
                .shading(Shading::Lit)
                .model(InstanceType::Skeletal)
                .build(*this);

        auto eyesenv = Material::builder().name("eyesenv")
                .color(glm::vec4(0.539415f, 0.560208f, 0.510821f, 1.0f))
                .emissive_color( glm::vec4(1.0f))
                .diffuse( TextureLoader::load(*this, assets_dir / "models/thanos/body_d.tga"))
                .normal( TextureLoader::load(*this, assets_dir / "models/thanos/body_n.tga"))
                .custom("emissive_map", TextureLoader::load(*this, assets_dir / "models/thanos/eye_env.tga"))
                .fragment("mctx.emissive_color = texture(emissive_map, getVertexUV()).rgb;")
                .shading(Shading::Lit)
                .model(InstanceType::Skeletal)
                .build(*this);

        auto blueenv = Material::builder()
                .name("blueenv")
                .color(glm::vec4(0.576473f, 0.481502f, 0.448319f, 1.0f))
                .emissive_color( glm::vec4(1.0f))
                .diffuse( TextureLoader::load(*this, assets_dir / "models/thanos/bodyblue_m.tga"))
                .normal( TextureLoader::load(*this, assets_dir / "models/thanos/body_n.tga"))
                .custom("emissive_map", TextureLoader::load(*this, assets_dir / "models/thanos/blue_env.tga"))
                .fragment("mctx.emissive_color = texture(emissive_map, getVertexUV()).rgb;")
                .shading(Shading::Lit)
                .model(InstanceType::Skeletal)
                .build(*this);

        auto eyes = Material::builder()
                .name("eyes")
                .color(glm::vec4(0.551182f, 0.681717f, 0.402268f, 1.0f))
                .diffuse( TextureLoader::load(*this, assets_dir / "models/thanos/body_d.tga"))
                .normal( TextureLoader::load(*this, assets_dir / "models/thanos/body_n.tga"))
                .metallic(TextureLoader::load(*this, assets_dir / "models/thanos/body_s.tga"))
                .roughness(0.2f)
                .shading(Shading::Lit)
                .model(InstanceType::Skeletal)
                .build(*this);

        auto body = Material::builder()
                .name("body")
                .color(glm::vec4(0.566327f, 0.533939f, 0.783312f, 1.0f))
                .diffuse( TextureLoader::load(*this, assets_dir / "models/thanos/body_d.tga"))
                .normal( TextureLoader::load(*this, assets_dir / "models/thanos/body_n.tga"))
                .metallic( TextureLoader::load(*this, assets_dir / "models/thanos/body_s.tga"))
                .roughness( 0.2f)
                .ao(TextureLoader::load(*this, assets_dir / "models/thanos/body_ao.tga"))
                .shading(Shading::Lit)
                .model(InstanceType::Skeletal)
                .build(*this);

        auto skin = Material::builder()
                .name("skin")
                .diffuse(TextureLoader::load(*this, assets_dir / "models/thanos/body_d.tga"))
                .normal( TextureLoader::load(*this, assets_dir / "models/thanos/body_n.tga"))
                .metallic( TextureLoader::load(*this, assets_dir / "models/thanos/body_s.tga"))
                .roughness( 0.2f)
                .ao( TextureLoader::load(*this, assets_dir / "models/thanos/skin_ao.tga"))
                .shading(Shading::Lit)
                .model(InstanceType::Skeletal)
                .build(*this);

        materials[0] = skin;
        materials[1] = body;
        materials[2] = eyes;
        materials[3] = blueenv;
        materials[4] = eyesenv;
        materials[5] = grayenv;
        materials[6] = skin;
    }

    {
        const fs::path assets_dir{ENGINE_ASSETS_DIR};

        models.add("daenerys", ModelLoader::loadModel(*this, assets_dir / "models/daenerys/daenerys.fbx"));

        auto& daenerys = dynamic_cast<SkeletalModel&>(*models.at("daenerys"));

        auto& materials = daenerys.getMaterials();

        auto head = Material::builder()
                .name("dae_head")
                .color( glm::vec4(0.657693f, 0.475201f, 0.647254f, 1.0f))
                .diffuse(TextureLoader::load(*this, assets_dir / "models/daenerys/tex_commander1_daenerys_face_d.png"))
                .normal(TextureLoader::load(*this, assets_dir / "models/daenerys/tex_commander1_daenerys_face_n.png"))
                .metallic(TextureLoader::load(*this, assets_dir / "models/daenerys/tex_commander1_daenerys_face_m.png"))
                .roughness( 0.2f)
                .ao(TextureLoader::load(*this, assets_dir / "models/daenerys/tex_commander1_daenerys_face_L.png"))
                .shading(Shading::Lit)
                .model(InstanceType::Skeletal)
                .build(*this);

        auto cloth = Material::builder()
                .name("dae_cloth")
                .diffuse(TextureLoader::load(*this, assets_dir / "models/daenerys/tex_commander1_daenerys_cloth_d.png"))
                .normal(TextureLoader::load(*this, assets_dir / "models/daenerys/tex_commander1_daenerys_cloth_ns.png"))
                .metallic(TextureLoader::load(*this, assets_dir / "models/daenerys/tex_commander1_daenerys_cloth_m.png"))
                .roughness( 0.2f)
                .ao(TextureLoader::load(*this, assets_dir / "models/daenerys/tex_commander1_daenerys_cloth_L.png"))
                .shading(Shading::Lit)
                .model(InstanceType::Skeletal)
                .build(*this);

        auto hair = Material::builder()
                .name("dae_hair")
                .color( glm::vec4(0.657236f, 0.567879f, 0.703609f, 1.0f))
                .diffuse(TextureLoader::load(*this, assets_dir / "models/daenerys/tex_commander1_daenerys_hair_d.png"))
                .normal(TextureLoader::load(*this, assets_dir / "models/daenerys/tex_commander1_daenerys_hair_n.png"))
                .metallic(TextureLoader::load(*this, assets_dir / "models/daenerys/jitter2.png"))
                .roughness( 0.2f)
                .shading(Shading::Lit)
                .model(InstanceType::Skeletal)
                .build(*this);

        materials[0] = hair;
        materials[1] = cloth;
        materials[2] = head;
    }

    {
        const fs::path assets_dir{ENGINE_ASSETS_DIR};

        models.add("skeleton", ModelLoader::loadModel(*this, assets_dir / "models/skeleton/skeleton.fbx", {{
           ModelLoaderOption::NoMaterials
       }}));

        auto& materials = dynamic_cast<SkeletalModel&>(*models.at("skeleton")).getMaterials();
        materials.resize(1);

        auto skin = Material::builder()
                .name("skeleton")
                .diffuse(TextureLoader::load(*this, assets_dir / "models/skeleton/Skeleton_Body.png"))
                .ao(TextureLoader::load(*this, assets_dir / "models/skeleton/Skeleton_Body_OcclusionRoughnessMetallic.png"))
                .custom("ao_r_m", TextureLoader::load(*this, assets_dir / "models/skeleton/Skeleton_Body_OcclusionRoughnessMetallic.png"))
                .fragment("vec3 skin = texture(ao_r_m, getVertexUV()).rgb;"
                                    "mctx.ao = skin.r;"
                                    "mctx.roughness = skin.g;"
                                    "mctx.metallic = skin.b;")
                .shading(Shading::Lit)
                .model(InstanceType::Skeletal)
                .build(*this);

        materials[0] = skin;
    }

    {
        const fs::path assets_dir{ENGINE_ASSETS_DIR};
        models.add("taskmaster", ModelLoader::loadModel(*this, assets_dir / "models/taskmaster/Taskmaster.fbx"));
    }

    {
        const fs::path assets_dir{ENGINE_ASSETS_DIR};

        models.add("k2", ModelLoader::loadModel(*this, assets_dir / "models/k2/k2.fbx"));

        auto& k2 = dynamic_cast<SkeletalModel&>(*models.at("k2"));

        auto& materials = k2.getMaterials();

        auto body = Material::builder()
                .name("k2_body")
                .diffuse( TextureLoader::load(*this, assets_dir / "models/k2/K2_D.tga"))
                .normal( TextureLoader::load(*this, assets_dir / "models/k2/K2_D.tga"))
                .metallic( TextureLoader::load(*this, assets_dir / "models/k2/K2_S.tga"))
                .roughness( 0.2f)
                .shading(Shading::Lit)
                .model(InstanceType::Skeletal)
                .build(*this);

        auto head = Material::builder()
                .name("k2_head")
                .diffuse( TextureLoader::load(*this, assets_dir / "models/k2/K2_Head_D.tga"))
                .normal( TextureLoader::load(*this, assets_dir / "models/k2/K2_Head_N.tga"))
                .metallic( TextureLoader::load(*this, assets_dir / "models/k2/K2_Head_S.tga"))
                .roughness( 0.2f)
                .shading(Shading::Lit)
                .model(InstanceType::Skeletal)
                .build(*this);

        materials[0] = head;
        materials[1] = body;
    }

    {
        const fs::path assets_dir{ENGINE_ASSETS_DIR};

        models.add("elemental", ModelLoader::loadModel(*this, assets_dir / "models/elemental/elemental.fbx"));

        auto& elemental = dynamic_cast<SkeletalModel&>(*models.at("elemental"));

        auto& materials = elemental.getMaterials();

        auto body = Material::builder()
                .name("elemental_body")
                .diffuse( TextureLoader::load(*this, assets_dir / "models/elemental/Textures/Albedo FireElemental.png"))
                .emissive_color(glm::vec4(1.0f))
                .normal( TextureLoader::load(*this, assets_dir / "models/elemental/Textures/Normal FireElemental.png"))
                .roughness(TextureLoader::load(*this, assets_dir / "models/elemental/Textures/Rougness FireElemental.png"))
                .metallic(0.1f)
                .custom("emissive_map", TextureLoader::load(*this, assets_dir / "models/elemental/Textures/Emission FireElemental.png"))
                .fragment("mctx.emissive_color = texture(emissive_map, getVertexUV()).rgb;")
                .shading(Shading::Lit)
                .build(*this);

        materials[0] = body;
    }
    
    {
        const fs::path assets_dir {getAssetsDir()};

        models.add("bob", ModelLoader::loadModel(*this, assets_dir / "models/boblamp/boblampclean.md5mesh"));
        models.add("backpack", ModelLoader::loadModel(*this, assets_dir / "models/backpack/backpack.obj", {{
               ::ModelLoaderOption::FlipUV
       }}));
        models.add("cyborg", ModelLoader::loadModel(*this, assets_dir / "models/cyborg/cyborg.obj"));
        models.add("drone", ModelLoader::loadModel(*this, assets_dir / "models/drone/model/BusterDrone.fbx"));
    }
}

void DemoAssets::loadSponzaScene() {
//    const fs::path assets_dir {getAssetsDir()};
//    models.add("sponza", ModelLoader::loadModel(*this, assets_dir / "models/sponza/sponza.obj"));
}

void DemoAssets::loadAssets() {
    loadMaterialsScene();
    loadModelsScene();
    loadLightingScene();
    loadEffectsScene();
//    loadSponzaScene();

    {
        const fs::path assets_dir{getAssetsDir()};

        skyboxes.add("skybox", std::make_shared<Skybox>(*this, assets_dir / "skyboxes/sky/sky.png", TextureLoaderFlags {
            TextureLoaderFlags::Origin::TopLeft,
            TextureLoaderFlags::Space::sRGB
        }));
        fonts.add("nunito", std::make_shared<FontAtlas>(assets_dir / "fonts/nunito.ttf", 48));
    }
}

DemoAssets::DemoAssets(::Context& ctx, ::Renderer& renderer, const fs::path& path)
    : ::Assets(path) {
    ::Assets::load(ctx);

    {
        const fs::path assets_dir{getAssetsDir()};

        ctx.setWindowIcon(TextureLoader::loadGLFWImage(*this, assets_dir / "icons/demo.png"));

        loadAssets();

        compileAssets(ctx, renderer.getSettings());
    }
}
