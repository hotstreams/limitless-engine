#include <limitless/renderer.hpp>

#include <limitless/render_settings.hpp>
#include <limitless/core/texture_builder.hpp>
#include <limitless/material_system/material.hpp>
#include <limitless/instances/elementary_instance.hpp>
#include <limitless/camera.hpp>
#include <limitless/scene.hpp>

#include <limitless/models/elementary_model.hpp>

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

//void Renderer::dispatchInstances(Context& context,
//                                 const Assets& assets,
//                                 MaterialShader shader_type,
//                                 Blending blending,
//                                 UniformSetter& uniform_set) const {
////    for (auto* const instance : instances) {
////        instance->isWireFrame() ? context.setPolygonMode(CullFace::FrontBack, PolygonMode::Line) : context.setPolygonMode(CullFace::FrontBack, PolygonMode::Fill);
////
////        instance->draw(assets, shader_type, blending, uniform_setter);
////    }
//
//
//}

void Renderer::renderLightsVolume(Context& context, Scene& scene, const Assets& assets) const {
    if (scene.lighting.point_lights.empty()) {
        return;
    }

    context.enable(Capabilities::DepthTest);
    context.setDepthFunc(DepthFunc::Less);
    context.setDepthMask(DepthMask::False);
    context.disable(Capabilities::DepthTest);

    auto sphere_instance = ElementaryInstance(assets.models.at("sphere"), assets.materials.at("default"), glm::vec3(0.0f));

    context.setPolygonMode(CullFace::FrontBack, PolygonMode::Line);
    for (const auto& light : scene.lighting.point_lights) {
        sphere_instance.setPosition(light.position);
        sphere_instance.setScale(glm::vec3(light.radius));
        sphere_instance.draw(assets, MaterialShader::Forward, Blending::Opaque);
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
    initializeOffscreenBuffer(context);
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

    // draws lights influence radius
    if (RenderSettings::LIGHT_RADIUS) {
        renderLightsVolume(context, scene, assets);
    }

    // rendering back to front to follow the translucent order
    std::sort(instances.begin(), instances.end(), BackToFrontSorter{camera});
    draw_scene(Blending::Additive);
    draw_scene(Blending::Modulate);
    draw_scene(Blending::Translucent);

    if (RenderSettings::COORDINATE_SYSTEM_AXES) {
        renderCoordinateSystemAxes(context, scene, assets);
    }

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

void Renderer::renderCoordinateSystemAxes(Context& context, Scene& scene, const Assets& assets) const {
    context.enable(Capabilities::DepthTest);
    context.setDepthFunc(DepthFunc::Less);
    context.setDepthMask(DepthMask::False);
    context.disable(Capabilities::DepthTest);

    glLineWidth(5.0f);

    static const auto x = std::make_shared<Line>(glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{1.0f, 0.0f, 0.0f});
    static const auto y = std::make_shared<Line>(glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 1.0f, 0.0f});
    static const auto z = std::make_shared<Line>(glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 0.0f, 1.0f});

    static ElementaryInstance x_i {x, assets.materials.at("green"), {5.0f, 1.0f, 0.0f}};
    static ElementaryInstance y_i {y, assets.materials.at("blue"), {5.0f, 1.0f, 0.0f}};
    static ElementaryInstance z_i {z, assets.materials.at("red"), {5.0f, 1.0f, 0.0f}};

    x_i.draw(assets, MaterialShader::Forward, Blending::Opaque);
    y_i.draw(assets, MaterialShader::Forward, Blending::Opaque);
    z_i.draw(assets, MaterialShader::Forward, Blending::Opaque);
}
