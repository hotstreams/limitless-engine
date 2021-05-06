#include "catch_amalgamated.hpp"

#include <serialization/material_serializer.hpp>
#include <ms/custom_material_builder.hpp>
#include <util/bytebuffer.hpp>
#include <limitless/assets.hpp>
#include <core/context.hpp>

using namespace Limitless;

class FakeBackend {
private:
    Context ctx;
public:
    FakeBackend() : ctx{"test", {1, 1}, {{WindowHint::Visible, false}}} {

    }
};

TEST_CASE("serialize Material") {
    FakeBackend fake;

    MaterialSerializer serializer;

    MaterialBuilder builder;

    auto material = builder .create("test_mat")
                            .add(PropertyType::Color, glm::vec4{1.0f})
                            .setBlending(Blending::Opaque)
                            .setShading(Shading::Lit)
                            .build();

    auto buffer = serializer.serialize(*material);

    assets.materials.remove("test_mat");

    auto deserialized_material = serializer.deserialize(buffer);

    REQUIRE(deserialized_material->isCustom() == false);
    REQUIRE(deserialized_material->getName() == "test_mat");
    REQUIRE_NOTHROW(deserialized_material->getColor());
    REQUIRE(deserialized_material->getColor().getValue() == glm::vec4{1.0f});
    REQUIRE(deserialized_material->getBlending() == Blending::Opaque);

    assets.materials.remove("test_mat");
}

TEST_CASE("serialize custom Material") {
    FakeBackend fake;

    MaterialSerializer serializer;

    CustomMaterialBuilder builder;

    auto material = builder .create("test_mat")
            .addUniform("test_uniform", new UniformValue<float>("test_uniform", 10.0f))
            .add(PropertyType::Color, glm::vec4{1.0f})
            .setBlending(Blending::Opaque)
            .setShading(Shading::Lit)
            .build();

    auto buffer = serializer.serialize(*material);

    assets.materials.remove("test_mat");

    auto deserialized_material = serializer.deserialize(buffer);

    REQUIRE(deserialized_material->isCustom() == true);
    REQUIRE(deserialized_material->getName() == "test_mat");
    REQUIRE_NOTHROW(deserialized_material->getColor());
    REQUIRE(deserialized_material->getColor().getValue() == glm::vec4{1.0f});
    REQUIRE(deserialized_material->getBlending() == Blending::Opaque);

    REQUIRE_NOTHROW(dynamic_cast<CustomMaterial&>(*deserialized_material));
    REQUIRE(static_cast<CustomMaterial&>(*deserialized_material).getUniform<UniformValue<float>>("test_uniform").getValue() == 10.0f);

    assets.materials.remove("test_mat");
}

