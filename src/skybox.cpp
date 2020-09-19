#include <skybox.hpp>

#include <texture_loader.hpp>
#include <shader_storage.hpp>
#include <assets.hpp>
#include <model.hpp>

using namespace GraphicsEngine;

Skybox::Skybox(const fs::path& path) {
    cubemap = TextureLoader::loadCubemap(path);
}

void Skybox::draw(Context& context) {
    auto& shader = *shader_storage.get("skybox");

    context.setDepthFunc(DepthFunc::Lequal);
    context.setDepthMask(GL_FALSE);

    shader << UniformSampler{"skybox", cubemap};

    shader.use();

    assets.models.get("cube")->getMeshes()[0]->draw();

    context.setDepthFunc(DepthFunc::Less);
    context.setDepthMask(GL_TRUE);
}
