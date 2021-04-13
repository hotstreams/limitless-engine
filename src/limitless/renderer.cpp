#include <limitless/renderer.hpp>

#include <limitless/core/texture_builder.hpp>
#include <limitless/instances/abstract_instance.hpp>
#include <limitless/material_system/properties.hpp>
#include <limitless/util/renderer_helper.hpp>
#include <limitless/core/uniform_setter.hpp>
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
    : postprocess{context}
    , effect_renderer{context}
    , scene_data{context}
    , offscreen{context}
    , shadow_mapping{context} {
    initialize(context);
}

void Renderer::dispatch(Context& ctx, const Assets& assets, Scene& scene, MaterialShader shader, Camera& camera, const UniformSetter& setter) {
    auto instances = performFrustumCulling(scene, camera);

    // rendering OPAQUE objects front to back
    std::sort(instances.begin(), instances.end(), FrontToBackSorter{camera});
    dispatch(ctx, assets, scene, shader, Blending::Opaque, setter);

    // draws skybox
    if (scene.getSkybox()) {
        scene.getSkybox()->draw(ctx, assets);
    }

    // rendering TRANSLUCENT objects back to front
    std::sort(instances.begin(), instances.end(), BackToFrontSorter{camera});

    dispatch(ctx, assets, scene, shader, Blending::Additive, setter);
    dispatch(ctx, assets, scene, shader, Blending::Modulate, setter);
    dispatch(ctx, assets, scene, shader, Blending::Translucent, setter);
}

void Renderer::dispatch(Context& ctx, const Assets& assets, Scene& scene, MaterialShader shader, Blending blending, const UniformSetter& setter) {
    for (auto& [id, instance] : scene) {
        instance->draw(ctx, assets, shader, blending, setter);
    }

    effect_renderer.draw(assets, shader, blending, setter);
}

void Renderer::draw(Context& context, const Assets& assets, Scene& scene, Camera& camera) {
    scene.update();
    scene_data.update(context, camera);

    auto instances = performFrustumCulling(scene, camera);

    effect_renderer.update(instances);

    UniformSetter uniform_setter;

    // shadows pass
    if (RenderSettings::DIRECTIONAL_CSM) {
        shadow_mapping.castShadows(*this, assets, scene, context, camera);

        uniform_setter.add([&] (ShaderProgram& shader) {
            shadow_mapping.setUniform(shader);
        });
    }

    // forward pass
    context.setViewPort(context.getSize());

    offscreen.bind();
    offscreen.clear();

    dispatch(context, assets, scene, MaterialShader::Forward, camera, uniform_setter);

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