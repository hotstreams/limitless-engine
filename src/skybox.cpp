#include <skybox.hpp>

#include <texture_loader.hpp>
#include <shader_storage.hpp>
#include <assets.hpp>

using namespace GraphicsEngine;

Skybox::Skybox(const fs::path& path) {
    cubemap = TextureLoader::loadCubemap(path);
}

void Skybox::draw(Context& context) {
    auto& shader = *shader_storage.get("skybox");

    context.setDepthFunc(GL_LEQUAL);
    context.setDepthMask(GL_FALSE);

    shader << UniformSampler{"skybox", cubemap};

    shader.use();

    assets.models.get("cube")->getMeshes()[0]->draw();

    context.setDepthFunc(GL_LESS);
    context.setDepthMask(GL_TRUE);
}
