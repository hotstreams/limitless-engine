#include "../catch_amalgamated.hpp"
#include "../opengl_state.hpp"
#include "../util/textures.hpp"

#include <limitless/ms/material_builder.hpp>
#include <limitless/core/context.hpp>
#include <limitless/assets.hpp>

using namespace Limitless;
using namespace Limitless::ms;
using namespace LimitlessTest;

TEST_CASE("MaterialBuilder throw on empty properties") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};

    REQUIRE_THROWS_AS(builder.build(), material_builder_error);

    check_opengl_state();
}

TEST_CASE("MaterialBuilder throw on empty name") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};
    builder.setName("material");

    REQUIRE_THROWS_AS(builder.build(), material_builder_error);

    check_opengl_state();
}

TEST_CASE("MaterialBuilder throw on tesselation when factor is not set") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};
    builder.setName("material");
    builder.setTessellationSnippet("snippet");

    REQUIRE_THROWS_AS(builder.build(), material_builder_error);

    check_opengl_state();
}

TEST_CASE("MaterialBuilder successfully build material with color") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};

    builder.setName("material")
           .add(Property::Color, glm::vec4{1.0f});

    auto material = builder.build();

    REQUIRE(material->getShaderIndex() == 1);
    REQUIRE(material->getColor().getValue() == glm::vec4 {1.0f});
    REQUIRE(material->getModelShaders() == std::set {ModelShader::Model});

    check_opengl_state();
}

TEST_CASE("MaterialBuilder successfully build material with color and not default model shader") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};

    builder.setName("material")
           .add(Property::Color, glm::vec4{1.0f})
           .setModelShaders({ModelShader::Skeletal});


    auto material = builder.build();

    REQUIRE(material->getShaderIndex() == 1);
    REQUIRE(material->getColor().getValue() == glm::vec4 {1.0f});
    REQUIRE(material->getModelShaders() == std::set {ModelShader::Skeletal});

    check_opengl_state();
}

TEST_CASE("MaterialBuilder assigns different material index") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};

    builder.setName("material1")
            .add(Property::Color, glm::vec4{1.0f});


    auto material1 = builder.build();

    builder.setName("material2")
            .add(Property::EmissiveColor, glm::vec4{1.0f});


    auto material2 = builder.build();

    REQUIRE(material1->getShaderIndex() != material2->getShaderIndex());

    check_opengl_state();
}

TEST_CASE("MaterialBuilder reuses shader index if material types are equal") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};

    builder.setName("material1")
            .add(Property::Color, glm::vec4{1.0f});


    auto material1 = builder.build();

    builder.setName("material2")
            .add(Property::Color, glm::vec4{5.0f});


    auto material2 = builder.build();

    REQUIRE(material1->getShaderIndex() == material2->getShaderIndex());

    check_opengl_state();
}

TEST_CASE("MaterialBuilder assigns new index if material is custom") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};

    builder.setName("material1")
            .add(Property::Color, glm::vec4{1.0f});


    auto material1 = builder.build();

    builder.setName("material2")
            .add(Property::Color, glm::vec4{1.0f})
            .setFragmentSnippet("snippet");


    auto material2 = builder.build();

    REQUIRE(material1->getShaderIndex() != material2->getShaderIndex());

    check_opengl_state();
}

TEST_CASE("MaterialBuilder builds material with emissive color") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};

    builder.setName("material")
            .add(Property::EmissiveColor, glm::vec4{1.0f});

    auto material = builder.build();

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getEmissiveColor().getValue() == glm::vec4 {1.0f});
    REQUIRE(material->getProperties().size() == 1);

    check_opengl_state();
}

TEST_CASE("MaterialBuilder builds material with metallic") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};

    builder.setName("material")
            .add(Property::Metallic, 1.0f);

    auto material = builder.build();

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getMetallic().getValue() == 1.0f);
    REQUIRE(material->getProperties().size() == 1);

    check_opengl_state();
}

TEST_CASE("MaterialBuilder builds material with roughness") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};

    builder.setName("material")
            .add(Property::Roughness, 1.0f);

    auto material = builder.build();

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getRoughness().getValue() == 1.0f);
    REQUIRE(material->getProperties().size() == 1);

    check_opengl_state();
}

TEST_CASE("MaterialBuilder builds material with tesselation factor") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};

    builder.setName("material")
            .add(Property::TessellationFactor, glm::vec2{8.0f, 8.0f});

    auto material = builder.build();

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getTesselationFactor().getValue() == glm::vec2{8.0f, 8.0f});
    REQUIRE(material->getProperties().size() == 1);

    check_opengl_state();
}

TEST_CASE("MaterialBuilder builds material with index of refraction") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};

    builder.setName("material")
            .add(Property::IoR, 0.66f);

    auto material = builder.build();

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getIoR().getValue() == 0.66f);
    REQUIRE(material->getProperties().size() == 1);

    check_opengl_state();
}

TEST_CASE("MaterialBuilder builds material with absorption") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};

    builder.setName("material")
            .add(Property::Absorption, 0.66f);

    auto material = builder.build();

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getAbsorption().getValue() == 0.66f);
    REQUIRE(material->getProperties().size() == 1);

    check_opengl_state();
}

TEST_CASE("MaterialBuilder builds material with diffuse") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};
    assets.textures.add("fake", Textures::fake());

    MaterialBuilder builder {assets};

    builder.setName("material")
            .add(Property::Diffuse, assets.textures.at("fake"));

    auto material = builder.build();

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getDiffuse().getSampler() == assets.textures.at("fake"));
    REQUIRE(material->getProperties().size() == 1);

    check_opengl_state();
}

TEST_CASE("MaterialBuilder builds material with normal") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};
    assets.textures.add("fake", Textures::fake());

    MaterialBuilder builder {assets};

    builder.setName("material")
            .add(Property::Normal, assets.textures.at("fake"));

    auto material = builder.build();

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getNormal().getSampler() == assets.textures.at("fake"));
    REQUIRE(material->getProperties().size() == 1);

    check_opengl_state();
}

TEST_CASE("MaterialBuilder builds material with EmissiveMask") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};
    assets.textures.add("fake", Textures::fake());

    MaterialBuilder builder {assets};

    builder.setName("material")
            .add(Property::EmissiveMask, assets.textures.at("fake"));

    auto material = builder.build();

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getEmissiveMask().getSampler() == assets.textures.at("fake"));
    REQUIRE(material->getProperties().size() == 1);

    check_opengl_state();
}

TEST_CASE("MaterialBuilder builds material with BlendMask") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};
    assets.textures.add("fake", Textures::fake());

    MaterialBuilder builder {assets};

    builder.setName("material")
            .add(Property::BlendMask, assets.textures.at("fake"));

    auto material = builder.build();

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getBlendMask().getSampler() == assets.textures.at("fake"));
    REQUIRE(material->getProperties().size() == 1);

    check_opengl_state();
}

TEST_CASE("MaterialBuilder builds material with MetallicTexture") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};
    assets.textures.add("fake", Textures::fake());

    MaterialBuilder builder {assets};

    builder.setName("material")
            .add(Property::MetallicTexture, assets.textures.at("fake"));

    auto material = builder.build();

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getMetallicTexture().getSampler() == assets.textures.at("fake"));
    REQUIRE(material->getProperties().size() == 1);

    check_opengl_state();
}

TEST_CASE("MaterialBuilder builds material with RoughnessTexture") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};
    assets.textures.add("fake", Textures::fake());

    MaterialBuilder builder {assets};

    builder.setName("material")
            .add(Property::RoughnessTexture, assets.textures.at("fake"));

    auto material = builder.build();

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getRoughnessTexture().getSampler() == assets.textures.at("fake"));
    REQUIRE(material->getProperties().size() == 1);

    check_opengl_state();
}

TEST_CASE("MaterialBuilder builds material with AmbientOcclusionTexture") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};
    assets.textures.add("fake", Textures::fake());

    MaterialBuilder builder {assets};

    builder.setName("material")
            .add(Property::AmbientOcclusionTexture, assets.textures.at("fake"));

    auto material = builder.build();

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getAmbientOcclusionTexture().getSampler() == assets.textures.at("fake"));
    REQUIRE(material->getProperties().size() == 1);

    check_opengl_state();
}

TEST_CASE("MaterialBuilder builds material with blending") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};

    builder.setName("material")
            .add(Property::Color, glm::vec4 {1.0f})
            .setBlending(Blending::Translucent);

    auto material = builder.build();

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getColor().getValue() == glm::vec4 {1.0f});
    REQUIRE(material->getProperties().size() == 1);
    REQUIRE(material->getBlending() == Blending::Translucent);

    check_opengl_state();
}

TEST_CASE("MaterialBuilder builds material with shading") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};

    builder.setName("material")
            .add(Property::Color, glm::vec4 {1.0f})
            .setShading(Shading::Unlit);

    auto material = builder.build();

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getColor().getValue() == glm::vec4 {1.0f});
    REQUIRE(material->getProperties().size() == 1);
    REQUIRE(material->getShading() == Shading::Unlit);

    check_opengl_state();
}

TEST_CASE("MaterialBuilder builds material with two sided") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};

    builder.setName("material")
            .add(Property::Color, glm::vec4 {1.0f})
            .setTwoSided(true);

    auto material = builder.build();

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getColor().getValue() == glm::vec4 {1.0f});
    REQUIRE(material->getProperties().size() == 1);
    REQUIRE(material->getTwoSided() == true);

    check_opengl_state();
}

TEST_CASE("MaterialBuilder builds material with refraction") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};

    builder.setName("material")
            .add(Property::Color, glm::vec4 {1.0f})
            .setRefraction(true);

    auto material = builder.build();

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getColor().getValue() == glm::vec4 {1.0f});
    REQUIRE(material->getProperties().size() == 1);
    REQUIRE(material->getRefraction() == true);

    check_opengl_state();
}

TEST_CASE("MaterialBuilder builds material with fragment snippet") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};

    builder.setName("material")
            .add(Property::Color, glm::vec4 {1.0f})
            .setFragmentSnippet("snippet");

    auto material = builder.build();

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getColor().getValue() == glm::vec4 {1.0f});
    REQUIRE(material->getProperties().size() == 1);
    REQUIRE(material->getFragmentSnippet() == "snippet");

    check_opengl_state();
}

TEST_CASE("MaterialBuilder builds material with vertex snippet") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};

    builder.setName("material")
            .add(Property::Color, glm::vec4 {1.0f})
            .setVertexSnippet("snippet");

    auto material = builder.build();

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getColor().getValue() == glm::vec4 {1.0f});
    REQUIRE(material->getProperties().size() == 1);
    REQUIRE(material->getVertexSnippet() == "snippet");

    check_opengl_state();
}

TEST_CASE("MaterialBuilder builds material with tess snippet") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};

    builder.setName("material")
            .add(Property::Color, glm::vec4 {1.0f})
            .setTessellationSnippet("snippet")
            .add(Property::TessellationFactor, glm::vec2 {16.0f});

    auto material = builder.build();

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getColor().getValue() == glm::vec4 {1.0f});
    REQUIRE(material->getProperties().size() == 2);
    REQUIRE(material->getTessellationSnippet() == "snippet");

    check_opengl_state();
}

TEST_CASE("MaterialBuilder builds material with global snippet") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};

    builder.setName("material")
            .add(Property::Color, glm::vec4 {1.0f})
            .setGlobalSnippet("snippet");

    auto material = builder.build();

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getColor().getValue() == glm::vec4 {1.0f});
    REQUIRE(material->getProperties().size() == 1);
    REQUIRE(material->getGlobalSnippet() == "snippet");

    check_opengl_state();
}

TEST_CASE("MaterialBuilder builds material with custom uniform") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};

    builder.setName("material")
            .add(Property::Color, glm::vec4 {1.0f})
            .addUniform(std::make_unique<UniformValue<float>>("custom", 0.5f));

    auto material = builder.build();

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getColor().getValue() == glm::vec4 {1.0f});
    REQUIRE(material->getProperties().size() == 1);
    REQUIRE(material->getUniforms().size() == 1);
    REQUIRE(material->getProperty<UniformValue<float>>("custom").getValue() == 0.5f);

    check_opengl_state();
}

TEST_CASE("MaterialBuilder builds material with model shaders") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};

    builder.setName("material")
            .add(Property::Color, glm::vec4 {1.0f})
            .addModelShader(ModelShader::Effect)
            .addModelShader(ModelShader::Skeletal);

    auto material = builder.build();

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getColor().getValue() == glm::vec4 {1.0f});
    REQUIRE(material->getProperties().size() == 1);

    REQUIRE(material->getModelShaders().size() == 2);
    REQUIRE(material->getModelShaders() == std::set {ModelShader::Effect, ModelShader::Skeletal});

    check_opengl_state();
}

TEST_CASE("MaterialBuilder builds skybox material") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    MaterialBuilder builder {assets};

    builder.setName("material")
            .add(Property::Color, glm::vec4 {1.0f})
            .setShading(Shading::Unlit);

    auto material = builder.buildSkybox();

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getColor().getValue() == glm::vec4 {1.0f});
    REQUIRE(material->getProperties().size() == 1);

    REQUIRE(material->getModelShaders().empty());

    check_opengl_state();
}

TEST_CASE("MaterialBuilder builds with specified properties") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    std::map<Property, std::unique_ptr<Uniform>> properties;
    properties.emplace(Property::Color, std::make_unique<UniformValue<glm::vec4>>("color", glm::vec4{1.0f}));

    MaterialBuilder builder {assets};

    builder.setName("material")
            .set(std::move(properties));

    auto material = builder.build();

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getColor().getValue() == glm::vec4 {1.0f});
    REQUIRE(material->getProperties().size() == 1);

    check_opengl_state();
}

TEST_CASE("MaterialBuilder builds with specified uniforms") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    std::map<std::string, std::unique_ptr<Uniform>> uniforms;
    uniforms.emplace("custom_color", std::make_unique<UniformValue<glm::vec4>>("custom_color", glm::vec4{1.0f}));

    MaterialBuilder builder {assets};

    builder.setName("material")
            .add(Property::Color, glm::vec4 {1.0f})
            .set(std::move(uniforms));

    auto material = builder.build();

    REQUIRE(material->getName() == "material");
    REQUIRE(material->getProperty<UniformValue<glm::vec4>>("custom_color").getValue() == glm::vec4 {1.0f});
    REQUIRE(material->getUniforms().size() == 1);

    check_opengl_state();
}