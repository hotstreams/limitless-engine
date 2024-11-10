#pragma once

#include <limitless/core/texture/texture.hpp>

namespace Limitless {
    class TerrainRegion {
    public:
        std::shared_ptr<Texture> height;
        std::shared_ptr<Texture> control;
        uint32_t region_size;
    public:
        TerrainRegion() {

        }
    };
}