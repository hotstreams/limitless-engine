#pragma once

#include <limitless/scene.hpp>
#include <limitless/renderer/instance_renderer.hpp>

namespace Limitless {
    class Instance;
    class RenderTarget;
    class UniformSetter;
    class Context;
    class Texture;
    class Camera;
    class Assets;
    class Scene;

    class Renderer;

    class render_pass_not_found final : public std::logic_error {
    public:
        using std::logic_error::logic_error;
    };

    /**
     *  RendererPass describes some stage of pipeline that have to be updated, rendered
     */
    class RendererPass {
    protected:
        /**
         * Renderer ref
         */
        Renderer& renderer;
    public:
        explicit RendererPass(Renderer& renderer) noexcept;
        virtual ~RendererPass() = default;

        /**
         * Updates current pass
         */
        virtual void update(Scene &scene, const Camera &camera);

        /**
         * Updates pass with new renderer settings
         */
        virtual void update(const RendererSettings& settings);

        /**
        * Render current pass
        */
        virtual void render(InstanceRenderer& renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera, UniformSetter &setter);

        /**
         * Adds uniform setters for future passes
         */
        virtual void addUniformSetter(UniformSetter& setter);

        /**
         * Framebuffer change callback
         */
        virtual void onFramebufferChange(glm::uvec2 size);
    };
}