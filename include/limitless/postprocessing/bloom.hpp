#pragma once

#include <limitless/core/framebuffer.hpp>

#include <cstdint>
#include <memory>
#include <vector>

namespace Limitless {
    class Assets;
    class Context;
    class RendererSettings;
    class Texture;

    /**
     * Filament-style bloom: fixed-aspect bloom plane, mip pyramid with 9-tap downsample
     * and COD / Jimenez upsample (blur_upsample). First pass matches Filament downsample2x
     * filtering + soft threshold + optional fireflies + highlight compression.
     */
    class Bloom final {
    public:
        float strength {1.0f};

        Bloom();

        void process(Context& ctx, const Assets& assets, const std::shared_ptr<Texture>& hdr_image, const RendererSettings& settings);

        [[nodiscard]] std::shared_ptr<Texture> getResult() const noexcept { return tex_out; }

        void onFramebufferChange(glm::uvec2 screen_size);

        [[nodiscard]] uint32_t getLevelCount() const noexcept { return levels; }

    private:
        glm::uvec2 screen_ {0};
        glm::uvec2 bloom_wh_ {0};
        uint32_t levels {8};

        uint32_t cached_bloom_buffer_height_ {0};
        uint32_t cached_bloom_levels_req_ {0};

        std::shared_ptr<Texture> tex_out;
        std::shared_ptr<Texture> tex_stage;

        std::vector<Framebuffer> out_rt;
        std::vector<Framebuffer> stage_rt;

        void rebuild(const RendererSettings& settings);
        [[nodiscard]] bool needs_rebuild(const RendererSettings& settings) const noexcept;

        void run_prefilter(Context& ctx, const Assets& assets, const std::shared_ptr<Texture>& hdr,
            const RendererSettings& settings);
        void run_downsample(Context& ctx, const Assets& assets);
        void run_upsample(Context& ctx, const Assets& assets);
    };
}
