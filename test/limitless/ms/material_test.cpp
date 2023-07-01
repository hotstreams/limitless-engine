#include "../catch_amalgamated.hpp"
#include "../opengl_state.hpp"
#include "../util/textures.hpp"

#include <limitless/ms/material_builder.hpp>
#include <limitless/core/context.hpp>
#include <limitless/assets.hpp>

using namespace Limitless;
using namespace Limitless::ms;
using namespace LimitlessTest;

TEST_CASE("Material copy constructor makes deep copy") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};

    auto material = builder
                            .setName("material")
                            .add(Property::Color, glm::vec4{1.0f})
                            .build();


    Material copy = Material {*material};

    REQUIRE(copy.getProperties().size() == 1);
    REQUIRE(copy.getColor().getValue() == glm::vec4{1.0f});
    REQUIRE(copy.getName() == "material");

    check_opengl_state();
}

TEST_CASE("Material update resets uniforms") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};

    auto material = builder
            .setName("material")
            .add(Property::Color, glm::vec4{1.0f})
            .addUniform(std::make_unique<UniformValue<float>>("custom", 1.0f))
            .build();

    material->update();

    REQUIRE(material->getColor().isChanged() == false);
    REQUIRE(material->getProperty<UniformValue<float>>("custom").isChanged() == false);

    check_opengl_state();
}

TEST_CASE("Material update does not updates uniforms") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};

    auto material = builder
            .setName("material")
            .add(Property::Color, glm::vec4{1.0f})
            .addUniform(std::make_unique<UniformValue<float>>("custom", 1.0f))
            .build();

    material->getColor().resetChanged();
    material->getProperty<UniformValue<float>>("custom").resetChanged();

    material->update();

    REQUIRE(material->getColor().isChanged() == false);
    REQUIRE(material->getProperty<UniformValue<float>>("custom").isChanged() == false);

    check_opengl_state();
}

TEST_CASE("Material contains works") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};

    auto material = builder
            .setName("material")
            .add(Property::Color, glm::vec4{1.0f})
            .addUniform(std::make_unique<UniformValue<float>>("custom", 1.0f))
            .build();

    REQUIRE(material->contains(Property::Color) == true);
    REQUIRE(material->contains("custom") == true);

    check_opengl_state();
}

TEST_CASE("Material equality operator works") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};

    auto material1 = builder
            .setName("material1")
            .add(Property::Color, glm::vec4{1.0f})
            .setBlending(Blending::Additive)
            .setShading(Shading::Unlit)
            .build();

    auto material2 = builder
            .setName("material2")
            .add(Property::Color, glm::vec4{1.0f})
            .setBlending(Blending::Additive)
            .setShading(Shading::Unlit)
            .build();

    auto material3 = builder
            .setName("material3")
            .add(Property::Color, glm::vec4{0.5f})
            .build();

    auto material4 = builder
            .setName("material4")
            .add(Property::EmissiveColor, glm::vec4{0.5f})
            .build();

    REQUIRE(*material1 == *material2);
    REQUIRE_FALSE(*material1 == *material3);
    REQUIRE_FALSE(*material1 == *material4);

    check_opengl_state();
}

