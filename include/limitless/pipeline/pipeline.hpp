#pragma once

#include <limitless/pipeline/pipeline_pass.hpp>
#include <glm/glm.hpp>

#include <vector>
#include <memory>
#include <stdexcept>

namespace Limitless {
    class RendererSettings;
    class InstanceRenderer;
    class Context;

    class pipeline_pass_not_found final : public std::logic_error {
    public:
        using std::logic_error::logic_error;
    };
}

namespace Limitless {
    /**
     * Pipeline describes set of passes that is needed to render the whole scene
     *
     * note: last pass should be ScreenPass to be able to set custom render target
     */
    class Pipeline {
    protected:
        /**
         * Set of passes
         */
        std::vector<std::unique_ptr<PipelinePass>> passes;

        RenderTarget* target {};
        glm::uvec2 size;
    public:
        /**
         * Initializes pipeline to render to specified render target
         */
        explicit Pipeline(glm::uvec2 size, RenderTarget& target) noexcept;
        virtual ~Pipeline() = default;

        /**
         * Changes render target of last ScreenPass
         */
        void setTarget(RenderTarget& target) noexcept;

        /**
         * Gets current render target
         */
        RenderTarget& getTarget() noexcept;

        /**
         * Updates pipeline with specified settings
         */
        virtual void update(Context& ctx, const RendererSettings& settings) = 0;

        /**
         * Framebuffer size callback
         */
        virtual void onFramebufferChange(glm::uvec2 size);

        /**
         * Updates all passes and sequentially invokes them
         */
        void draw(InstanceRenderer& renderer, Context& context, const Assets& assets, Scene& scene, Camera& camera);

        /**
         * Clears passes
         */
        void clear();

        /**
         * Adds pass to current pipeline
         *
         * Example:
         *      pipeline.add<GBufferPass>(args);
         */
        template<typename Pass, typename... Args>
        Pass& add(Args&&... args) {
            auto* pass = new Pass(*this, std::forward<Args>(args)...);
            passes.emplace_back(pass);
            return *pass;
        }

        /**
         * Tries to get specified Pass
         *
         * throws pipeline_pass_not_found if not found
         */
        template<typename Pass>
        auto& get() {
            for (const auto& pass : passes) {
                if (auto *p = dynamic_cast<Pass*>(pass.get()); p) {
                    return *p;
                }
            }

            throw pipeline_pass_not_found(typeid(Pass).name());
        }

        /**
         * Gets previous pass
         */
        std::unique_ptr<PipelinePass>& getPrevious(PipelinePass* curr);
    };

    class RendererPipeline {
    private:
        std::vector<std::unique_ptr<PipelinePass>> passes;
        glm::uvec2 resolution;
    public:
        /**
         * Updates pipeline with specified settings
         */
        void update(Context& ctx, const RendererSettings& settings);

        /**
         * Updates all passes and sequentially invokes them
         */
        void draw(InstanceRenderer& renderer, Context& context, const Assets& assets, Scene& scene, Camera& camera);

        /**
         * Framebuffer size callback
         */
        void onFramebufferChange(glm::uvec2 size);

        class Builder {
        private:
            std::vector<std::unique_ptr<PipelinePass>> passes;
        public:
            Builder add(std::unique_ptr<PipelinePass> pass) { return *this; }
            Builder add(PipelinePassFunction) { return *this; }

            Builder& addSceneUpdatePass();
            Builder& addDirectionalShadowPass();
            Builder& addDeferredFramebufferPass();
            Builder& addDepthPass();
            Builder& addGBufferPass();
            Builder& addDecalPass();
            Builder& addSkyboxPass();
            Builder& addSSAOPass();
            Builder& addSSRPass();
            Builder& addDeferredLightingPass();
            Builder& addTranslucentPass();
            Builder& addBloomPass();
            Builder& addOutlinePass();
            Builder& addCompositePass();
            Builder& addFXAAPass();
            Builder& addScreenPass();
            Builder& addRenderDebugPass();

            Builder& addBefore();
            Builder& addAfter();
            Builder& addAt();

            Builder& deferred();

            RendererPipeline build();
        };

        static Builder builder() { return {}; }
    };
}