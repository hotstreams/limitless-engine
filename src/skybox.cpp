#include <limitless/skybox.hpp>

#include <limitless/texture_loader.hpp>
#include <limitless/shader_storage.hpp>
#include <limitless/assets.hpp>
#include <limitless/model.hpp>

using namespace LimitlessEngine;

Skybox::Skybox(const fs::path& path) {
    cubemap = TextureLoader::loadCubemap(path);
}

void Skybox::draw(Context& context) {
    auto& shader = *shader_storage.get("skybox");

    context.enable(Capabilities::DepthTest);
    context.setDepthFunc(DepthFunc::Lequal);
    context.setDepthMask(DepthMask::True);
    context.disable(Capabilities::Blending);

    shader << UniformSampler{"skybox", cubemap};

    shader.use();

    assets.models["cube"]->getMeshes()[0]->draw();
}
