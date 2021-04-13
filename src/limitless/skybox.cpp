#include <limitless/skybox.hpp>

#include <limitless/loaders/texture_loader.hpp>
#include <limitless/core/shader_program.hpp>
#include <limitless/shader_storage.hpp>
#include <limitless/core/uniform.hpp>
#include <limitless/core/context.hpp>
#include <limitless/assets.hpp>

using namespace LimitlessEngine;

Skybox::Skybox(Assets& assets, const fs::path& path) {
    TextureLoader loader {assets};
    cubemap = loader.loadCubemap(path);
}

void Skybox::draw(Context& context, const Assets& assets) {
    auto& shader = assets.shaders.get("skybox");

    context.enable(Capabilities::DepthTest);
    context.setDepthFunc(DepthFunc::Lequal);
    context.setDepthMask(DepthMask::True);
    context.disable(Capabilities::Blending);

    shader << UniformSampler{"skybox", cubemap};

    shader.use();

    assets.meshes.at("cube_mesh")->draw();
}
