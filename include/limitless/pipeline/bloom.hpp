#pragma once

#include <limitless/core/framebuffer.hpp>
#include <limitless/pipeline/blur.hpp>

namespace Limitless {
    class Bloom final {
    public:
        Blur blur;

        float threshold {1.0f};
        float strength {1.0f};
    private:
        Framebuffer brightness;

        void extractBrightness(const Assets& ctx, const std::shared_ptr<Texture>& image);
    public:
        explicit Bloom(glm::uvec2 frame_size);

        void process(Context& ctx, const Assets& assets, const std::shared_ptr<Texture>& image);
        [[nodiscard]] const std::shared_ptr<Texture>& getResult() const noexcept;

        void onFramebufferChange(glm::uvec2 frame_size);
    };
}