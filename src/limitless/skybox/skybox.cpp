#include <limitless/skybox/skybox.hpp>

#include <limitless/loaders/texture_loader.hpp>
#include <limitless/core/shader_program.hpp>
#include <limitless/models/abstract_mesh.hpp>
#include <limitless/core/context.hpp>
#include <limitless/assets.hpp>
#include <limitless/ms/material_builder.hpp>

using namespace Limitless;
using namespace Limitless::ms;

Skybox::Skybox(const std::shared_ptr<Material>& material)
    : material {std::make_shared<Material>(*material)} {
}

Skybox::Skybox(Assets& assets, const fs::path& path, const TextureLoaderFlags& flags) {
    const auto& cube_map_texture = TextureLoader::loadCubemap(assets, path, flags);

    MaterialBuilder material_builder {assets};
    material = material_builder
                    .setName(path.stem().string())
                    .addUniform(std::make_unique<UniformSampler>("skybox", cube_map_texture))
                    .setFragmentSnippet("albedo.rgb = texture(skybox, uv).rgb;\n")
                    .add(Property::Color, glm::vec4(1.0f))
                    .setTwoSided(true)
                    .setShading(Shading::Unlit)
                    .setBlending(Blending::Opaque)
                    .buildSkybox();
}

void Skybox::draw(Context& context, const Assets& assets) {
    auto& shader = assets.shaders.get(ShaderPass::Skybox, ModelShader::Model, material->getShaderIndex());

    context.enable(Capabilities::DepthTest);
    context.setDepthFunc(DepthFunc::Lequal);
    context.setDepthMask(DepthMask::True);
    context.disable(Capabilities::Blending);

    shader << *material;

    shader.use();

    assets.meshes.at("cube")->draw();
}
