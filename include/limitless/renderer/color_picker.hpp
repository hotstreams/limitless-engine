#pragma once

#include <limitless/core/framebuffer.hpp>
#include <future>
#include <limitless/core/sync.hpp>
#include "limitless/core/uniform/uniform.hpp"
#include <limitless/scene.hpp>
#include <limitless/core/texture/texture_builder.hpp>
#include <utility>
#include <limitless/instances/instance.hpp>
#include "limitless/core/uniform/uniform_setter.hpp"
#include "limitless/core/shader/shader_program.hpp"
#include <limitless/ms/blending.hpp>
#include <limitless/renderer/renderer_pass.hpp>
#include <list>

namespace Limitless {
    class ColorPicker : public RendererPass {
    private:
        struct PickData {
            std::function<void(uint32_t)> callback;
            glm::uvec2 coords;
            Sync sync {};
        };
        std::list<PickData> data;

        Framebuffer framebuffer;

        static uint32_t convert(glm::uvec3 color) noexcept;

        void process(Context& ctx);
    public:
        explicit ColorPicker(Renderer& renderer);

        void onPick(Context& ctx, glm::uvec2 coords, std::function<void(uint32_t)> callback);

        void render(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera, UniformSetter &setter) override;

        void onFramebufferChange(glm::uvec2 size) override;
    };
}