#pragma once

#include <limitless/core/framebuffer.hpp>
#include <future>
#include <limitless/core/sync.hpp>
#include <limitless/core/uniform.hpp>
#include <limitless/scene.hpp>
#include <limitless/core/texture_builder.hpp>
#include <utility>
#include <limitless/instances/abstract_instance.hpp>
#include <limitless/core/uniform_setter.hpp>
#include <limitless/core/shader_program.hpp>
#include <limitless/ms/blending.hpp>
#include <list>
#include "sorter.hpp"

namespace Limitless {
    class ColorPicker {
    private:
        struct PickData {
            std::function<void(uint32_t)> callback;
            glm::uvec2 coords;
            Sync sync {};
        };
        std::list<PickData> data;

        Framebuffer framebuffer;

        static glm::vec3 convert(uint32_t id) noexcept {
            const auto r = (id & 0x000000FF) >> 0;
            const auto g = (id & 0x0000FF00) >> 8;
            const auto b = (id & 0x00FF0000) >> 16;
            return glm::vec3{r, g, b} / 255.0f;
        }

        static uint32_t convert(glm::uvec3 color) noexcept {
            return color.r + color.g * 256 + color.b * 256 * 256;
        }
    public:
        ColorPicker(glm::uvec2 frame_size, const std::shared_ptr<Texture>& depth)
            : framebuffer {Framebuffer::asRGB8LinearClampToEdgeWithDepth(frame_size, depth)} {
        }

        void process(Context& ctx);

        void onPick(Context& ctx, Assets& assets, Scene& scene, glm::uvec2 coords, std::function<void(uint32_t id)> callback);
    };
}