#include <limitless/renderer/translucent_pass.hpp>

#include <limitless/renderer/shader_type.hpp>
#include <limitless/renderer/indirect_instance_renderer.hpp>
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

#include "limitless/core/profiler.hpp"

using namespace Limitless;

TranslucentPass::TranslucentPass(Renderer& renderer)
    : RendererPass {renderer}
    , framebuffer {Framebuffer::asRGB16FNearestClampToEdgeWithDepth(renderer.getResolution(), renderer.getPass<DeferredFramebufferPass>().getDepth())} {
}

void TranslucentPass::render(
        InstanceRenderer& instance_renderer,
        [[maybe_unused]] Scene &scene,
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

    // Use indirect draw if enabled (prepare() called once in Renderer::render)
    const bool use_indirect = renderer.getSettings().indirect_draw;

    for (const auto& blending : transparent) {
        DrawParameters drawp {ctx, assets, ShaderType::Forward, blending, setter};

        if (use_indirect) {
            // Render batched translucent instances via indirect draw
            renderer.getIndirectInstanceRenderer().render(drawp);
            // Fallback: render non-batched translucent instances via legacy (sorted for correct alpha)
            if (renderer.getSettings().sorted_rendering) {
                instance_renderer.renderVisibleNonBatchedNonTerrainSorted(drawp);
            } else {
                instance_renderer.renderVisibleNonBatchedNonTerrain(drawp);
            }
        } else if (renderer.getSettings().sorted_rendering) {
            instance_renderer.renderSceneSorted(drawp);
        } else {
            instance_renderer.renderScene(drawp);
        }
    }
}

std::shared_ptr<Texture> TranslucentPass::getResult() {
    return framebuffer.get(FramebufferAttachment::Color0).texture;
}

void TranslucentPass::onFramebufferChange(glm::uvec2 size) {
    framebuffer.onFramebufferChange(size);

    framebuffer << TextureAttachment{FramebufferAttachment::Depth, renderer.getPass<DeferredFramebufferPass>().getDepth()};
}
