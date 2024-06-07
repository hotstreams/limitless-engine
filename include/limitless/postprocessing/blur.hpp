#pragma once

#include <limitless/core/framebuffer.hpp>

namespace Limitless {
    class Assets;
    class Context;

    class Blur final {
    private:
        uint8_t BASE_ITERATION_COUNT = 8;
        uint8_t ITERATION_COUNT = BASE_ITERATION_COUNT;

        std::vector<Framebuffer> outRT;
        std::vector<Framebuffer> stageRT;

        std::shared_ptr<Texture> out;
        std::shared_ptr<Texture> stage;

        void build(glm::uvec2 frame_size);
        void downsample(Context& ctx, const Assets& assets, const std::shared_ptr<Texture>& source);
        void upsample(Context& ctx, const Assets& assets);
    public:
        explicit Blur(glm::uvec2 frame_size);

        void setIterationCount(uint8_t count) noexcept;
        [[nodiscard]] uint8_t getIterationCount() const noexcept;

        void process(Context& ctx, const Assets& assets, const std::shared_ptr<Texture>& image);

        [[nodiscard]] const std::shared_ptr<Texture>& getResult() const noexcept;

        void onFramebufferChange(glm::uvec2 frame_size);
    };
}