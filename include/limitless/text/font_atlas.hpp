#pragma once

#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/util/filesystem.hpp>
#include <glm/glm.hpp>
#include <limitless/core/vertex.hpp>
#include <memory>
#include <array>
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/freetype.h>

namespace Limitless {
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
        std::map<uint32_t, FontCharacter> chars;
        std::shared_ptr<Texture> texture;
        FT_Face face {};
        uint32_t font_size;

        static constexpr auto TAB_WIDTH_IN_SPACES = 4;

        bool isSynthetizedGlyph(uint32_t utf32_codepoint) const noexcept;
    public:
        FontAtlas(const fs::path& path, uint32_t pixel_size);
        ~FontAtlas();

        [[nodiscard]] auto getFontSize() const noexcept { return font_size; }
        [[nodiscard]] const auto& getFontCharacter(uint32_t ucs) const { return chars.at(ucs); }

        [[nodiscard]] const auto& getTexture() const { return texture; }

        /**
         * Return vertices for UTF-8 encoded string.
         */
        [[nodiscard]] std::vector<TextVertex> generate(const std::string& text) const;

        /**
         * Return bounding box for UTF-8 encoded string.
         */
        [[nodiscard]] glm::vec2 getTextSize(const std::string& text) const;

        std::vector<TextVertex> getSelectionGeometry(std::string_view text, size_t begin, size_t end);
    };
}