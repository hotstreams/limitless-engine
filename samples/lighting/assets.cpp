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
            .name("PBR")
            .metallic(TextureLoader::load(*this, assets_dir / "textures/rustediron2_metallic.png"))
            .roughness(TextureLoader::load(*this, assets_dir / "textures/rustediron2_roughness.png"))
            .diffuse(TextureLoader::load(*this, assets_dir / "textures/rustediron2_basecolor.png", {TextureLoaderFlags::Space::sRGB}))
            .normal(TextureLoader::load(*this, assets_dir / "textures/rustediron2_normal.png"))
            .shading(Shading::Lit)
            .models({InstanceType::Instanced, InstanceType::Model})
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
