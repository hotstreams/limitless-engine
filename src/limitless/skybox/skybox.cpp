#include <limitless/skybox/skybox.hpp>

#include <limitless/loaders/texture_loader.hpp>
#include "limitless/core/shader/shader_program.hpp"
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

    material = Material::builder()
                    .name(path.stem().string())
                    .custom("skybox", cube_map_texture)
                    .fragment("mctx.color.rgb = texture(skybox, skybox_uv).rgb;\n")
                    .color(glm::vec4(1.0f))
                    .two_sided(true)
                    .shading(Shading::Unlit)
                    .blending(Blending::Opaque)
                    .skybox()
                    .build(assets);
}

void Skybox::draw(Context& context, const Assets& assets) {
    auto& shader = assets.shaders.get(ShaderType::Skybox, InstanceType::Model, material->getShaderIndex());

    context.enable(Capabilities::DepthTest);
    context.setDepthFunc(DepthFunc::Lequal);
    context.setDepthMask(DepthMask::True);
    context.disable(Capabilities::Blending);

    shader.setMaterial(*material)
          .setUniform("_model_transform", glm::mat4{1.0f});
    shader.use();

    assets.meshes.at("cube")->draw();
}
