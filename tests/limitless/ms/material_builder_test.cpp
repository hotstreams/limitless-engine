#include "../catch_amalgamated.hpp"
#include "../opengl_state.hpp"
#include "../util/textures.hpp"

#include <limitless/ms/material_builder.hpp>
#include <limitless/core/context.hpp>
#include <limitless/assets.hpp>

using namespace Limitless;
using namespace Limitless::ms;
using namespace LimitlessTest;

TEST_CASE("Material::Builder throw on empty properties") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    Material::Builder builder {};

    REQUIRE_THROWS_AS(builder.build(assets), material_builder_exception);

    check_opengl_state();
}

TEST_CASE("Material::Builder throw on empty name") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    Material::Builder builder {};
    builder.color({1.0f, 0.5f, 1.0f, 1.0f});

    REQUIRE_THROWS_AS(builder.build(assets), material_builder_exception);

    check_opengl_state();
}


TEST_CASE("Material::Builder successfully build material with color") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    Material::Builder builder {};

    builder.name("material")
           .color(glm::vec4{1.0f});

    auto material = builder.build(assets);

    REQUIRE(material->getShaderIndex() != 0);
    REQUIRE(material->getColor() == glm::vec4 {1.0f});
    REQUIRE(material->getModelShaders() == std::set {InstanceType::Model});

    check_opengl_state();
}

TEST_CASE("Material::Builder successfully build material with color and not default model shader") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    Material::Builder builder {};

    builder.name("material")
           .color(glm::vec4{1.0f})
           .models({InstanceType::Skeletal});


    auto material = builder.build(assets);

    REQUIRE(material->getShaderIndex() != 0);
    REQUIRE(material->getColor() == glm::vec4 {1.0f});
    REQUIRE(material->getModelShaders() == std::set {InstanceType::Skeletal});

    check_opengl_state();
}

TEST_CASE("Material::Builder assigns different material index") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    Material::Builder builder {};

    builder.name("material1")
            .color(glm::vec4{1.0f});


    auto material1 = builder.build(assets);

    builder.name("material2")
            .emissive_color(glm::vec4{1.0f});


    auto material2 = builder.build(assets);

    REQUIRE(material1->getShaderIndex() != material2->getShaderIndex());

    check_opengl_state();
}

TEST_CASE("Material::Builder reuses shader index if material types are equal") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    Material::Builder builder {};

    builder.name("material1")
            .color(glm::vec4{1.0f});


    auto material1 = builder.build(assets);

    builder.name("material2")
            .color(glm::vec4{5.0f});


    auto material2 = builder.build(assets);

    REQUIRE(material1->getShaderIndex() == material2->getShaderIndex());

    check_opengl_state();
}

TEST_CASE("Material::Builder assigns new index if material is custom") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    Material::Builder builder {};

    builder.name("material1")
            .color(glm::vec4{1.0f});


    auto material1 = builder.build(assets);

    builder.name("material2")
            .color(glm::vec4{1.0f})
            .fragment("snippet");


    auto material2 = builder.build(assets);

    REQUIRE(material1->getShaderIndex() != material2->getShaderIndex());

    check_opengl_state();
}

TEST_CASE("Material::Builder builds material with emissive color") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    Material::Builder builder {};

    builder.name("material")
            .emissive_color(glm::vec4{1.0f});

    auto material = builder.build(assets);

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getEmissiveColor() == glm::vec3 {1.0f});
    REQUIRE(material->getProperties().size() == 1);

    check_opengl_state();
}

TEST_CASE("Material::Builder builds material with metallic") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    Material::Builder builder {};

    builder.name("material")
           .metallic(1.0f);

    auto material = builder.build(assets);

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getMetallic() == 1.0f);
    REQUIRE(material->getProperties().size() == 1);

    check_opengl_state();
}

TEST_CASE("Material::Builder builds material with roughness") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    Material::Builder builder {};

    builder.name("material")
           .roughness(1.0f);

    auto material = builder.build(assets);

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getRoughness() == 1.0f);
    REQUIRE(material->getProperties().size() == 1);

    check_opengl_state();
}

TEST_CASE("Material::Builder builds material with index of refraction") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    Material::Builder builder {};

    builder.name("material")
            .refraction(true)
            .ior(0.66f);

    auto material = builder.build(assets);

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getIoR() == 0.66f);
    REQUIRE(material->getProperties().size() == 1);

    check_opengl_state();
}

TEST_CASE("Material::Builder builds material with absorption") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    Material::Builder builder {};

    builder.name("material")
           .absorption(0.66f);

    auto material = builder.build(assets);

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getAbsorption() == 0.66f);
    REQUIRE(material->getProperties().size() == 1);

    check_opengl_state();
}

TEST_CASE("Material::Builder builds material with diffuse") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};
    assets.textures.add("fake", Textures::fake());

    Material::Builder builder {};

    builder.name("material")
           .diffuse(assets.textures.at("fake"));

    auto material = builder.build(assets);

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getDiffuseTexture() == assets.textures.at("fake"));
    REQUIRE(material->getProperties().size() == 1);

    check_opengl_state();
}

TEST_CASE("Material::Builder builds material with normal") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};
    assets.textures.add("fake", Textures::fake());

    Material::Builder builder {};

    builder.name("material")
           .normal(assets.textures.at("fake"));

    auto material = builder.build(assets);

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getNormalTexture() == assets.textures.at("fake"));
    REQUIRE(material->getProperties().size() == 1);

    check_opengl_state();
}

TEST_CASE("Material::Builder builds material with EmissiveMask") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};
    assets.textures.add("fake", Textures::fake());

    Material::Builder builder {};

    builder.name("material")
            .emissive_mask(assets.textures.at("fake"));

    auto material = builder.build(assets);

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getEmissiveMaskTexture() == assets.textures.at("fake"));
    REQUIRE(material->getProperties().size() == 1);

    check_opengl_state();
}

TEST_CASE("Material::Builder builds material with BlendMask") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};
    assets.textures.add("fake", Textures::fake());

    Material::Builder builder {};

    builder.name("material")
           .blend_mask(assets.textures.at("fake"));

    auto material = builder.build(assets);

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getBlendMaskTexture() == assets.textures.at("fake"));
    REQUIRE(material->getProperties().size() == 1);

    check_opengl_state();
}

TEST_CASE("Material::Builder builds material with MetallicTexture") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};
    assets.textures.add("fake", Textures::fake());

    Material::Builder builder {};

    builder.name("material")
           .metallic(assets.textures.at("fake"));

    auto material = builder.build(assets);

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getMetallicTexture() == assets.textures.at("fake"));
    REQUIRE(material->getProperties().size() == 1);

    check_opengl_state();
}

TEST_CASE("Material::Builder builds material with RoughnessTexture") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};
    assets.textures.add("fake", Textures::fake());

    Material::Builder builder {};

    builder.name("material")
           .roughness(assets.textures.at("fake"));

    auto material = builder.build(assets);

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getRoughnessTexture() == assets.textures.at("fake"));
    REQUIRE(material->getProperties().size() == 1);

    check_opengl_state();
}

TEST_CASE("Material::Builder builds material with AmbientOcclusionTexture") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};
    assets.textures.add("fake", Textures::fake());

    Material::Builder builder {};

    builder.name("material")
           .ao(assets.textures.at("fake"));

    auto material = builder.build(assets);

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getAmbientOcclusionTexture() == assets.textures.at("fake"));
    REQUIRE(material->getProperties().size() == 1);

    check_opengl_state();
}

TEST_CASE("Material::Builder builds material with ORM") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};
    assets.textures.add("fake", Textures::fake());

    Material::Builder builder {};

    builder.name("material")
           .orm(assets.textures.at("fake"));

    auto material = builder.build(assets);

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getORMTexture() == assets.textures.at("fake"));
    REQUIRE(material->getProperties().size() == 1);

    check_opengl_state();
}

TEST_CASE("Material::Builder builds material with blending") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    Material::Builder builder {};

    builder.name("material")
            .color(glm::vec4 {1.0f})
            .blending(Blending::Translucent);

    auto material = builder.build(assets);

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getColor() == glm::vec4 {1.0f});
    REQUIRE(material->getProperties().size() == 1);
    REQUIRE(material->getBlending() == Blending::Translucent);

    check_opengl_state();
}

TEST_CASE("Material::Builder builds material with shading") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    Material::Builder builder {};

    builder.name("material")
            .color(glm::vec4 {1.0f})
            .shading(Shading::Unlit);

    auto material = builder.build(assets);

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getColor() == glm::vec4 {1.0f});
    REQUIRE(material->getProperties().size() == 1);
    REQUIRE(material->getShading() == Shading::Unlit);

    check_opengl_state();
}

TEST_CASE("Material::Builder builds material with two sided") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    Material::Builder builder {};

    builder.name("material")
            .color(glm::vec4 {1.0f})
            .two_sided(true);

    auto material = builder.build(assets);

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getColor() == glm::vec4 {1.0f});
    REQUIRE(material->getProperties().size() == 1);
    REQUIRE(material->getTwoSided() == true);

    check_opengl_state();
}

TEST_CASE("Material::Builder builds material with refraction") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    Material::Builder builder {};

    builder.name("material")
            .color(glm::vec4 {1.0f})
            .refraction(true);

    auto material = builder.build(assets);

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getColor() == glm::vec4 {1.0f});
    REQUIRE(material->getProperties().size() == 1);
    REQUIRE(material->getRefraction() == true);

    check_opengl_state();
}

TEST_CASE("Material::Builder builds material with fragment snippet") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    Material::Builder builder {};

    builder.name("material")
            .color(glm::vec4 {1.0f})
            .fragment("snippet");

    auto material = builder.build(assets);

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getColor() == glm::vec4 {1.0f});
    REQUIRE(material->getProperties().size() == 1);
    REQUIRE(material->getFragmentSnippet() == "snippet");

    check_opengl_state();
}

TEST_CASE("Material::Builder builds material with vertex snippet") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    Material::Builder builder {};

    builder.name("material")
            .color(glm::vec4 {1.0f})
            .vertex("snippet");

    auto material = builder.build(assets);

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getColor() == glm::vec4 {1.0f});
    REQUIRE(material->getProperties().size() == 1);
    REQUIRE(material->getVertexSnippet() == "snippet");

    check_opengl_state();
}

TEST_CASE("Material::Builder builds material with global snippet") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    Material::Builder builder {};

    builder.name("material")
            .color(glm::vec4 {1.0f})
            .global("snippet");

    auto material = builder.build(assets);

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getColor() == glm::vec4 {1.0f});
    REQUIRE(material->getProperties().size() == 1);
    REQUIRE(material->getGlobalSnippet() == "snippet");

    check_opengl_state();
}

//TEST_CASE("Material::Builder builds material with custom uniform") {
//    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
//    Assets assets {"../assets"};
//
//    Material::Builder builder {};
//
//    builder.name("material")
//            .color(glm::vec4 {1.0f})
//            .custom("custom", 0.5f));
//
//    auto material = builder.build(assets);
//
//    REQUIRE(material->getName() == "material");
//    REQUIRE(material->getColor() == glm::vec4 {1.0f});
//    REQUIRE(material->getProperties().size() == 1);
//    REQUIRE(material->getUniforms().size() == 1);
//    REQUIRE(material->getU<UniformValue<float>>("custom") == 0.5f);
//
//    check_opengl_state();
//}

TEST_CASE("Material::Builder builds material with model shaders") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    Material::Builder builder {};

    builder.name("material")
            .color(glm::vec4 {1.0f})
            .models({InstanceType::Effect, InstanceType::Skeletal});

    auto material = builder.build(assets);

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getColor() == glm::vec4 {1.0f});
    REQUIRE(material->getProperties().size() == 1);

    REQUIRE(material->getModelShaders().size() == 2);
    REQUIRE(material->getModelShaders() == std::set {InstanceType::Effect, InstanceType::Skeletal});

    check_opengl_state();
}

TEST_CASE("Material::Builder builds skybox material") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    Material::Builder builder {};

    builder.name("material")
            .color(glm::vec4 {1.0f})
            .shading(Shading::Unlit)
            .skybox();

    auto material = builder.build(assets);

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getColor() == glm::vec4 {1.0f});
    REQUIRE(material->getProperties().size() == 1);

    REQUIRE(material->getModelShaders().empty());

    check_opengl_state();
}

//TEST_CASE("Material::Builder builds with specified properties") {
//    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
//    Assets assets {"../assets"};
//
//    std::map<Property, std::unique_ptr<Uniform>> properties;
//    properties.emplace(Property::Color, std::make_unique<UniformValue<glm::vec4>>("color", glm::vec4{1.0f}));
//
//    Material::Builder builder {};
//
//    builder.name("material")
//            .set(std::move(properties));
//
//    auto material = builder.build(assets);
//
//    REQUIRE(material->getName() == "material");
//    REQUIRE(material->getColor() == glm::vec4 {1.0f});
//    REQUIRE(material->getProperties().size() == 1);
//
//    check_opengl_state();
//}
//
//TEST_CASE("Material::Builder builds with specified uniforms") {
//    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
//    Assets assets {"../assets"};
//
//    std::map<std::string, std::unique_ptr<Uniform>> uniforms;
//    uniforms.emplace("custom_color", std::make_unique<UniformValue<glm::vec4>>("custom_color", glm::vec4{1.0f}));
//
//    Material::Builder builder {};
//
//    builder.name("material")
//            glm::vec4 {1.0f})
//            .set(std::move(uniforms));
//
//    auto material = builder.build(assets);
//
//    REQUIRE(material->getName() == "material");
//    REQUIRE(material->getProperty<UniformValue<glm::vec4>>("custom_color") == glm::vec4 {1.0f});
//    REQUIRE(material->getUniforms().size() == 1);
//
//    check_opengl_state();
//}