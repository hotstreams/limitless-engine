#pragma once

#include <limitless/fx/modules/module.hpp>

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
        std::chrono::time_point<std::chrono::steady_clock> last_time;
        bool first_update {false};
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

        void update([[maybe_unused]] AbstractEmitter& emitter, std::vector<Particle>& particles, [[maybe_unused]] float dt, [[maybe_unused]] Context& ctx, [[maybe_unused]] const Camera& camera) noexcept override {
            if (first_update) {
                last_time = std::chrono::steady_clock::now();
                first_update = false;
            }

            auto current_time = std::chrono::steady_clock::now();

            if (std::chrono::duration_cast<std::chrono::duration<float>>(current_time - last_time).count() >= (1.0f / fps)) {
                for (auto& p : particles) {

                    auto current_frame = glm::vec2{p.subUV.z, p.subUV.w};
                    auto it = std::find(frames.begin(), frames.end(), current_frame);

                    auto next_frame = (*it == frames.back()) ? frames[0] : *(++it);

                    p.subUV.z = next_frame.x;
                    p.subUV.w = next_frame.y;
                }

                last_time = current_time;
            }
        }

        [[nodiscard]] const auto& getTextureSize() const { return texture_size; }
        void setTextureSize(const glm::vec2& size) { texture_size = size; updateFrames(); }

        [[nodiscard]] const auto& getFrameCount() const { return frame_count; }
        void setFrameCount(const glm::vec2& count) { frame_count = count; updateFrames(); }

        [[nodiscard]] auto& getFPS() const noexcept { return fps; }
    };
}