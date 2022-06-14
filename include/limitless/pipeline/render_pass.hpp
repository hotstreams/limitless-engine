#pragma once

#include <functional>
#include <memory>
#include <stdexcept>
#include <glm/vec2.hpp>

namespace Limitless {
    class AbstractInstance;
    using Instances = std::vector<std::reference_wrapper<AbstractInstance>>;
    class RenderTarget;
    class UniformSetter;
    class Context;
    class Texture;
    class Camera;
    class Assets;
    class Scene;

    class Pipeline;

    class RenderPass {
    protected:
        Pipeline& pipeline;
    public:
        explicit RenderPass(Pipeline& pass) noexcept;
        virtual ~RenderPass() = default;

        std::shared_ptr<Texture> getPreviousResult();
        virtual std::shared_ptr<Texture> getResult() { throw std::logic_error{"This RenderPass does not provide result method!"}; }

        virtual void addSetter(UniformSetter& setter);
        virtual void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter);

        virtual void update(Scene& scene, Instances& instances, Context& ctx, const Camera& camera);

        virtual void onFramebufferChange(glm::uvec2 size);
    };
}