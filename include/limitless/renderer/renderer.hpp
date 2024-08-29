#pragma once

#include <limitless/renderer/renderer_settings.hpp>
#include <limitless/renderer/renderer_pass.hpp>
#include <limitless/renderer/instance_renderer.hpp>

namespace Limitless {
    class Context;
    class Scene;
    class Context;
    class Assets;
    class Camera;

    class Renderer final {
    private:
        // renderer settings
        RendererSettings settings;

        // renderer resolution
        glm::uvec2 resolution;

        // set of renderer passes
        std::vector<std::unique_ptr<RendererPass>> passes;

        // instance renderer
        InstanceRenderer instance_renderer;

        Renderer() noexcept = default;
    public:
        /**
         * Updates renderer with new settings
         */
        void update(const RendererSettings& settings);

        /**
         * Renders scene
         */
        void render(Context& context, const Assets& assets, Scene& scene, Camera& camera);

        /**
         * Framebuffer size callback
         */
        void onFramebufferChange(glm::uvec2 size);

        [[nodiscard]] const RendererSettings& getSettings() const noexcept { return settings; }
        [[nodiscard]] const glm::uvec2& getResolution() const noexcept { return resolution; }
        [[nodiscard]] const InstanceRenderer& getInstanceRenderer() const noexcept { return instance_renderer; }

        /**
         * Sets of methods to handle RendererPasses
         */

        /**
         * Checks if specified pass is present in the renderer
         */
        template<typename RenderPass>
        bool isPresent() {
            for (const auto& pass : passes) {
                if (auto *p = dynamic_cast<RenderPass*>(pass.get()); p) {
                    return true;
                }
            }
            return false;
        }

        /**
         *  Returns typed RendererPass
         *
         *  throws render_pass_not_found if not present
         */
        template<typename RenderPass>
        auto& getPass() {
            for (const auto& pass : passes) {
                if (auto *p = dynamic_cast<RenderPass*>(pass.get()); p) {
                    return *p;
                }
            }

            throw render_pass_not_found(typeid(RenderPass).name());
        }

        class Builder {
        private:
            std::unique_ptr<Renderer> renderer;
        public:
            /**
             * Creates Builder with new instance of Renderer
             */
            Builder();

            /**
             * Creates Builder for specified Renderer
             *
             * takes ownership of passes
             *
             * note: use build(Renderer&) to transfer back
             */
            explicit Builder(Renderer& from);

            /**
             * Sets renderer resolution
             */
            Builder& resolution(glm::uvec2 resolution);

            /**
             * Sets renderer settings
             */
            Builder& settings(const RendererSettings& settings);

            /**
             * Adds RenderPass to the end of pipeline
             */
            Builder& addSceneUpdatePass();
            Builder& addDirectionalShadowPass();
            Builder& addDeferredFramebufferPass();
            Builder& addDepthPass();
            Builder& addColorPicker();
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

            /**
             * Constructs RenderPass at the end of pipeline
             */
            template<typename RenderPass, typename... Args>
            Builder& add(Args&&... args) {
                auto* pass = new RenderPass(*renderer, std::forward<Args>(args)...);
                renderer->passes.emplace_back(pass);
                return *this;
            }

            /**
             * Constructs RenderPass before specified Pass
             */
            template<typename Before, typename RenderPass, typename... Args>
            Builder& addBefore(Args&&... args) {
                auto it = std::find_if(renderer->passes.begin(), renderer->passes.end(), [](auto& pass) {
                    return dynamic_cast<Before*>(pass.get()) ? true : false;
                });

                if (it != renderer->passes.end()) {
                    auto* pass = new RenderPass(*renderer, std::forward<Args>(args)...);
                    renderer->passes.insert(it, pass);
                } else {
                    throw render_pass_not_found {"Cannot add RenderPass before specified element, does not exist"};
                }

                return *this;
            }

            /**
             * Constructs RenderPass after specified Pass
             */
            template<typename After, typename RenderPass, typename... Args>
            Builder& addAfter(Args&&... args) {
                auto it = std::find_if(renderer->passes.begin(), renderer->passes.end(), [](const auto& pass) {
                    return dynamic_cast<After*>(pass.get()) != nullptr ? true : false;
                });

                if (it != renderer->passes.end()) {
                    auto pass = std::make_unique<RenderPass>(*renderer, std::forward<Args>(args)...);
                    renderer->passes.insert(it++, std::move(pass));
                } else {
                    throw render_pass_not_found {"Cannot add RenderPass after specified element, does not exist"};
                }

                return *this;
            }

            /**
             * Removes RendererPass if present
             */
            template<typename RendererPass>
            Builder& remove() {
                auto it = std::find_if(renderer->passes.begin(), renderer->passes.end(), [](auto& pass) {
                    return dynamic_cast<RendererPass*>(pass.get()) != nullptr ? true : false;
                });

                if (it != renderer->passes.end()) {
                    renderer->passes.erase(it);
                }

                return *this;
            }

            /**
             * Constructs Deferred Renderer from settings
             */
            Builder& deferred();

            /**
             * Builds Renderer
             */
            std::unique_ptr<Renderer> build();

            /**
             * Changes current set of renderer passes according to settings
             */
            Builder& update();

            /**
             * Transfers built RenderPasses to specified Renderer
             */
            void build(Renderer& renderer);
        };

        static Builder builder() { return {}; }
    };
}
