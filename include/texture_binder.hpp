#pragma once

#include <texture.hpp>

#include <vector>

namespace GraphicsEngine {
    class TextureBinder {
    private:
        static inline GLint current_bind {0};
    public:
        // binds texture to any unit and returns its index
        [[nodiscard]] static GLint bind(Texture& texture) noexcept;

        // binds textures and returns indices to units
        [[nodiscard]] static std::vector<GLint> bind(const std::vector<Texture*>& textures);
    };
}