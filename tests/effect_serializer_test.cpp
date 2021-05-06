#include "catch_amalgamated.hpp"

#include <core/context.hpp>

#include <particle_system/effect_builder.hpp>
#include <serialization/effect_serializer.hpp>
#include <util/bytebuffer.hpp>
#include <ms/material_builder.hpp>
#include <limitless/assets.hpp>

using namespace Limitless;

class FakeBackend {
private:
    Context ctx;
public:
    FakeBackend() : ctx{"test", {1, 1}, {{WindowHint::Visible, false}}} {

    }
};

TEST_CASE("serialize Effect") {
    FakeBackend fake;

    MaterialBuilder builder;
    auto material = builder.create("material1")
            .add(PropertyType::Color, glm::vec4(0.7f, 0.3, 0.5f, 1.0f))
            .setShading(Shading::Lit)
            .build();

    EffectBuilder eb;
    auto effect = eb.create("eff")
            .createEmitter<SpriteEmitter>("test")
//            .addModule<InitialRotation>(EmitterModuleType::InitialRotation, new RangeDistribution{glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{6.28f, 0.0f, 0.0f}})
            .addModule<Lifetime>(EmitterModuleType::Lifetime, new RangeDistribution(0.2f, 0.4f))
//            .addModule<InitialSize>(EmitterModuleType::InitialSize, new ConstDistribution(512.0f))
//            .addModule<SizeByLife>(EmitterModuleType::SizeByLife, new RangeDistribution(256.0f, 512.0f), -1.0f)
//            .addModule<CustomMaterialModule>(EmitterModuleType::CustomMaterial, new RangeDistribution(0.0f, 0.9f), new RangeDistribution(0.0f, 0.9f), new ConstDistribution(0.0f))
//            .addModule<CustomMaterialModuleOverLife>(EmitterModuleType::CustomMaterialOverLife, nullptr, nullptr, new ConstDistribution(1.0f))
            .setMaterial(material)
            .setSpawnMode(EmitterSpawn::Mode::Spray)
            .setMaxCount(100)
            .setSpawnRate(100.0f)
            .build();

    // TODO: check existance in effectserializer first????????/
    assets.materials.remove("material1");

    EffectSerializer serializer;
    auto buffer = serializer.serialize(*effect);

    auto shrek = serializer.deserialize(buffer);

    REQUIRE_NOTHROW(shrek->get<SpriteEmitter>("test"));
    REQUIRE(shrek->get<SpriteEmitter>("test").getSpawn().mode == EmitterSpawn::Mode::Spray);
    REQUIRE(shrek->get<SpriteEmitter>("test").getSpawn().max_count == 100);
    REQUIRE(shrek->get<SpriteEmitter>("test").getSpawn().spawn_rate == 100.0f);
}
