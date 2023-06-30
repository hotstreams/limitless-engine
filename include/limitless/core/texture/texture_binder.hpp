#pragma once

#include <limitless/core/texture/texture.hpp>
#include <vector>

namespace Limitless {
    class TextureBinder {
    private:
        //TODO: move to context state
        static inline GLint current_bind {0};
    public:
        // TODO: fix
//        // binds texture to any unit and returns its index
//        static GLint bind(Texture& texture) noexcept;
//
//        //TODO: returns slot to bind, not binding it itself
//        static GLint bind(GLenum target, const ExtensionTexture& texture) noexcept;

        // binds textures and returns indices to units
        [[nodiscard]] static std::vector<GLint> bind(const std::vector<Texture*>& textures);
    };
}