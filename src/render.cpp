#include <render.hpp>

#include <assets.hpp>
#include <render_settings.hpp>

using namespace GraphicsEngine;

void Render::dispatchInstances(Context& context, Scene& scene, MaterialShaderType shader_type, Blending blending) const {
    switch (blending) {
        case Blending::Opaque:
            context.enable(GL_DEPTH_TEST);
            context.setDepthFunc(GL_LESS);
            context.setDepthMask(GL_TRUE);
            context.disable(GL_BLEND);
            break;
        case Blending::Additive:
            context.enable(GL_DEPTH_TEST);
            context.setDepthFunc(GL_LESS);
            context.setDepthMask(GL_FALSE);
            context.enable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);
            break;
        case Blending::Modulate:
            context.enable(GL_DEPTH_TEST);
            context.setDepthFunc(GL_LESS);
            context.setDepthMask(GL_FALSE);
            context.enable(GL_BLEND);
            glBlendFunc(GL_DST_COLOR, GL_ZERO);
            break;
        case Blending::Translucent:
            context.enable(GL_DEPTH_TEST);
            context.setDepthFunc(GL_LESS);
            context.setDepthMask(GL_FALSE);
            context.enable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
    }

    for (const auto& [id, instance] : scene.instances) {
        instance->isWireFrame() ? context.setPolygonMode(GL_LINE) : context.setPolygonMode(GL_FILL);

        instance->draw(shader_type, blending);
    }
}

void Render::renderLightsVolume(Context& context, Scene& scene) const {
    if (scene.lighting.dynamic.point_lights.getLights().empty()) {
        return;
    }

    context.enable(GL_DEPTH_TEST);
    context.setDepthFunc(GL_LESS);
    context.setDepthMask(GL_FALSE);
    context.disable(GL_BLEND);

    static auto sphere_instance = ModelInstance(assets.models.get("sphere"), assets.materials.get("sphere"), glm::vec3(0.0f));

    context.setPolygonMode(GL_LINE);
    for (const auto& light : scene.lighting.dynamic.point_lights.getLights()) {
        sphere_instance.setPosition(light.position);
        sphere_instance.setScale(glm::vec3(light.radius));
        sphere_instance.draw(MaterialShaderType::Default, Blending::Opaque);
    }
    context.setPolygonMode(GL_FILL);
}

void Render::initializeOffscreenBuffer(ContextEventObserver& ctx) {
    auto color0 = TextureBuilder::build(Texture::Type::Tex2D, 1, Texture::InternalFormat::RGBA16F, ctx.getSize(), Texture::Format::RGBA, Texture::DataType::Float, nullptr);
    auto depth = TextureBuilder::build(Texture::Type::Tex2D, 1, Texture::InternalFormat::Depth32F, ctx.getSize(), Texture::Format::DepthComponent, Texture::DataType::Float, nullptr);

    offscreen << TextureAttachment{FramebufferAttachment::Color0, color0}
              << TextureAttachment{FramebufferAttachment::Depth, depth};

    offscreen.drawBuffer(FramebufferAttachment::Color0);
    offscreen.checkStatus();
    offscreen.unbind();
}

Render::Render(ContextEventObserver& context) noexcept
    : postprocess(context) {
    context.clearColor({ 0.5f, 0.5f, 0.5f, 1.0f });

    initializeOffscreenBuffer(context);
}

void Render::draw(Context& context, Scene& scene, Camera& camera) {
    scene.update();

    scene_data.update(context, scene, camera);

    context.setViewPort(context.getSize());

    context.enable(GL_DEPTH_TEST);
    context.setDepthFunc(GL_LESS);
    context.setDepthMask(GL_TRUE);
    context.disable(GL_BLEND);

    offscreen.bind();
    offscreen.clear();

    dispatchInstances(context, scene, MaterialShaderType::Default, Blending::Opaque);

    if (render_settings.light_radius) {
        renderLightsVolume(context, scene);
    }

    dispatchInstances(context, scene, MaterialShaderType::Default, Blending::Additive);
    dispatchInstances(context, scene, MaterialShaderType::Default, Blending::Modulate);
    dispatchInstances(context, scene, MaterialShaderType::Default, Blending::Translucent);

    offscreen.unbind();

    postprocess.process(context, offscreen);
}
