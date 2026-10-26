#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <memory>
#include <optional>
#include <vector>

#include <limitless/text/font_atlas.hpp>
#include <limitless/core/vertex.hpp>

namespace Limitless {
    class FontAtlas;

    /**
     * Shadow under the glyphs, in screen pixels whatever the text scale. Offset y points down.
     */
    struct TextShadow {
        glm::vec4 color;
        glm::vec2 offset {1.f, 1.f};
        // Half width of the shadow edge falloff.
        float softness {0.5f};

        friend bool operator==(const TextShadow& lhs, const TextShadow& rhs) {
            return lhs.color == rhs.color && lhs.offset == rhs.offset && lhs.softness == rhs.softness;
        }
    };

    /**
     * Outline around the glyphs, in screen pixels whatever the text scale.
     * Effects must fit into the SDF spread of the atlas, as scaled on screen.
     */
    struct TextOutline {
        glm::vec4 color;
        float width {1.f};

        friend bool operator==(const TextOutline& lhs, const TextOutline& rhs) {
            return lhs.color == rhs.color && lhs.width == rhs.width;
        }
    };

    /**
     * Effects are drawn only for glyphs from SDF font atlases.
     */
    struct TextEffects {
        std::optional<TextShadow> shadow;
        std::optional<TextOutline> outline;

        [[nodiscard]] bool empty() const noexcept { return !shadow && !outline; }

        friend bool operator==(const TextEffects& lhs, const TextEffects& rhs) {
            return lhs.shadow == rhs.shadow && lhs.outline == rhs.outline;
        }

        friend bool operator!=(const TextEffects& lhs, const TextEffects& rhs) {
            return !(lhs == rhs);
        }
    };

    struct TextFormat {
        glm::vec4 color;
        std::vector<std::shared_ptr<FontAtlas>> font_stack;
        std::optional<float> wrap_width;
        std::optional<CjkVariant> cjk_variant;
        float line_spacing_modifier;
        std::optional<uint32_t> pixel_size;
        std::optional<std::string> link_id;
        TextEffects effects;

        TextFormat(
            glm::vec4 _color,
            std::vector<std::shared_ptr<FontAtlas>> _font_stack,
            std::optional<float> _wrap_width,
            std::optional<CjkVariant> _cjk_variant,
            float _line_spacing_modifier,
            std::optional<uint32_t> _pixel_size,
            std::optional<std::string> _link_id,
            TextEffects _effects = {}
        )
            : color (_color)
            , font_stack (std::move(_font_stack))
            , wrap_width (_wrap_width)
            , cjk_variant (_cjk_variant)
            , line_spacing_modifier (_line_spacing_modifier)
            , pixel_size (_pixel_size)
            , link_id (_link_id)
            , effects (std::move(_effects))
        {
        }

        friend bool operator==(const TextFormat& lhs, const TextFormat& rhs) {
            return lhs.color == rhs.color
                && sameFontStack(lhs, rhs)
                && lhs.wrap_width == rhs.wrap_width
                && lhs.cjk_variant == rhs.cjk_variant
                && lhs.line_spacing_modifier == rhs.line_spacing_modifier
                && lhs.pixel_size == rhs.pixel_size
                && lhs.link_id == rhs.link_id
                && lhs.effects == rhs.effects;
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
        // Empty unless the font atlas supports them.
        TextEffects effects;

        FontVertices(std::shared_ptr<FontAtlas> _font, std::vector<TextVertex> _vertices, TextEffects _effects = {})
            : font (std::move(_font))
            , vertices (std::move(_vertices))
            , effects (std::move(_effects))
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
