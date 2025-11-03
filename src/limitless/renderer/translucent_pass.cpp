#include <limitless/renderer/translucent_pass.hpp>

#include <limitless/renderer/shader_type.hpp>
#include <limitless/util/sorter.hpp>
#include <limitless/instances/instance.hpp>
#include <limitless/fx/effect_renderer.hpp>
#include <limitless/assets.hpp>
#include "limitless/core/uniform/uniform.hpp"
#include "limitless/core/shader/shader_program.hpp"
#include "limitless/core/uniform/uniform_setter.hpp"
#include <limitless/core/texture/texture_builder.hpp>
#include <stdexcept>
#include <limitless/renderer/renderer.hpp>
#include <limitless/renderer/deferred_lighting_pass.hpp>
#include <limitless/renderer/deferred_framebuffer_pass.hpp>

using namespace Limitless;

TranslucentPass::TranslucentPass(Renderer& renderer)
    : RendererPass {renderer}
    , framebuffer {Framebuffer::asRGB16FNearestClampToEdgeWithDepth(renderer.getResolution(), renderer.getPass<DeferredFramebufferPass>().getDepth())} {
}

void TranslucentPass::render(
        InstanceRenderer& instance_renderer,
        Scene &scene,
        Context &ctx,
        const Assets &assets,
        const Camera &camera,
        UniformSetter &setter) {
    ProfilerScope profile_scope {"TranslucentPass"};

    std::array transparent = {
        ms::Blending::Additive,
        ms::Blending::Modulate,
        ms::Blending::Translucent
    };

    auto& background_fb = renderer.getPass<DeferredLightingPass>().getFramebuffer();

    framebuffer.blit(background_fb, Texture::Filter::Nearest);

    framebuffer.bind();

    setter.add([&] (ShaderProgram& shader) {
        shader.setUniform("_refraction_texture", renderer.getPass<DeferredLightingPass>().getResult());
    });

    for (const auto& blending : transparent) {
        instance_renderer.renderScene({ctx, assets, ShaderType::Forward, blending, setter});
    }
}

std::shared_ptr<Texture> TranslucentPass::getResult() {
    return framebuffer.get(FramebufferAttachment::Color0).texture;
}

void TranslucentPass::onFramebufferChange(glm::uvec2 size) {
    framebuffer.onFramebufferChange(size);

    framebuffer << TextureAttachment{FramebufferAttachment::Depth, renderer.getPass<DeferredFramebufferPass>().getDepth()};
}
