#pragma once

#include <functional>

namespace Limitless {
    class AbstractInstance;
    using Instances = std::vector<std::reference_wrapper<AbstractInstance>>;
    class RenderTarget;
    class UniformSetter;
    class Context;
    class Camera;
    class Assets;
    class Scene;

    class RenderPass {
    protected:
        RenderPass* prev_pass;
    public:
        explicit RenderPass(RenderPass *pass) noexcept;
        virtual ~RenderPass() = default;

        virtual RenderTarget& getTarget();
        virtual void addSetter(UniformSetter& setter);
        virtual void update(Scene& scene, Instances& instances, Context& ctx, const Camera& camera);
        virtual void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, const UniformSetter& setter);
    };
}