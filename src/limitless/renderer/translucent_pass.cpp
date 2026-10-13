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
#include <limitless/core/cpu_profiler.hpp>

using namespace Limitless;

TranslucentPass::TranslucentPass(Renderer& renderer)
    : RendererPass {renderer}
    , framebuffer {Framebuffer::asRGB16FNearestClampToEdgeWithDepth(renderer.getResolution(), renderer.getPass<DeferredFramebufferPass>().getDepth())}
    , scene_depth_copy {Texture::Builder::asDepth32F(renderer.getResolution())} {
}

void TranslucentPass::copySceneDepth() {
    const auto& depth = *renderer.getPass<DeferredFramebufferPass>().getDepth();
    const auto size   = depth.getSize();
    glCopyImageSubData(
        depth.getId(), GL_TEXTURE_2D, 0, 0, 0, 0,
        scene_depth_copy->getId(), GL_TEXTURE_2D, 0, 0, 0, 0,
        static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y), 1
    );
}

void TranslucentPass::render(
        InstanceRenderer& instance_renderer,
        [[maybe_unused]] Scene &scene,
        Context &ctx,
        const Assets &assets,
        [[maybe_unused]] const Camera &camera,
        UniformSetter &setter) {

    CpuProfileScope scope(global_profiler, "TranslucentPass::render");

    std::array transparent = {
        ms::Blending::Additive,
        ms::Blending::Modulate,
        ms::Blending::Translucent
    };

    auto& background_fb = renderer.getPass<DeferredLightingPass>().getFramebuffer();

    framebuffer.blit(background_fb, Texture::Filter::Nearest);

    copySceneDepth();

    framebuffer.bind();

    setter.add([&] (ShaderProgram& shader) {
        shader.setUniform("refraction_texture", renderer.getPass<DeferredLightingPass>().getResult());
        shader.setUniform("scene_depth_texture", scene_depth_copy);
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
    scene_depth_copy = Texture::Builder::asDepth32F(size);

    framebuffer << TextureAttachment{FramebufferAttachment::Depth, renderer.getPass<DeferredFramebufferPass>().getDepth()};
}
