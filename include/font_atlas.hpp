#pragma once

#include <core/texture_builder.hpp>
#include <util/filesystem.hpp>
#include <glm/glm.hpp>
#include <core/vertex.hpp>
#include <memory>
#include <array>
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace GraphicsEngine {
    struct FontCharacter {
        glm::ivec2 size;
        glm::ivec2 bearing;
        uint32_t advance;
        std::array<glm::vec2, 4> uvs;
    };

    struct font_error : public std::runtime_error {
        explicit font_error(const char* error) : runtime_error{error} { }
        explicit font_error(const std::string& error) : runtime_error{error} {}
    };

    class FontAtlas {
    private:
        std::map<char, FontCharacter> chars;
        std::shared_ptr<Texture> texture;
        FT_Face face {nullptr};
        uint32_t font_size;
    public:
        FontAtlas(const fs::path& path, uint32_t size);
        ~FontAtlas();

        [[nodiscard]] const auto& getTexture() const { return texture; }

        [[nodiscard]] std::vector<TextVertex> generate(const std::string& text) const;
    };
}