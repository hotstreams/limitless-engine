#pragma once

#include <limitless/fx/modules/module.hpp>

#include <algorithm>

namespace Limitless::fx {
    template<typename Particle>
    class SubUV : public Module<Particle> {
    private:
        // uv of all frames
        std::vector<glm::vec2> frames;
        // rendering frames per second
        float fps;
        // scaling factor to frame-sprite space
        glm::vec2 subUV_factor;
        // last time updated
        float frame_time_accum {0.f};
        // texture size
        glm::vec2 texture_size;
        // frame count
        glm::vec2 frame_count;

        void updateFrames() noexcept {
            uint32_t width = texture_size.x / frame_count.x;
            uint32_t height = texture_size.y / frame_count.y;

            frames.clear();
            for (uint32_t i = 0; i < frame_count.x; ++i)
                for (uint32_t j = 0; j < frame_count.y; ++j)
                    frames.emplace_back(glm::vec2{
                            static_cast<float>(j) * static_cast<float>(width) / texture_size.x,
                            static_cast<float>(i) * static_cast<float>(height) / texture_size.y});

            subUV_factor.x = static_cast<float>(width) / texture_size.x;
            subUV_factor.y = static_cast<float>(height) / texture_size.y;
        }
    public:
        explicit SubUV(const glm::vec2& _texture_size, float _fps, const glm::vec2& _frame_count) noexcept
            : Module<Particle>(ModuleType::SubUV)
            , fps {_fps}
            , subUV_factor {1.0f}
            , texture_size {_texture_size}
            , frame_count {_frame_count} {
            updateFrames();
        }

        ~SubUV() override = default;

        SubUV(const SubUV& module) = default;

        [[nodiscard]] SubUV* clone() const override {
            return new SubUV(*this);
        }

        void initialize([[maybe_unused]] AbstractEmitter& emitter, Particle& particle, [[maybe_unused]] size_t index) noexcept override {
            particle.subUV.x = subUV_factor.x;
            particle.subUV.y = subUV_factor.y;
            particle.subUV.z = frames[0].x;
            particle.subUV.w = frames[0].y;
        }

        void update([[maybe_unused]] AbstractEmitter &emitter, std::vector<Particle> &particles, float dt, [[maybe_unused]] const Camera &camera) noexcept override {
            if (fps <= 0.0f || frames.empty()) {
                return;
            }

            frame_time_accum += dt;
            const auto interval = 1.0f / fps;
            size_t steps = 0;
            const auto max_steps = frames.size();
            while (frame_time_accum >= interval && steps < max_steps) {
                for (auto& p : particles) {
                    auto current_frame = glm::vec2{p.subUV.z, p.subUV.w};
                    auto it = std::find(frames.begin(), frames.end(), current_frame);

                    auto next_frame = (*it == frames.back()) ? frames[0] : *(++it);

                    p.subUV.z = next_frame.x;
                    p.subUV.w = next_frame.y;
                }

                frame_time_accum -= interval;
                ++steps;
            }
        }

        [[nodiscard]] const auto& getTextureSize() const { return texture_size; }
        void setTextureSize(const glm::vec2& size) { texture_size = size; updateFrames(); }

        [[nodiscard]] const auto& getFrameCount() const { return frame_count; }
        void setFrameCount(const glm::vec2& count) { frame_count = count; updateFrames(); }

        [[nodiscard]] auto& getFPS() const noexcept { return fps; }
    };
}