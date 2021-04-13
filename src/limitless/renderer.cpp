#include <limitless/renderer.hpp>

#include <limitless/core/texture_builder.hpp>
#include <limitless/instances/abstract_instance.hpp>
#include <limitless/material_system/properties.hpp>
#include <limitless/util/renderer_helper.hpp>
#include <limitless/util/sorter.hpp>
#include <limitless/scene.hpp>

using namespace LimitlessEngine;

void Renderer::initialize(ContextEventObserver& ctx) {
    auto param_set = [] (Texture& texture) {
        texture << TexParameter<GLint>{GL_TEXTURE_MAG_FILTER, GL_LINEAR}
                << TexParameter<GLint>{GL_TEXTURE_MIN_FILTER, GL_LINEAR}
                << TexParameter<GLint>{GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE}
                << TexParameter<GLint>{GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE};
    };

    TextureBuilder builder;
    auto color = builder.setTarget(Texture::Type::Tex2D)
                        .setInternalFormat(Texture::InternalFormat::RGBA16F)
                        .setSize(ctx.getSize())
                        .setFormat(Texture::Format::RGBA)
                        .setDataType(Texture::DataType::Float)
                        .setParameters(param_set)
                        .build();

    auto depth = builder.setTarget(Texture::Type::Tex2D)
                        .setInternalFormat(Texture::InternalFormat::Depth32F)
                        .setSize(ctx.getSize())
                        .setFormat(Texture::Format::DepthComponent)
                        .setDataType(Texture::DataType::Float)
                        .setParameters(param_set)
                        .build();

    offscreen.bind();
    offscreen << TextureAttachment{FramebufferAttachment::Color0, color}
              << TextureAttachment{FramebufferAttachment::Depth, depth};

    offscreen.drawBuffer(FramebufferAttachment::Color0);
    offscreen.checkStatus();
    offscreen.unbind();
}

Renderer::Renderer(ContextEventObserver& context)
    : postprocess{context}, effect_renderer{context}, scene_data{context}, offscreen{context}, shadow_mapping{context} {
    initialize(context);
}

void Renderer::draw(Context& context, const Assets& assets, Scene& scene, Camera& camera) {
    scene.update();
    scene_data.update(context, camera);

    auto instances = performFrustumCulling(scene, camera);

    effect_renderer.update(instances);

    // shadows pass
    shadow_mapping.castShadows(*this, assets, scene, context, camera);

    // forward pass
    context.setViewPort(context.getSize());

    offscreen.bind();
    offscreen.clear();

    auto draw_scene = [&] (Blending blending) {
        auto setter = [&] (ShaderProgram& shader) {
            shadow_mapping.setUniform(shader);
        };

        dispatch(scene, context, assets, MaterialShader::Forward, blending, {setter});
        effect_renderer.draw(assets, blending);
    };

    // rendering front to back
    std::sort(instances.begin(), instances.end(), FrontToBackSorter{camera});
    draw_scene(Blending::Opaque);

    // draws skybox if it exists
    if (scene.getSkybox()) {
        scene.getSkybox()->draw(context, assets);
    }

    // rendering back to front to follow the translucent order
    std::sort(instances.begin(), instances.end(), BackToFrontSorter{camera});
    draw_scene(Blending::Additive);
    draw_scene(Blending::Modulate);
    draw_scene(Blending::Translucent);

    RendererHelper::render(context, assets, scene);

    offscreen.unbind();

    postprocess.process(context, assets, offscreen);
}

std::vector<AbstractInstance*> Renderer::performFrustumCulling(Scene &scene, [[maybe_unused]] Camera& camera) const noexcept {
    //TODO: culling
    std::vector<AbstractInstance*> culled;
    culled.reserve(scene.size());

    for (const auto& [id, instance] : scene) {
        culled.emplace_back(instance.get());
    }

    return culled;
}

void Renderer::dispatch(Scene& scene, Context& context, const Assets& assets, MaterialShader shader_type, Blending blending, const UniformSetter& uniform_set) const {
    for (auto& [id, instance] : scene) {
        instance->isWireFrame() ? context.setPolygonMode(CullFace::FrontBack, PolygonMode::Line) : context.setPolygonMode(CullFace::FrontBack, PolygonMode::Fill);

        instance->draw(assets, shader_type, blending, uniform_set);
    }
}
