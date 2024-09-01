#include "assets.hpp"

#include <limitless/ms/material_builder.hpp>
#include <limitless/fx/effect_builder.hpp>
#include <limitless/loaders/texture_loader.hpp>
#include <limitless/text/font_atlas.hpp>

void LimitlessMaterials::Assets::setUpMaterials() {
    using namespace Limitless;
    using namespace Limitless::ms;

    const fs::path assets_dir {ENGINE_ASSETS_DIR};
//
//    auto terrain_diffuse = TextureLoader::load(*this,{
//        "dirt.png",
//        "rock.png",
//        "grass/grass_color.jpg"
//    });
//
//    auto terrain_normal = TextureLoader::load(*this,{
//            "dirt_norm.png",
//            "rock_norm.png",
//            "grass/grass_normal.jpg"
//    });
//
//    auto terrain_orm = TextureLoader::load(*this,{
//            "dirt_orm.png",
//            "rock_orm.png",
//            "grass_orm.png"
//    });
//
//    Material::builder()
//            .name("terrain")
//            .shading(Shading::Lit)
//            .global("#include \"../functions/unpack8x4.glsl\""
//                    "#include \"../terrain/terrain.glsl\"")
//            .custom("_terrain_diffuse_texture", terrain_diffuse)
//            .custom("_terrain_normal_texture", terrain_normal)
//            .custom("_terrain_orm_texture", terrain_orm)
//            .fragment("calculateTerrain(mctx);")
//            .build(*this);
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
