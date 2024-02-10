#include "assets.hpp"

#include <limitless/ms/material_builder.hpp>
#include <limitless/fx/effect_builder.hpp>
#include <limitless/loaders/texture_loader.hpp>
#include <limitless/text/font_atlas.hpp>
#include <limitless/fx/emitters/beam_emitter.hpp>
#include <limitless/loaders/gltf_model_loader.hpp>

void LimitlessMaterials::Assets::setUpEffects() {
    using namespace Limitless;
    using namespace Limitless::ms;
    using namespace Limitless::fx;

    const fs::path assets_dir {ENGINE_ASSETS_DIR};

    models.add("model", GltfModelLoader::loadModel(*this, assets_dir / "models/gltf/BrainStem.gltf", {}));

    Material::builder()
            .name("basic1")
            .metallic(0.3f)
            .roughness(TextureLoader::load(*this, assets_dir / "textures/stonework_roughness.png"))
            .diffuse(TextureLoader::load(*this, assets_dir / "textures/stonework_albedo.png"))
            .ao(TextureLoader::load(*this, assets_dir / "textures/stonework_ao.png"))
            .normal(TextureLoader::load(*this, assets_dir / "textures/stonework_normal.png"))
            .shading(Shading::Lit)
            .models({InstanceType::Model, InstanceType::Instanced})
            .build(*this);

    {
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
                .setSpawnMode(EmitterSpawn::Mode::Burst)
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
                .setSpawnMode(EmitterSpawn::Mode::Burst)
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
        Material::builder()
                .name("shield")
                .shading(Shading::Unlit)
                .blending(Blending::Additive)
                .model(InstanceType::Effect)
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
                .model(InstanceType::Effect)
                .build(*this);

        Material::builder().name("fireball_sparks")
                .fragment("mctx.emissive_color *= circle(getVertexUV(), 0.7);"
                          "mctx.color.rgb *= circle(getVertexUV(), 0.7);")
                .global("#include \"../functions/circle.glsl\"")

                .emissive_color( glm::vec4{5.0f, 1.5f, 1.0f, 1.0f})
                .color(glm::vec4(1.0))
                .shading(Shading::Unlit)
                .blending(Blending::Additive)
                .model(InstanceType::Effect)
                .build(*this);
    }
    {
        EffectBuilder builder {*this};
        builder.create("fireball")
                .createEmitter<SpriteEmitter>("fire")
                .addInitialRotation(std::make_unique<RangeDistribution<glm::vec3>>(glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{6.28f, 0.0f, 0.0f}))
                .addLifetime(std::make_unique<RangeDistribution<float>>(0.5f, 0.7f))
                .addInitialSize(std::make_unique<ConstDistribution<float>>(512.0f))
                .addSizeByLife(std::make_unique<ConstDistribution<float>>(1024.0f))
                .addCustomMaterial(std::make_unique<RangeDistribution<float>>(0.0f, 0.9f), std::make_unique<RangeDistribution<float>>(0.0f, 0.9f), std::make_unique<ConstDistribution<float>>(0.0f), nullptr)
                .addCustomMaterialByLife(nullptr, nullptr, std::make_unique<ConstDistribution<float>>(1.0f), nullptr)
                .setMaterial(materials.at("fireball_material"))
                .setSpawnMode(EmitterSpawn::Mode::Spray)
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
                .emissive_color(glm::vec4(50.0, 0.0, 50.0, 1.0))
                .color(glm::vec4(0.0))
                .fragment("mctx.emissive_color *= circle(getVertexUV(), 0.5);")
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
                .emissive_color( glm::vec4(1.0f))
                .color( glm::vec4(0.0f))
                .fragment("mctx.emissive_color *= circle(getVertexUV(), 0.7) * getParticleColor().rgb * 5.0;")
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
                .addInitialColor(std::make_unique<RangeDistribution<glm::vec4>>(glm::vec4{2.0f}, glm::vec4{5.0f}))
                .addInitialMeshLocation(dynamic_cast<SkeletalModel&>(*models.at("model")).getMeshes().at(1))
                .setMaterial(materials.at("drop"))
                .setMaxCount(500)
                .setSpawnRate(500.0f)
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
                .setSpawnMode(EmitterSpawn::Mode::Burst)
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
                .addInitialMeshLocation(models.at("model"), glm::vec3(1.0f), glm::vec3(-M_PI_2, 0.0f, M_PI_2))
                .setMaterial(materials.at("skeleton4ik"))
                .setMaxCount(4000)
                .setSpawnRate(4000.0f)
                .build();
    }

    {
        Material::builder()
                .name("aura")
                .shading(Shading::Unlit)
                .blending(Blending::Additive)
                .two_sided(true)
                .diffuse( TextureLoader::load(*this, assets_dir / "textures/aura.png"))
                .model(InstanceType::Effect)
                .build(*this);

        Material::builder().name("aura_sparks")
                .shading(Shading::Unlit)
                .blending(Blending::Additive)
                .two_sided(true)
                .blend_mask(TextureLoader::load(*this, assets_dir / "textures/pirate.jpg", {
                        TextureLoaderFlags::Origin::TopLeft,
                        TextureLoaderFlags::Filter::Nearest
                }))
                .emissive_color( glm::vec4(3.5f, 0.0f, 0.0f, 1.0f))
                .model(InstanceType::Effect)
                .build(*this);

        Material::builder().name("aura_beam")
                .shading(Shading::Unlit)
                .blending(Blending::Additive)
                .two_sided(true)
                .emissive_color( glm::vec4(3.5f, 0.0f, 0.0f, 1.0f))
                .model(InstanceType::Effect)
                .build(*this);
    }

    {
        EffectBuilder builder{*this};
        builder.create("aura")
                .createEmitter<MeshEmitter>("aura")
                .setMesh(meshes.at("plane"))
                .setMaterial(materials.at("aura"))
                .setLocalSpace(true)
                .setSpawnMode(EmitterSpawn::Mode::Spray)
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
                .setSpawnMode(EmitterSpawn::Mode::Spray)
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

LimitlessMaterials::Assets::Assets(Limitless::Context &ctx, const fs::path &path)
    : Limitless::Assets {path} {
    Assets::load(ctx);
    setUpCommon(ctx);
    setUpEffects();
}

void LimitlessMaterials::Assets::setUpCommon(Limitless::Context &ctx) {
    const fs::path assets_dir {ENGINE_ASSETS_DIR};

    skyboxes.add("skybox", std::make_shared<Limitless::Skybox>(*this, assets_dir / "skyboxes/sky/sky.png", Limitless::TextureLoaderFlags {
        Limitless::TextureLoaderFlags::Origin::TopLeft,
        Limitless::TextureLoaderFlags::Space::sRGB
    }));

    fonts.add("nunito", std::make_shared<Limitless::FontAtlas>(assets_dir / "fonts/nunito.ttf", 48));

    ctx.setWindowIcon(Limitless::TextureLoader::loadGLFWImage(*this, assets_dir / "icons/demo.png"));
}
