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

using namespace Limitless::ms;
using namespace Limitless::fx;
using namespace Limitless;

void DemoAssets::loadLightingScene() {
    {
        MaterialBuilder builder {*this};
        const fs::path assets_dir {ASSETS_DIR};
        builder.setName("PBR")
                .add(Property::MetallicTexture, TextureLoader::load(*this, assets_dir / "textures/rustediron2_metallic.png"))
                .add(Property::RoughnessTexture, TextureLoader::load(*this, assets_dir / "textures/rustediron2_roughness.png"))
                .add(Property::Diffuse, TextureLoader::load(*this, assets_dir / "textures/rustediron2_basecolor.png", {::TextureLoaderFlags::Space::sRGB}))
                .add(Property::Normal, TextureLoader::load(*this, assets_dir / "textures/rustediron2_normal.png"))
                .setShading(Shading::Lit)
                .addModelShader(::ModelShader::Instanced)
                .addModelShader(::ModelShader::Model)
                .build();
    }

    {
        MaterialBuilder builder {*this};
        const fs::path assets_dir {ASSETS_DIR};
        builder.setName("floor")
                .setShading(Shading::Lit)
                .add(Property::Diffuse, TextureLoader::load(*this, assets_dir / "textures/wood.jpg", {::TextureLoaderFlags::Space::sRGB}))
                .add(Property::Normal, TextureLoader::load(*this, assets_dir / "textures/wood_normal.png"))
                .setShading(Shading::Lit)
                .setTwoSided(true)
                .addModelShader(ModelShader::Model)
                .addModelShader(ModelShader::Instanced)
                .build();
    }
}

void DemoAssets::loadMaterialsScene() {
    MaterialBuilder builder {*this};
    const fs::path assets_dir {ASSETS_DIR};

    builder .setName("color")
            .add(Property::Color, glm::vec4(1.0f, 0.5f, 1.0f, 1.0f))
            .setShading(Shading::Unlit)
            .build();

    builder .setName("albedo")
            .add(Property::Diffuse, TextureLoader::load(*this, assets_dir / "textures/albedo.jpg", {
                    TextureLoaderFlags::Space::sRGB
            }))
            .setShading(Shading::Unlit)
            .build();

    builder .setName("emissive_color")
            .add(Property::EmissiveColor, glm::vec4(5.0f, 5.1f, 0.0f, 1.0f))
            .setShading(Shading::Unlit)
            .build();

    builder .setName("emissive_mask")
            .add(Property::EmissiveColor, glm::vec4(2.5f, 0.0f, 5.0f, 1.0f))
            .add(Property::Color, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))
            .add(Property::EmissiveMask, TextureLoader::load(*this, assets_dir / "textures/open_mask.jpg", {
                    TextureLoaderFlags::Filter::Nearest
            }))
            .setShading(Shading::Unlit)
            .setBlending(::Blending::Additive)
            .setTwoSided(true)
            .build();

    builder .setName("blend_mask")
            .add(Property::BlendMask, TextureLoader::load(*this, assets_dir / "textures/blend_mask.jpg", {
                    TextureLoaderFlags::Filter::Nearest
            }))
            .add(Property::Color, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f))
            .setShading(Shading::Unlit)
            .setTwoSided(true)
            .build();

    builder .setName("basic1")
            .add(Property::Metallic, 0.0f)
            .add(Property::RoughnessTexture, TextureLoader::load(*this, assets_dir / "textures/stonework_roughness.png"))
            .add(Property::Diffuse, TextureLoader::load(*this, assets_dir / "textures/stonework_albedo.png"))
            .add(Property::AmbientOcclusionTexture, TextureLoader::load(*this, assets_dir / "textures/stonework_ao.png"))
            .add(Property::Normal, TextureLoader::load(*this, assets_dir / "textures/stonework_normal.png"))
            .setShading(Shading::Lit)
            .build();

    builder .setName("basic2")
            .add(Property::MetallicTexture, TextureLoader::load(*this, assets_dir / "textures/metall_metallic.png"))
            .add(Property::RoughnessTexture, TextureLoader::load(*this, assets_dir / "textures/metall_roughness.png"))
            .add(Property::Diffuse, TextureLoader::load(*this, assets_dir / "textures/metall_albedo.png"))
            .add(Property::AmbientOcclusionTexture, TextureLoader::load(*this, assets_dir / "textures/metall_ao.png"))
            .add(Property::Normal, TextureLoader::load(*this, assets_dir / "textures/metall_normal.png"))
            .setShading(Shading::Lit)
            .build();

    builder .setName("basic3")
            .add(Property::MetallicTexture, TextureLoader::load(*this, assets_dir / "textures/green_metallic.png"))
            .add(Property::RoughnessTexture, TextureLoader::load(*this, assets_dir / "textures/green_roughness.png"))
            .add(Property::Diffuse, TextureLoader::load(*this, assets_dir / "textures/green_albedo.png"))
            .add(Property::AmbientOcclusionTexture, TextureLoader::load(*this, assets_dir / "textures/green_ao.png"))
            .add(Property::Normal, TextureLoader::load(*this, assets_dir / "textures/green_normal.png"))
            .setShading(Shading::Lit)
            .build();

    builder .setName("basic4")
            .add(Property::MetallicTexture, TextureLoader::load(*this, assets_dir / "textures/charcoal_metallic.png"))
            .add(Property::RoughnessTexture, TextureLoader::load(*this, assets_dir / "textures/charcoal_roughness.png"))
            .add(Property::Diffuse, TextureLoader::load(*this, assets_dir / "textures/charcoal_albedo.png"))
            .add(Property::Normal, TextureLoader::load(*this, assets_dir / "textures/charcoal_normal.png"))
            .setShading(Shading::Lit)
            .setTwoSided(true)
            .build();

    builder .setName("basic5")
            .add(Property::MetallicTexture, TextureLoader::load(*this, assets_dir / "textures/pirate_metallic.png"))
            .add(Property::RoughnessTexture, TextureLoader::load(*this, assets_dir / "textures/pirate_roughness.png"))
            .add(Property::Diffuse, TextureLoader::load(*this, assets_dir / "textures/pirate_albedo.png"))
            .add(Property::Normal, TextureLoader::load(*this, assets_dir / "textures/pirate_normal.png"))
            .setShading(Shading::Lit)
            .setTwoSided(true)
            .build();

    builder .setName("basic6")
            .add(Property::MetallicTexture, TextureLoader::load(*this, assets_dir / "textures/ornate_metallic.png"))
            .add(Property::RoughnessTexture, TextureLoader::load(*this, assets_dir / "textures/ornate_roughness.png"))
            .add(Property::Diffuse, TextureLoader::load(*this, assets_dir / "textures/ornate_albedo.png"))
            .add(Property::Normal, TextureLoader::load(*this, assets_dir / "textures/ornate_normal.png"))
            .setShading(Shading::Lit)
            .build();

    builder .setName("basic7")
            .add(Property::MetallicTexture, TextureLoader::load(*this, assets_dir / "textures/streaked_metallic.png"))
            .add(Property::RoughnessTexture, TextureLoader::load(*this, assets_dir / "textures/streaked_roughness.png"))
            .add(Property::Diffuse, TextureLoader::load(*this, assets_dir / "textures/streaked_albedo.png"))
            .add(Property::Normal, TextureLoader::load(*this, assets_dir / "textures/streaked_normal.png"))
            .setShading(Shading::Lit)
            .build();

    builder .setName("basic8")
            .add(Property::MetallicTexture, TextureLoader::load(*this, assets_dir / "textures/fabric_metallic.png"))
            .add(Property::RoughnessTexture, TextureLoader::load(*this, assets_dir / "textures/fabric_roughness.png"))
            .add(Property::Diffuse, TextureLoader::load(*this, assets_dir / "textures/fabric_albedo.png"))
            .add(Property::Normal, TextureLoader::load(*this, assets_dir / "textures/fabric_normal.png"))
            .setShading(Shading::Lit)
            .build();

    builder .setName("refraction")
            .setBlending(Blending::Translucent)
            .add(Property::Refraction, 1.03)
            .addUniform(std::make_unique<UniformTime>("time"))
            .addUniform(std::make_unique<UniformSampler>("noise", TextureLoader::load(*this, assets_dir / "textures/true_noise.tga")))
            .setFragmentSnippet("refraction = texture(noise, vec2(getVertexUV().x, getVertexUV().y + time * 0.1)).r;")
            .setShading(Shading::Unlit)
            .build();

    builder .setName("blending1")
            .add(Property::Color, glm::vec4(2.0f, 0.0f, 0.0f, 0.5f))
            .setShading(Shading::Unlit)
            .setBlending(Blending::Translucent)
            .setTwoSided(true)
            .build();

    builder .setName("blending2")
            .add(Property::Color, glm::vec4(0.0f, 2.0f, 0.0f, 0.4f))
            .setShading(Shading::Unlit)
            .setBlending(Blending::Translucent)
            .setTwoSided(true)
            .build();

    builder .setName("blending3")
            .add(Property::Color, glm::vec4(0.0f, 0.0f, 2.0f, 0.3f))
            .setShading(Shading::Unlit)
            .setBlending(Blending::Translucent)
            .setTwoSided(true)
            .build();

    builder .setName("blending4")
            .add(Property::Color, glm::vec4(1.0f, 0.5f, 0.5f, 1.0f))
            .setShading(Shading::Unlit)
            .setBlending(Blending::Additive)
            .setTwoSided(true)
            .build();

    builder .setName("blending5")
            .add(Property::Color, glm::vec4(2.0f, 0.3f, 0.3f, 1.0f))
            .setShading(Shading::Unlit)
            .setBlending(Blending::Modulate)
            .setTwoSided(true)
            .build();

    builder .setName("lava")
            .add(Property::EmissiveColor, glm::vec4(2.5f, 0.9f, 0.1f, 1.0f))
            .add(Property::Diffuse, TextureLoader::load(*this, assets_dir / "textures/lava.png", {
                    TextureLoaderFlags::Space::sRGB
            }))
            .addUniform(std::make_unique<UniformSampler>("noise", TextureLoader::load(*this, assets_dir / "textures/true_noise.tga")))
            .addUniform(std::make_unique<UniformTime>("time"))
            .setFragmentSnippet(
                    "vec2 panned = vec2(getVertexUV().x + time * 0.1, getVertexUV().y + time * 0.05);"
                    "vec2 uv = getVertexUV() + texture(noise, panned).r;"
                    "albedo.rgb = getMaterialDiffuse(uv).rgb;"
                    "emissive *= texture(noise, panned).g;"
            )
            .setShading(Shading::Unlit)
            .build();

    builder .setName("ice")
            .add(Property::EmissiveColor, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
            .add(Property::Diffuse, TextureLoader::load(*this, assets_dir / "textures/ice.jpg", {
                    TextureLoaderFlags::Space::sRGB
            }))
            .addUniform(std::make_unique<UniformSampler>("snow", TextureLoader::load(*this, assets_dir / "textures/snow.jpg")))
            .addUniform(std::make_unique<UniformTime>("time"))
            .setFragmentSnippet(
                    "vec2 uv = getVertexUV() + time * 0.05;"
                    "albedo.rgb += texture(snow, uv).rgb * abs(cos(time * 0.5));"
                    "emissive *= texture(snow, uv).r;"
            )
            .setShading(Shading::Unlit)
            .build();

    builder .setName("poison")
            .add(Property::EmissiveColor, glm::vec4(0.1f, 4.0f, 0.1f, 1.0f))
            .add(Property::Diffuse, TextureLoader::load(*this, assets_dir / "textures/poison.jpg", {
                    TextureLoaderFlags::Space::sRGB
            }))
            .addUniform(std::make_unique<UniformSampler>("noise", TextureLoader::load(*this, assets_dir / "textures/true_noise.tga")))
            .addUniform(std::make_unique<UniformTime>("time"))
            .setFragmentSnippet(
                    "vec2 panned = vec2(getVertexUV().x + time * 0.05, getVertexUV().y);"
                    "vec2 uv = vec2(getVertexUV().x, getVertexUV().y  + texture(noise, panned).g);"
                    "albedo.rgb = getMaterialDiffuse(uv).rgb;"
                    "emissive *= getMaterialDiffuse(uv).g;"
            )
            .setShading(Shading::Unlit)
            .build();

    builder.setName("fireball")
            .setVertexSnippet("vec2 uv_1 = vec2(uv.x + time * 0.05, uv.y + time * 0.0);\n"
                              "vec2 uv_2 = vec2(uv.x - time * 0.05, uv.y - time * 0.0);\n"
                              " \n"
                              "float s = texture(fire_mask, uv_1).r;\n"
                              "float t = texture(fire_mask, uv_2).r;\n"
                              "\n"
                              "vertex_position.xyz -= getVertexNormal() * texture(fire_mask, uv + vec2(s, t)).r * 0.6;")
            .setFragmentSnippet("vec2 uv = getVertexUV();"
                                "vec2 uv_1 = vec2(uv.x + time * 0.05, uv.y + time * 0.0);\n"
                                "vec2 uv_2 = vec2(uv.x - time * 0.05, uv.y - time * 0.0);\n"
                                " \n"
                                "float s = texture(fire_mask, uv_1).r;\n"
                                "float t = texture(fire_mask, uv_2).r;\n"
                                "\n"
                                "emissive = getMaterialDiffuse(uv + vec2(s, t)).rgb;\n"
                                "\n"
                                "if (emissive.r <= 0.2) discard;")
            .addUniform(std::make_unique<UniformTime>("time"))
            .addUniform(std::make_unique<UniformSampler>("fire_mask", TextureLoader::load(*this, assets_dir / "textures/fireball_mask.png")))
            .add(Property::Diffuse, TextureLoader::load(*this, assets_dir / "textures/rock_lava.png"))
            .add(Property::Color, glm::vec4{15.0f, 5.0f, 0.0f, 1.0f})
            .setShading(Shading::Unlit)
            .setBlending(Blending::Additive)
            .setTwoSided(true)
            .build();

    builder .setName("bump_mapping")
            .add(Property::MetallicTexture, TextureLoader::load(*this, assets_dir / "textures/brickwork_metallic.png"))
            .add(Property::RoughnessTexture, TextureLoader::load(*this, assets_dir / "textures/brickwork_roughness.png"))
            .add(Property::Diffuse, TextureLoader::load(*this, assets_dir / "textures/brickwork_albedo.png"))
            .add(Property::Normal, TextureLoader::load(*this, assets_dir / "textures/brickwork_normal.png"))
            .add(Property::AmbientOcclusionTexture, TextureLoader::load(*this, assets_dir / "textures/brickwork_ao.png"))
            .addUniform(std::make_unique<UniformSampler>("height", TextureLoader::load(*this, assets_dir / "textures/brickwork_height.png")))
            .setShading(Shading::Lit)
            .setVertexSnippet(
                    "vertex_position += getVertexNormal() * texture(height, getVertexUV()).xyz * 0.25;"
            )
            .build();

    builder .setName("fresnel")
            .add(::Property::EmissiveColor, glm::vec4(5.0, 0.7, 0.3, 1.0))
            .addUniform(std::make_unique<UniformTime>("time"))
            .setShading(Shading::Unlit)
            .add(Property::Diffuse, TextureLoader::load(*this, assets_dir / "textures/albedo.jpg", {
                    TextureLoaderFlags::Space::sRGB
            }))
            .setGlobalSnippet(
                    "#include \"../../functions/fresnel.glsl\""
            )
            .setFragmentSnippet(
                    "emissive *= fresnel(getFragmentNormal(), getCameraPosition() - getVertexPosition(), 5.0);"
            )
            .build();

    builder .setName("hue_shift")
            .addUniform(std::make_unique<UniformTime>("time"))
            .setShading(Shading::Unlit)
            .add(Property::Diffuse, TextureLoader::load(*this, assets_dir / "textures/albedo.jpg", {
                    TextureLoaderFlags::Space::sRGB
            }))
            .setGlobalSnippet(
                    "#include \"../../functions/hue_shift.glsl\""
            )
            .setFragmentSnippet(
                    "emissive = hue_shift(albedo.rgb, time);"
            )
            .build();
}

void DemoAssets::loadEffectsScene() {
    {
        MaterialBuilder builder {*this};
        const fs::path assets_dir {ENGINE_ASSETS_DIR};

        builder .setName("blink")
                .add(Property::Color, glm::vec4(1.0f))
                .addUniform(std::make_unique<UniformSampler>("mask", TextureLoader::load(*this, assets_dir / "textures/fireball_mask.png")))
                .add(Property::Diffuse, TextureLoader::load(*this, assets_dir / "textures/blink.jpg"))
                .setShading(Shading::Unlit)
                .setBlending(Blending::Translucent)
                .setTwoSided(true)
                .setFragmentSnippet("vec2 uv = getVertexUV();"
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
                                    "albedo.a = mix(0.0, m_a, pow(r, erode));"
                )
                .addModelShader(ModelShader::Effect)
                .build();
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
        MaterialBuilder builder {*this};
        const fs::path assets_dir{ENGINE_ASSETS_DIR};
        builder .setName("shield")
                .setShading(::Shading::Unlit)
                .setBlending(::Blending::Additive)
                .addModelShader(::ModelShader::Effect)
                .add(Property::Color, glm::vec4(1.0f))
                .addUniform(std::make_unique<UniformSampler>("maintexture", TextureLoader::load(*this, assets_dir / "textures/shield_texture.jpg")))
                .addUniform(std::make_unique<UniformSampler>("noise", TextureLoader::load(*this, assets_dir / "textures/noise.png")))
                .addUniform(std::make_unique<UniformValue<float>>("vertex_offset_freq", 2.0f))
                .addUniform(std::make_unique<UniformValue<glm::vec3>>("vertex_offset_dir", glm::vec3(0.2)))
                .setFragmentSnippet("vec2 uv = getVertexUV();"
                            "float fres = fresnel(getVertexNormal(), getCameraPosition() - getVertexPosition(), 3.5);"
                            "albedo.rgb *= 1.0 - texture(maintexture, uv).r;"
                            "albedo.rgb *= fres * vec3(33.0 / 255.0 * 15.0f, 99.0 / 255.0 * 15.0f, 1000.0 / 255.0 * 25.0f);"
                )
                .setVertexSnippet(
                        "vertex_position.xyz += sin(getParticleTime() * vertex_offset_freq) * getVertexNormal() * vertex_offset_dir *texture(noise, getParticleTime() + uv).r;")

                .setGlobalSnippet("#include \"../../functions/fresnel.glsl\"")
                .build();
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
        MaterialBuilder builder {*this};
        const fs::path assets_dir {ENGINE_ASSETS_DIR};
        builder .setName("fireball_material")
                .setFragmentSnippet("vec2 uv = getVertexUV();"
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
                                    "albedo.rgb = getMaterialDiffuse(uv).rgb;\n"
                                    "albedo.rgb *= clamp((1.0 - getParticleProperties().z) * r, 0, 1);"
                                    "albedo.rgb = toneMapping(albedo.rgb, 1.0);"
                )
                .addUniform(std::make_unique<UniformSampler>("noise", TextureLoader::load(*this, assets_dir /"textures/true_noise.tga")))
                .add(Property::Diffuse, TextureLoader::load(*this, assets_dir / "textures/true_fire.tga"))
                .setShading(Shading::Unlit)
                .setBlending(Blending::Additive)
                .setGlobalSnippet("#include \"../../functions/tone_mapping.glsl\"")
                .addModelShader(::ModelShader::Effect)
                .build();

        builder .setName("fireball_sparks")
                .setFragmentSnippet("emissive *= circle(getVertexUV(), 0.7);"
                                    "albedo.rgb *= circle(getVertexUV(), 0.7);")
                .setGlobalSnippet("#include \"../../functions/circle.glsl\"")

                .add(Property::EmissiveColor, glm::vec4{5.0f, 1.5f, 1.0f, 1.0f})
                .add(Property::Color, glm::vec4(1.0))
                .setShading(Shading::Unlit)
                .setBlending(Blending::Additive)
                .addModelShader(::ModelShader::Effect)
                .build();
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
        MaterialBuilder builder {*this};
        builder.setName("explosion")
                .setBlending(Blending::Additive)
                .add(::Property::EmissiveColor, glm::vec4(2.0, 1.0, 0.3, 1.0))
                .add(::Property::Color, glm::vec4(1.0, 1.0, 0.3, 1.0))
                .setFragmentSnippet("emissive *= circle(getVertexUV(), 0.7) * getParticleColor().rgb;"
                                    "albedo.rgb *= circle(getVertexUV(), 0.7);")
                .setGlobalSnippet("#include \"../../functions/circle.glsl\"")
                .addModelShader(ModelShader::Effect)
                .build();
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
        MaterialBuilder builder {*this};
        builder .setName("hurricane")
                .setBlending(Blending::Additive)
                .add(::Property::EmissiveColor, glm::vec4(5.0, 0.0, 5.0, 1.0))
                .add(::Property::Color, glm::vec4(1.0, 0.0, 1.0, 1.0))
                .setFragmentSnippet("emissive *= circle(getVertexUV(), 0.5);"
                                    "albedo.rgb = vec3(0.0);")
                .setGlobalSnippet("#include \"../../functions/circle.glsl\"")
                .addModelShader(ModelShader::Effect)
                .build();
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
        MaterialBuilder builder {*this};
        builder.setName("drop")
                .add(Property::EmissiveColor, glm::vec4(0.3f, 1.5f, 0.5f, 1.0f))
                .setFragmentSnippet("emissive *= circle(getVertexUV(), 0.7) * getParticleColor().rgb;"
                                    "albedo.rgb *= circle(getVertexUV(), 0.7) * getParticleColor().rgb;")
                .setShading(Shading::Unlit)
                .setBlending(Blending::Additive)
                .setGlobalSnippet("#include \"../../functions/circle.glsl\"")
                .addModelShader(ModelShader::Effect)
                .build();
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
        MaterialBuilder builder {*this};
        builder.setName("beam_lightning")
                .add(Property::EmissiveColor, glm::vec4(2.8f, 2.8f, 4.8f, 1.0f))
                .setFragmentSnippet("emissive *= getParticleColor().rgb;")
                .setShading(Shading::Unlit)
                .setBlending(Blending::Opaque)
                .addModelShader(ModelShader::Effect)
                .build();
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
        MaterialBuilder builder {*this};
        builder.setName("skeleton4ik")
                .add(Property::EmissiveColor, glm::vec4(0.3f, 1.5f, 0.5f, 1.0f))
                .setFragmentSnippet("emissive *= circle(getVertexUV(), 0.7);"
                                    "albedo.rgb *= circle(getVertexUV(), 0.7);")
                .setShading(Shading::Unlit)
                .setBlending(Blending::Additive)
                .setGlobalSnippet("#include \"../../functions/circle.glsl\"")
                .addModelShader(ModelShader::Effect)
                .build();
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
        MaterialBuilder builder {*this};
        const fs::path assets_dir {ENGINE_ASSETS_DIR};
        builder .setName("aura")
                .setShading(::Shading::Unlit)
                .setBlending(::Blending::Additive)
                .setTwoSided(true)
                .add(Property::Diffuse, TextureLoader::load(*this, assets_dir / "textures/aura.png"))
                .setFragmentSnippet("emissive = albedo.rgb * 5.0;")
                .addModelShader(::ModelShader::Effect)
                .build();

        builder .setName("aura_sparks")
                .setShading(::Shading::Unlit)
                .setBlending(::Blending::Additive)
                .setTwoSided(true)
                .add(Property::BlendMask, TextureLoader::load(*this, assets_dir / "textures/pirate.jpg", {
                        TextureLoaderFlags::Origin::TopLeft,
                        TextureLoaderFlags::Filter::Nearest
                }))
                .add(Property::EmissiveColor, glm::vec4(3.5f, 0.0f, 0.0f, 1.0f))
                .addModelShader(::ModelShader::Effect)
                .build();

        builder .setName("aura_beam")
                .setShading(::Shading::Unlit)
                .setBlending(::Blending::Additive)
                .setTwoSided(true)
                .add(Property::EmissiveColor, glm::vec4(3.5f, 0.0f, 0.0f, 1.0f))
                .addModelShader(::ModelShader::Effect)
                .build();
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

        MaterialBuilder builder{*this};
        auto yellowenv = builder.setName("yellowenv")
                .add(Property::Color, glm::vec4(0.546171f, 0.532152f, 0.546140f, 1.0f))
                .add(Property::Diffuse, TextureLoader::load(*this, assets_dir / "models/thanos/bodyyellow_m.tga"))
                .add(Property::Normal, TextureLoader::load(*this, assets_dir / "models/thanos/body_n.tga"))
                .addUniform(std::make_unique<UniformSampler>("emissive_map", TextureLoader::load(*this, assets_dir /
                                                                                                        "models/thanos/yellow_env.tga")))
                .setFragmentSnippet("emissive = texture(emissive_map, getVertexUV()).rgb;")
                .setShading(Shading::Lit)
                .addModelShader(ModelShader::Skeletal)
                .build();

        auto grayenv = builder.setName("grayenv")
                .add(Property::Color, glm::vec4(0.636099f, 0.753402f, 0.750975f, 1.0f))
                .add(Property::Diffuse, TextureLoader::load(*this, assets_dir / "models/thanos/bodygray_m.tga"))
                .add(Property::Normal, TextureLoader::load(*this, assets_dir / "models/thanos/body_n.tga"))
                .addUniform(std::make_unique<UniformSampler>("emissive_map", TextureLoader::load(*this, assets_dir /
                                                                                                        "models/thanos/gray_env.tga")))
                .setFragmentSnippet("emissive = texture(emissive_map, getVertexUV()).rgb;")
                .setShading(Shading::Lit)
                .addModelShader(ModelShader::Skeletal)
                .build();

        auto eyesenv = builder.setName("eyesenv")
                .add(Property::Color, glm::vec4(0.539415f, 0.560208f, 0.510821f, 1.0f))
                .add(Property::Diffuse, TextureLoader::load(*this, assets_dir / "models/thanos/body_d.tga"))
                .add(Property::Normal, TextureLoader::load(*this, assets_dir / "models/thanos/body_n.tga"))
                .addUniform(std::make_unique<UniformSampler>("emissive_map", TextureLoader::load(*this, assets_dir /
                                                                                                        "models/thanos/eye_env.tga")))
                .setFragmentSnippet("emissive = texture(emissive_map, getVertexUV()).rgb;")
                .setShading(Shading::Lit)
                .addModelShader(ModelShader::Skeletal)
                .build();

        auto blueenv = builder.setName("blueenv")
                .add(Property::Color, glm::vec4(0.576473f, 0.481502f, 0.448319f, 1.0f))
                .add(Property::Diffuse, TextureLoader::load(*this, assets_dir / "models/thanos/bodyblue_m.tga"))
                .add(Property::Normal, TextureLoader::load(*this, assets_dir / "models/thanos/body_n.tga"))
                .addUniform(std::make_unique<UniformSampler>("emissive_map", TextureLoader::load(*this, assets_dir /
                                                                                                        "models/thanos/blue_env.tga")))
                .setFragmentSnippet("emissive = texture(emissive_map, getVertexUV()).rgb;")
                .setShading(Shading::Lit)
                .addModelShader(ModelShader::Skeletal)
                .build();

        auto eyes = builder.setName("eyes")
                .add(Property::Color, glm::vec4(0.551182f, 0.681717f, 0.402268f, 1.0f))
                .add(Property::Diffuse, TextureLoader::load(*this, assets_dir / "models/thanos/body_d.tga"))
                .add(Property::Normal, TextureLoader::load(*this, assets_dir / "models/thanos/body_n.tga"))
                .add(Property::MetallicTexture, TextureLoader::load(*this, assets_dir / "models/thanos/body_s.tga"))
                .add(Property::Roughness, 0.2f)
                .setShading(Shading::Lit)
                .addModelShader(ModelShader::Skeletal)
                .build();

        auto body = builder.setName("body")
                .add(Property::Color, glm::vec4(0.566327f, 0.533939f, 0.783312f, 1.0f))
                .add(::Property::Diffuse, TextureLoader::load(*this, assets_dir / "models/thanos/body_d.tga"))
                .add(::Property::Normal, TextureLoader::load(*this, assets_dir / "models/thanos/body_n.tga"))
                .add(::Property::MetallicTexture, TextureLoader::load(*this, assets_dir / "models/thanos/body_s.tga"))
                .add(::Property::Roughness, 0.2f)
                .add(::Property::AmbientOcclusionTexture,
                     TextureLoader::load(*this, assets_dir / "models/thanos/body_ao.tga"))
                .setShading(Shading::Lit)
                .addModelShader(ModelShader::Skeletal)
                .build();

        auto skin = builder.setName("skin")
                .add(::Property::Diffuse, TextureLoader::load(*this, assets_dir / "models/thanos/body_d.tga"))
                .add(::Property::Normal, TextureLoader::load(*this, assets_dir / "models/thanos/body_n.tga"))
                .add(::Property::MetallicTexture, TextureLoader::load(*this, assets_dir / "models/thanos/body_s.tga"))
                .add(::Property::Roughness, 0.2f)
                .add(::Property::AmbientOcclusionTexture,
                     TextureLoader::load(*this, assets_dir / "models/thanos/skin_ao.tga"))
                .setShading(Shading::Lit)
                .addModelShader(ModelShader::Skeletal)
                .build();

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

        MaterialBuilder builder{*this};
        auto head = builder.setName("dae_head")
                .add(::Property::Color, glm::vec4(0.657693f, 0.475201f, 0.647254f, 1.0f))
                .add(::Property::Diffuse,
                     TextureLoader::load(*this, assets_dir / "models/daenerys/tex_commander1_daenerys_face_d.png"))
                .add(::Property::Normal,
                     TextureLoader::load(*this, assets_dir / "models/daenerys/tex_commander1_daenerys_face_n.png"))
                .add(::Property::MetallicTexture,
                     TextureLoader::load(*this, assets_dir / "models/daenerys/tex_commander1_daenerys_face_m.png"))
                .add(::Property::Roughness, 0.2f)
                .add(::Property::AmbientOcclusionTexture,
                     TextureLoader::load(*this, assets_dir / "models/daenerys/tex_commander1_daenerys_face_L.png"))
                .setShading(Shading::Lit)
                .addModelShader(ModelShader::Skeletal)
                .build();

        auto cloth = builder.setName("dae_cloth")
                .add(::Property::Diffuse,
                     TextureLoader::load(*this, assets_dir / "models/daenerys/tex_commander1_daenerys_cloth_d.png"))
                .add(::Property::Normal,
                     TextureLoader::load(*this, assets_dir / "models/daenerys/tex_commander1_daenerys_cloth_ns.png"))
                .add(::Property::MetallicTexture,
                     TextureLoader::load(*this, assets_dir / "models/daenerys/tex_commander1_daenerys_cloth_m.png"))
                .add(::Property::Roughness, 0.2f)
                .add(::Property::AmbientOcclusionTexture,
                     TextureLoader::load(*this, assets_dir / "models/daenerys/tex_commander1_daenerys_cloth_L.png"))
                .setShading(Shading::Lit)
                .addModelShader(ModelShader::Skeletal)
                .build();

        auto hair = builder.setName("dae_hair")
                .add(::Property::Color, glm::vec4(0.657236f, 0.567879f, 0.703609f, 1.0f))
                .add(::Property::Diffuse,
                     TextureLoader::load(*this, assets_dir / "models/daenerys/tex_commander1_daenerys_hair_d.png"))
                .add(::Property::Normal,
                     TextureLoader::load(*this, assets_dir / "models/daenerys/tex_commander1_daenerys_hair_n.png"))
                .add(::Property::MetallicTexture,
                     TextureLoader::load(*this, assets_dir / "models/daenerys/jitter2.png"))
                .add(::Property::Roughness, 0.2f)
                .setShading(Shading::Lit)
                .addModelShader(ModelShader::Skeletal)
                .build();

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

        MaterialBuilder builder{*this};
        auto skin = builder.setName("skeleton")
                .add(::Property::Diffuse, TextureLoader::load(*this, assets_dir / "models/skeleton/Skeleton_Body.png"))
                .addUniform(std::make_unique<UniformSampler>("ao_r_m", TextureLoader::load(*this, assets_dir /
                                                                                                  "models/skeleton/Skeleton_Body_OcclusionRoughnessMetallic.png")))
                .setFragmentSnippet("vec3 skin = texture(ao_r_m, getVertexUV()).rgb;"
                                    "ao = skin.r;"
                                    "roughness = skin.g;"
                                    "metallic = skin.b;")
                .setShading(Shading::Lit)
                .addModelShader(ModelShader::Skeletal)
                .build();

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

        MaterialBuilder builder{*this};
        auto body = builder.setName("k2_body")
                .add(::Property::Diffuse, TextureLoader::load(*this, assets_dir / "models/k2/K2_D.tga"))
                .add(::Property::Normal, TextureLoader::load(*this, assets_dir / "models/k2/K2_D.tga"))
                .add(::Property::MetallicTexture, TextureLoader::load(*this, assets_dir / "models/k2/K2_S.tga"))
                .add(::Property::Roughness, 0.2f)
                .setShading(Shading::Lit)
                .addModelShader(ModelShader::Skeletal)
                .build();

        auto head = builder.setName("k2_head")
                .add(::Property::Diffuse, TextureLoader::load(*this, assets_dir / "models/k2/K2_Head_D.tga"))
                .add(::Property::Normal, TextureLoader::load(*this, assets_dir / "models/k2/K2_Head_N.tga"))
                .add(::Property::MetallicTexture, TextureLoader::load(*this, assets_dir / "models/k2/K2_Head_S.tga"))
                .add(::Property::Roughness, 0.2f)
                .setShading(Shading::Lit)
                .addModelShader(ModelShader::Skeletal)
                .build();

        materials[0] = head;
        materials[1] = body;
    }

    {
        const fs::path assets_dir{ENGINE_ASSETS_DIR};

        models.add("elemental", ModelLoader::loadModel(*this, assets_dir / "models/elemental/elemental.fbx"));

        auto& elemental = dynamic_cast<SkeletalModel&>(*models.at("elemental"));

        auto& materials = elemental.getMaterials();

        MaterialBuilder builder{*this};
        auto body = builder.setName("elemental_body")
                .add(::Property::Diffuse, TextureLoader::load(*this, assets_dir / "models/elemental/Textures/Albedo FireElemental.png"))
                .add(::Property::Normal, TextureLoader::load(*this, assets_dir / "models/elemental/Textures/Normal FireElemental.png"))
                .add(::Property::RoughnessTexture, TextureLoader::load(*this, assets_dir / "models/elemental/Textures/Rougness FireElemental.png"))
                .add(::Property::Metallic, 0.1f)
                .addUniform(std::make_unique<UniformSampler>("emissive_map", TextureLoader::load(*this, assets_dir / "models/elemental/Textures/Emission FireElemental.png")))
                .setFragmentSnippet("emissive = texture(emissive_map, getVertexUV()).rgb;")
                .setShading(Shading::Lit)
                .build();

        materials[0] = body;
    }
    
    {
        const fs::path assets_dir {ASSETS_DIR};

        models.add("bob", ModelLoader::loadModel(*this, assets_dir / "models/boblamp/boblampclean.md5mesh"));
        models.add("backpack", ModelLoader::loadModel(*this, assets_dir / "models/backpack/backpack.obj", {{
               ::ModelLoaderOption::FlipUV
       }}));
        models.add("cyborg", ModelLoader::loadModel(*this, assets_dir / "models/cyborg/cyborg.obj"));
        models.add("drone", ModelLoader::loadModel(*this, assets_dir / "models/drone/model/BusterDrone.fbx"));
    }
}

void DemoAssets::loadSponzaScene() {
    const fs::path assets_dir {ASSETS_DIR};
    models.add("sponza", ModelLoader::loadModel(*this, assets_dir / "models/sponza/sponza.obj"));
}

void DemoAssets::loadAssets() {
    loadMaterialsScene();
    loadModelsScene();
    loadLightingScene();
    loadEffectsScene();
    loadSponzaScene();

    {
        const fs::path assets_dir{ASSETS_DIR};

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
        const fs::path assets_dir{ASSETS_DIR};

        ctx.setWindowIcon(TextureLoader::loadGLFWImage(*this, assets_dir / "icons/demo.png"));

        loadAssets();

        compileShaders(ctx, renderer.getSettings());
    }
}
