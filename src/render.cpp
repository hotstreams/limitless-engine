#include <render.hpp>

#include <assets.hpp>
#include <render_settings.hpp>
#include <core/texture_builder.hpp>
#include <material_system/material.hpp>
#include <shader_types.hpp>
#include <elementary_instance.hpp>
#include <camera.hpp>
#include <scene.hpp>

using namespace LimitlessEngine;

struct FrontToBackSorter {
    const Camera& camera;

    explicit FrontToBackSorter(const Camera& _camera) noexcept : camera{_camera} {}

    bool operator()(const AbstractInstance* const lhs, const AbstractInstance* const rhs) const noexcept {
        const auto& a_pos = lhs->getPosition();
        const auto& b_pos = rhs->getPosition();
        const auto& c_pos = camera.getPosition();

        return glm::distance(c_pos, a_pos) < glm::distance(c_pos, b_pos);
    }
};

struct BackToFrontSorter {
    const Camera& camera;

    explicit BackToFrontSorter(const Camera& _camera) noexcept : camera{_camera} {}

    bool operator()(const AbstractInstance* const lhs, const AbstractInstance* const rhs) const noexcept {
        const auto& a_pos = lhs->getPosition();
        const auto& b_pos = rhs->getPosition();
        const auto& c_pos = camera.getPosition();

        return glm::distance(c_pos, a_pos) > glm::distance(c_pos, b_pos);
    }
};

void Renderer::dispatchInstances(const std::vector<AbstractInstance*>& instances, Context& context, MaterialShader shader_type, Blending blending) const {
    for (auto* const instance : instances) {
        instance->isWireFrame() ? context.setPolygonMode(CullFace::FrontBack, PolygonMode::Line) : context.setPolygonMode(CullFace::FrontBack, PolygonMode::Fill);

        instance->draw(shader_type, blending);
    }
}

void Renderer::renderLightsVolume(Context& context, Scene& scene) const {
    if (scene.lighting.point_lights.empty()) {
        return;
    }

    context.enable(Capabilities::DepthTest);
    context.setDepthFunc(DepthFunc::Less);
    context.setDepthMask(DepthMask::False);
    context.disable(Capabilities::DepthTest);

    static auto sphere_instance = ElementaryInstance(assets.models["sphere"], assets.materials["default"], glm::vec3(0.0f));

    context.setPolygonMode(CullFace::FrontBack, PolygonMode::Line);
    for (const auto& light : scene.lighting.point_lights) {
        sphere_instance.setPosition(light.position);
        sphere_instance.setScale(glm::vec3(light.radius));
        sphere_instance.draw(MaterialShader::Default, Blending::Opaque);
    }
    context.setPolygonMode(CullFace::FrontBack, PolygonMode::Fill);
}

void Renderer::initializeOffscreenBuffer(ContextEventObserver& ctx) {
    auto param_set = [] (Texture& texture) {
        texture << TexParameter<GLint>{GL_TEXTURE_MAG_FILTER, GL_LINEAR}
                << TexParameter<GLint>{GL_TEXTURE_MIN_FILTER, GL_LINEAR}
                << TexParameter<GLint>{GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE}
                << TexParameter<GLint>{GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE};
    };
    auto color = TextureBuilder::build(Texture::Type::Tex2D, 1, Texture::InternalFormat::RGBA16F, ctx.getSize(), Texture::Format::RGBA, Texture::DataType::Float, nullptr, param_set);
    auto depth = TextureBuilder::build(Texture::Type::Tex2D, 1, Texture::InternalFormat::Depth32F, ctx.getSize(), Texture::Format::DepthComponent, Texture::DataType::Float, nullptr, param_set);

    offscreen.bind();
    offscreen << TextureAttachment{FramebufferAttachment::Color0, color}
              << TextureAttachment{FramebufferAttachment::Depth, depth};

    offscreen.drawBuffer(FramebufferAttachment::Color0);
    offscreen.checkStatus();
    offscreen.unbind();
}

Renderer::Renderer(ContextEventObserver& context)
    : postprocess{context}, effect_renderer{context}, offscreen{context} {
    initializeOffscreenBuffer(context);
}

void Renderer::draw(Context& context, Scene& scene, Camera& camera) {
    scene.update();
    scene_data.update(context, scene, camera);

    context.setViewPort(context.getSize());

    offscreen.bind();
    offscreen.clear();

    auto instances = performFrustumCulling(scene, camera);

    effect_renderer.update(instances);

    auto draw_scene = [&] (Blending blending) {
        dispatchInstances(instances, context, MaterialShader::Default, blending);
        effect_renderer.draw(blending);
    };

    // rendering front to back
    std::sort(instances.begin(), instances.end(), FrontToBackSorter{camera});
    draw_scene(Blending::Opaque);

    // draws skybox if it exists
    if (scene.getSkybox()) {
        scene.getSkybox()->draw(context);
    }

    // draws lights influence radius
    if (render_settings.light_radius) {
        renderLightsVolume(context, scene);
    }

    // rendering back to front to follow the translucent order
    std::sort(instances.begin(), instances.end(), BackToFrontSorter{camera});
    draw_scene(Blending::Additive);
    draw_scene(Blending::Modulate);
    draw_scene(Blending::Translucent);

    offscreen.unbind();

    postprocess.process(context, offscreen);
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
