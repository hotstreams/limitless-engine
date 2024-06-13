#pragma once

#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/util/filesystem.hpp>
#include <limitless/core/vertex.hpp>

#include <memory>
#include <array>
#include <unordered_map>

#include <glm/glm.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/freetype.h>

namespace Limitless {
    struct FontChar {
        /**
         * Character glyph bounding box size, in pixels.
         */
        glm::ivec2 size;

        /**
         * Character glyph offset inside its bounding box, in pixels.
         */
        glm::ivec2 bearing;

        /**
         * Horizontal advance width in (1/26.6) pixel units.
         * It is the distance until next character used during typesetting.
         * Multiply by ~27 (or 32, which is faster) to get value in pixels.
         */
        uint32_t advance;

        /**
         * UVs of this character glyph on font atlas texture.
         */
        std::array<glm::vec2, 4> uvs;
    };

    struct font_error : public std::runtime_error {
        explicit font_error(const char* error) : runtime_error{error} { }
        explicit font_error(const std::string& error) : runtime_error{error} {}
    };

    class FontAtlas {
    public:
        FontAtlas(const fs::path& path, uint32_t pixel_size);
        ~FontAtlas();

        /**
         * Return font vertical size in pixels.
         */
        [[nodiscard]] auto getFontSize() const noexcept { return font_size; }

        /**
         * Return font character for given Unicode codepoint.
         * If font does not have it, then ""missing/tofu" font character is returned.
         */
        [[nodiscard]] const FontChar& getFontChar(uint32_t utf32_codepoint) const noexcept;

        [[nodiscard]] const auto& getTexture() const { return texture; }

        /**
         * Return vertices for UTF-8 encoded string.
         */
        [[nodiscard]] std::vector<TextVertex> generate(const std::string& text) const;

        /**
         * Return bounding box for UTF-8 encoded string.
         */
        [[nodiscard]] glm::vec2 getTextSize(const std::string& text) const;

        /**
         * Return selection geometry for UTF-8 encoded string, from [@begin; @end) range position runes.
         */
        std::vector<TextVertex> getSelectionGeometry(std::string_view text, size_t begin, size_t end) const;

    private:
        std::unordered_map<uint32_t, FontChar> chars;
        std::shared_ptr<Texture> texture;
        FT_Face face {};
        uint32_t font_size;

        static constexpr auto TAB_WIDTH_IN_SPACES = 4;
    };
}
