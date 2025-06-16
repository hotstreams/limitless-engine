#pragma once

#include <glm/vec4.hpp>
#include <memory>
#include <optional>
#include <vector>

#include <limitless/text/font_atlas.hpp>
#include <limitless/core/vertex.hpp>

namespace Limitless {
    class FontAtlas;

    struct TextFormat {
        glm::vec4 color;
        std::vector<std::shared_ptr<FontAtlas>> font_stack;
        std::optional<float> wrap_width;
        std::optional<CjkVariant> cjk_variant;
        float line_spacing_modifier;
        std::optional<uint32_t> pixel_size;
        std::optional<std::string> link_id;

        TextFormat(
            glm::vec4 _color,
            std::vector<std::shared_ptr<FontAtlas>> _font_stack,
            std::optional<float> _wrap_width,
            std::optional<CjkVariant> _cjk_variant,
            float _line_spacing_modifier,
            std::optional<uint32_t> _pixel_size,
            std::optional<std::string> _link_id
        )
            : color (_color)
            , font_stack (std::move(_font_stack))
            , wrap_width (_wrap_width)
            , cjk_variant (_cjk_variant)
            , line_spacing_modifier (_line_spacing_modifier)
            , pixel_size (_pixel_size)
            , link_id (_link_id)
        {
        }

        friend bool operator==(const TextFormat& lhs, const TextFormat& rhs) {
            return lhs.color == rhs.color
                && sameFontStack(lhs, rhs)
                && lhs.wrap_width == rhs.wrap_width
                && lhs.cjk_variant == rhs.cjk_variant
                && lhs.line_spacing_modifier == rhs.line_spacing_modifier
                && lhs.pixel_size == rhs.pixel_size
                && lhs.link_id == rhs.link_id;
        }

        friend bool operator!=(const TextFormat& lhs, const TextFormat& rhs) {
            return !(lhs == rhs);
        }

    private:
        static bool sameFontStack(const TextFormat& lhs, const TextFormat& rhs);
    };

    struct FormattedText {
        std::string text;
        TextFormat format;

        FormattedText(std::string _text, TextFormat _text_format)
            : text (std::move(_text))
            , format (std::move(_text_format))
        {
        }

        friend bool operator==(const FormattedText& a, const FormattedText& b) {
            return a.text == b.text && a.format == b.format;
        }
    };

    struct FontVertices {
        std::shared_ptr<FontAtlas> font;
        std::vector<TextVertex> vertices;

        FontVertices(std::shared_ptr<FontAtlas> _font, std::vector<TextVertex> _vertices)
            : font (std::move(_font))
            , vertices (std::move(_vertices))
        {

        }
    };

    struct TypeSetResult {
        std::vector<FontVertices> vertices_of_fonts;
        std::pair<glm::vec2, glm::vec2> bounding_box;
        std::unordered_map<std::string, std::vector<std::pair<glm::vec2, glm::vec2>>> link_rectangles;

        TypeSetResult(
            std::vector<FontVertices> _vertices_of_fonts,
            std::pair<glm::vec2, glm::vec2> _bounding_box,
            std::unordered_map<std::string, std::vector<std::pair<glm::vec2, glm::vec2>>> _link_rectangles
        )
            : vertices_of_fonts(std::move(_vertices_of_fonts))
            , bounding_box(std::move(_bounding_box))
            , link_rectangles(std::move(_link_rectangles))
        {

        }
    };

    class TypeSetter {
    public:
        static TypeSetResult
        typeSet(
            const std::vector<FormattedText>& formatted_text_parts
        );

        static std::vector<TextSelectionVertex> typeSetSelection(
            const std::vector<FormattedText>& formatted_text_parts,
            size_t start_codepoint_index,
            size_t end_codepoint_index
        );
    };
}
