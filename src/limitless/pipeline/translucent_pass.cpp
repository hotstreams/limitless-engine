#include <limitless/pipeline/translucent_pass.hpp>

#include <limitless/pipeline/shader_pass_types.hpp>
#include <limitless/util/sorter.hpp>
#include <limitless/instances/abstract_instance.hpp>
#include <limitless/fx/effect_renderer.hpp>
#include <limitless/assets.hpp>
#include <limitless/core/uniform.hpp>
#include <limitless/core/shader_program.hpp>
#include <limitless/core/uniform_setter.hpp>
#include <limitless/core/texture_builder.hpp>
#include <stdexcept>
#include <limitless/pipeline/pipeline.hpp>
#include <limitless/pipeline/deferred_lighting_pass.hpp>

using namespace Limitless;

TranslucentPass::TranslucentPass(Pipeline& pipeline, fx::EffectRenderer& _renderer, glm::uvec2 frame_size)
    : RenderPass(pipeline)
    , framebuffer {Framebuffer::asRGB16FNearestClampToEdge(frame_size)}
    , renderer {_renderer} {
}

void TranslucentPass::sort(Instances& instances, const Camera& camera, ms::Blending blending) {
    switch (blending) {
        case ms::Blending::Opaque:
            std::sort(instances.begin(), instances.end(), FrontToBackSorter{camera});
            break;
        case ms::Blending::Translucent:
        case ms::Blending::Additive:
        case ms::Blending::Modulate:
            std::sort(instances.begin(), instances.end(), BackToFrontSorter{camera});
            break;
        case ms::Blending::MultipleOpaque:
        case ms::Blending::Text:
            throw std::logic_error("This type of blending cannot be used as ColorPass value");
    }
}

void TranslucentPass::draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) {
    std::array transparent = {
        ms::Blending::Additive,
        ms::Blending::Modulate,
        ms::Blending::Translucent
    };

    framebuffer.blit(pipeline.get<DeferredLightingPass>().getFramebuffer(), Texture::Filter::Nearest);

    setter.add([&] (ShaderProgram& shader) {
        shader << UniformSampler{"refraction_texture", getPreviousResult()};
    });

    for (const auto& blending : transparent) {
        sort(instances, camera, blending);

        for (auto& instance : instances) {
            instance.get().draw(ctx, assets, ShaderPass::Forward, blending, setter);
        }

        renderer.draw(ctx, assets, ShaderPass::Forward, blending, setter);
    }
}

std::shared_ptr<Texture> TranslucentPass::getResult() {
    return framebuffer.get(FramebufferAttachment::Color0).texture;
}

void TranslucentPass::onFramebufferChange(glm::uvec2 size) {
    framebuffer.onFramebufferChange(size);
}
