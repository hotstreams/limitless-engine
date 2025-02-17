#include "assets.hpp"

#include <limitless/ms/material_builder.hpp>
#include <limitless/fx/effect_builder.hpp>
#include <limitless/loaders/texture_loader.hpp>
#include <limitless/text/font_atlas.hpp>

void LimitlessMaterials::Assets::setUpMaterials() {
    using namespace Limitless;
    using namespace Limitless::ms;

    const fs::path assets_dir {ENGINE_ASSETS_DIR};

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
            .blending(Blending::Additive)
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
            .models({InstanceType::Model, InstanceType::Instanced})
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
            .normal(TextureLoader::load(*this, assets_dir / "textures/stonework_normal.png"))
            .shading(Shading::Lit)
            .blending(Blending::Opaque)
            .models({InstanceType::Decal})
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
            .vertex("vec2 uv_1 = vec2(vctx.uv.x + time * 0.05, vctx.uv.y + time * 0.0);\n"
                    "vec2 uv_2 = vec2(vctx.uv.x - time * 0.05, vctx.uv.y - time * 0.0);\n"
                    " \n"
                    "float s = texture(fire_mask, uv_1).r;\n"
                    "float t = texture(fire_mask, uv_2).r;\n"
                    "\n"
                    "vctx.position.xyz -= getVertexNormal() * texture(fire_mask, vctx.uv + vec2(s, t)).r * 0.6;")
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

LimitlessMaterials::Assets::Assets(Limitless::Context &ctx, Limitless::Renderer &renderer, const fs::path &path)
    : Limitless::Assets {path} {
    Assets::load(ctx);
    setUpCommon(ctx);
    setUpMaterials();

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
