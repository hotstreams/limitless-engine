#pragma once

#include <limitless/scene.hpp>

//#include <functional>
//#include <memory>
//#include <stdexcept>
//#include <glm/vec2.hpp>

namespace Limitless {
    class Instance;
    class RenderTarget;
    class UniformSetter;
    class Context;
    class Texture;
    class Camera;
    class Assets;
    class Scene;

    class Pipeline;

    /**
     *  PipelinePass describes some stage of pipeline that have to be updated, draw and has result
     */
    class PipelinePass {
    protected:
        /**
         * Pipeline ref
         */
        Pipeline& pipeline;
    public:
        explicit PipelinePass(Pipeline& pass) noexcept;
        virtual ~PipelinePass() = default;

        /**
         *  Returns result from current pipeline pass
         */
        virtual std::shared_ptr<Texture> getResult();

        /**
         *  Returns result from previous pass
         */
        std::shared_ptr<Texture> getPreviousResult();

        /**
         * Adds uniform setters for future passes
         */
        virtual void addSetter(UniformSetter& setter);

        /**
         * Render curent pass
         */
        virtual void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter);

        /**
         * Update current pass
         */
        virtual void update(Scene& scene, Instances& instances, Context& ctx, const Camera& camera);

        /**
         * Framebuffer change callback
         */
        virtual void onFramebufferChange(glm::uvec2 size);
    };
}