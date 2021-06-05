#include "catch_amalgamated.hpp"

#include <core/context.hpp>
#include <limitless/loaders/material_loader.hpp>
#include <ms/material_builder.hpp>
#include <iostream>
#include <limitless/loaders/asset_manager.hpp>
#include <particle_system/effect_builder.hpp>
#include <limitless/loaders/effect_loader.hpp>


using namespace Limitless;

class FakeBackend {
private:
    Context ctx;
public:
    FakeBackend() : ctx{"test", {1, 1}, {{WindowHint::Visible, false}}} {

    }
};

TEST_CASE("material loader") {
    FakeBackend fake;

    MaterialLoader loader;

std::cout << "dir = " << MATERIAL_DIR << std::endl;
//    MaterialBuilder builder;
//    auto mat = builder  .create("test")
//                        .add(PropertyType::Color, {1.0f, 0.5f, 0.5f, 1.0f})
//                        .setBlending(Blending::Additive)
//                        .build();
//
//    loader.save("test");

    loader.load("test");
}

TEST_CASE("effect loader save") {
    FakeBackend fake;

    MaterialBuilder builder;
    auto material1 = builder.create("material1")
            .add(PropertyType::Color, glm::vec4(0.7f, 0.3, 0.5f, 1.0f))
            .setShading(Shading::Lit)
            .build();

    EffectBuilder eb;
    auto effect1 = eb.create("test_effect")
            .createEmitter<SpriteEmitter>("test")
//            .addModule<InitialVelocity>(EmitterModuleType::InitialVelocity, new RangeDistribution{glm::vec3{-5.0f}, glm::vec3{5.0f}})
            .addModule<Lifetime>(EmitterModuleType::Lifetime, new RangeDistribution(0.2f, 0.5f))
//            .addModule<InitialSize>(EmitterModuleType::InitialSize, new RangeDistribution(0.0f, 50.0f))
//            .addModule<SizeByLife>(EmitterModuleType::SizeByLife, new RangeDistribution(0.0f, 100.0f), -1.0f)
            .setMaterial(material1)
            .setSpawnMode(EmitterSpawn::Mode::Burst)
            .setBurstCount(std::make_unique<ConstDistribution<uint32_t>>(1000))
            .setMaxCount(1000)
            .setSpawnRate(1.0f)
            .build();

    EffectLoader loader;
    loader.save("test_effect");
}

TEST_CASE("effect loader load") {
    FakeBackend fake;

    EffectLoader loader;
    auto effect = loader.load("test_effect");

    REQUIRE(effect->getName() == "test_effect");
    REQUIRE_NOTHROW(effect->get<SpriteEmitter>("test"));
    REQUIRE(effect->get<SpriteEmitter>("test").getSpawn().mode == EmitterSpawn::Mode::Burst);
    REQUIRE(effect->get<SpriteEmitter>("test").getSpawn().max_count == 1000);
}
