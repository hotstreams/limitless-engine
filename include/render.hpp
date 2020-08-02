#pragma once

#include <scene_data.hpp>
#include <scene.hpp>
#include <render_settings.hpp>
#include <assets.hpp>
#include <framebuffer.hpp>
#include "postprocessing.hpp"

namespace GraphicsEngine {
    class Render {
    private:
        SceneDataStorage scene_data;
        Framebuffer offscreen;

        void dispatchInstances(Context& context, Scene& scene, MaterialShaderType shader_type, Blending blending) const {
            for (const auto& [id, instance] : scene.instances) {
                instance->isWireFrame() ? context.setPolygonMode(GL_LINE) : context.setPolygonMode(GL_FILL);

                instance->draw(shader_type, blending);
            }
        }

        void renderLightsVolume(Context& context, Scene& scene) const {
            context.setPolygonMode(GL_LINE);
            auto sphere_instance =  ModelInstance(assets.models.get("sphere"), assets.materials.get("sphere"), glm::vec3(0.0f));
            for (const auto& light : scene.lighting.dynamic.point_lights.getLights()) {
                sphere_instance.setPosition(light.position);
                sphere_instance.setScale(glm::vec3(light.radius));
                sphere_instance.draw(MaterialShaderType::Default, Blending::Opaque);
            }
            context.setPolygonMode(GL_FILL);
        }

        void initializeOffscreenBuffer(ContextEventObserver& ctx) {
            auto color0 = TextureBuilder::build(Texture::Type::Tex2D, 1, Texture::InternalFormat::RGBA16F, ctx.getSize(), Texture::Format::RGBA, Texture::DataType::Float, nullptr);
            auto depth = TextureBuilder::build(Texture::Type::Tex2D, 1, Texture::InternalFormat::Depth16, ctx.getSize(), Texture::Format::DepthComponent, Texture::DataType::Float, nullptr);

            offscreen << TextureAttachment{FramebufferAttachment::Color0, color0}
                      << TextureAttachment{FramebufferAttachment::Depth, depth};
            offscreen.drawBuffer(FramebufferAttachment::Color0);
            offscreen.checkStatus();
        }
    public:
        explicit Render(ContextEventObserver& context) noexcept {
            context.clearColor({ 0.5f, 0.5f, 0.5f, 1.0f});

            initializeOffscreenBuffer(context);
        }

        void draw(Context& context, Scene& scene, Camera& camera) {
            scene.update();

            scene_data.update(context, scene, camera);

            context.setViewPort(context.getSize());

            offscreen.bind();

            context.clear(Clear::ColorDepth);

            context.enable(GL_DEPTH_TEST);
            context.setDepthFunc(GL_LEQUAL);

            dispatchInstances(context, scene, MaterialShaderType::Default, Blending::Opaque);

            context.enable(GL_BLEND);
            context.setDepthMask(GL_FALSE);
            glBlendFunc(GL_ONE, GL_ONE);

            dispatchInstances(context, scene, MaterialShaderType::Default, Blending::Additive);

            context.setDepthMask(GL_TRUE);
            context.disable(GL_BLEND);

            context.enable(GL_BLEND);
            context.setDepthMask(GL_FALSE);
            glBlendFunc(GL_DST_COLOR, GL_ZERO);

            dispatchInstances(context, scene, MaterialShaderType::Default, Blending::Modulate);

            context.setDepthMask(GL_FALSE);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            dispatchInstances(context, scene, MaterialShaderType::Default, Blending::Translucent);

            context.setDepthMask(GL_TRUE);

            if (render_settings.light_radius) {
                renderLightsVolume(context, scene);
            }

            offscreen.unbind();

            context.clear(Clear::ColorDepth);

            context.disable(GL_DEPTH_TEST);

            static PostProcessing postprocess(static_cast<ContextEventObserver&>(context));
            postprocess.process(offscreen.get(FramebufferAttachment::Color0).texture);
        }
    };
}