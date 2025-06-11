#include <limitless/text/type_setter.hpp>

#include <sstream>

using namespace Limitless;

static size_t utf8CharLength(char c) {
    if ((c & 0x80) == 0) {
        return 1;

    } else if ((c & 0xE0) == 0xC0) {
        return 2;

    } else if ((c & 0xF0) == 0xE0) {
        return 3;

    } else if ((c & 0xF8) == 0xF0) {
        return 4;
    }

    return 0;
}

/**
 * Invokes bool(uint32_t) function for each Unicode codepoint of a UTF-8 encoded string.
 * If that function returns false, then iteration is stopped.
 */
template <typename T>
static void forEachUnicodeCodepoint(std::string_view str, T&& func) {
    size_t i = 0;
    while (i < str.size()) {
        size_t char_len = utf8CharLength(str[i]);
        if (char_len == 0) {
            throw font_error {"invalid UTF-8 char"};
        }

        uint32_t codepoint = str[i] & (0xFF >> char_len);
        for (size_t j = 1; j < char_len; ++j) {
            char continuation_byte = str[i + j];

            if ((continuation_byte & 0xC0) != 0x80) {
                throw font_error {"invalid UTF-8 byte sequence at " + std::to_string(i + j)};
            }
            codepoint = (codepoint << 6) | (continuation_byte & 0x3F);
        }

        if (!func(codepoint)) {
            return;
        }

        i += char_len;
    }
}

static std::vector<std::string> split(const std::string& str, char separator) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, separator)) {
        tokens.emplace_back(std::move(token));
    }

    return tokens;
}

static const std::shared_ptr<FontAtlas>& getFontForChar(
    uint32_t utf32_codepoint,
    const std::vector<std::shared_ptr<FontAtlas>>& font_stack,
    std::optional<CjkVariant> cjk_variant
) {
    for (const auto& font_sptr : font_stack) {
        if (cjk_variant) {
            const auto font_cjk_variant = font_sptr->getCjkVariant();
            if (font_cjk_variant && *cjk_variant != *font_cjk_variant) {
                // skip font if its for wrong CJK variant.
                continue;
            }
        }

        auto maybe_fc = font_sptr->getFontChar(utf32_codepoint);
        if (maybe_fc) {
            return font_sptr;
        }
    }
    return font_stack[0];
}

static std::string wordWrap(
    const std::string& original_text,
    const std::vector<std::shared_ptr<FontAtlas>>& font_stack,
    float wrap_width,
    float curr_width
) {
    std::string word_wrapped_text;
    // TODO: investigate word split for CJK.
    auto lines = split(original_text, '\n');

    for (const auto& line : lines) {
        auto words = split(line, ' ');

        for (auto& word : words) {
            if (&word != &words.back()) {
                word += ' ';
            }

            const auto word_width = [&](){
                float result = 0.f;

                forEachUnicodeCodepoint(word, [&](uint32_t cp){
                     // TODO: this might result in wrong word wrap if diff fonts are selected due to CJK variance.
                    const auto& font = getFontForChar(cp, font_stack, /* cjk_variant = */std::nullopt);
                    const auto& fc = font->getFontCharOrTofu(cp);

                    result += (fc.advance >> 6);

                    return true;
                });

                return result;
            }();

            if (curr_width + word_width <= wrap_width) {
                word_wrapped_text += word;
                curr_width += word_width;

            } else {
                if (curr_width != 0.f) {
                    word_wrapped_text += '\n';
                }
                word_wrapped_text += word;
                curr_width = word_width;
            }
        }

        if (&line != &lines.back()) {
            curr_width = 0.f;
            word_wrapped_text += '\n';
        }
    }

    return word_wrapped_text;
}

TypeSetResult TypeSetter::typeSet(
    const std::vector<FormattedText>& formatted_text_parts
) {
    std::unordered_map<FontAtlas*, FontVertices> font_vertices;
    std::pair<glm::vec2, glm::vec2> bounding_box {{0.f, 0.f}, {0.f, 0.f}};
    glm::vec2& min_pos = bounding_box.first;
    glm::vec2& max_pos = bounding_box.second;

    if (formatted_text_parts.empty()) {
        return TypeSetResult({}, bounding_box);
    }

    glm::vec2 offset {0.f, 0.f};
    
    for (const auto& formatted_text : formatted_text_parts) {
        const auto& color = formatted_text.format.color;
        const auto& font_stack = formatted_text.format.font_stack;
        const auto& wrap_width = formatted_text.format.wrap_width;
        const auto& cjk_variant = formatted_text.format.cjk_variant;

        if (font_stack.empty()) {
            throw font_error("empty font stack");
        }

        const auto& text = wrap_width
            ? wordWrap(formatted_text.text, font_stack, *wrap_width, offset.x)
            : formatted_text.text;

        auto getVertices = [&](const std::shared_ptr<FontAtlas>& font) -> std::vector<TextVertex>& {
            auto it = font_vertices.find(font.get());
            if (it != font_vertices.end()) {
                return it->second.vertices;
            }

            font_vertices.emplace(font.get(), FontVertices(font, {}));
            return font_vertices.at(font.get()).vertices;
        };

        forEachUnicodeCodepoint(text, [&](uint32_t cp) -> bool {
            if (cp == '\n') {
                offset.y -= font_stack[0]->getFontSize();
                offset.x = 0;

                return true;
            }

            const auto& font = getFontForChar(cp, font_stack, cjk_variant);
            const auto& fc = font->getFontCharOrTofu(cp);

            float x = offset.x + fc.bearing.x;
            float y = offset.y + fc.bearing.y - fc.size.y;

            min_pos = glm::vec2(std::min(min_pos.x, x), std::min(min_pos.y, y));
            max_pos = glm::vec2(std::max(max_pos.x, x + fc.size.x), std::max(max_pos.y, y + fc.size.y));

            const auto vertex_color = fc.is_icon ? glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) : color;

            auto& vertices = getVertices(font);

            vertices.emplace_back(glm::vec2{x, y + fc.size.y}, fc.uvs[2], vertex_color);
            vertices.emplace_back(glm::vec2{x, y},             fc.uvs[0], vertex_color);
            vertices.emplace_back(glm::vec2{x + fc.size.x, y}, fc.uvs[1], vertex_color);

            vertices.emplace_back(glm::vec2{x, y + fc.size.y}, fc.uvs[2], vertex_color);
            vertices.emplace_back(glm::vec2{x + fc.size.x, y}, fc.uvs[1], vertex_color);
            vertices.emplace_back(glm::vec2{x + fc.size.x, y + fc.size.y}, fc.uvs[3], vertex_color);

            offset.x += (fc.advance >> 6);

            return true;
        });
    };

    std::vector<FontVertices> vertices_per_font;

    for (auto& [font_atlas_ptr, vertices] : font_vertices) {
        vertices_per_font.emplace_back(std::move(vertices));
    }

    return TypeSetResult(std::move(vertices_per_font), bounding_box);
}

std::vector<TextSelectionVertex> TypeSetter::typeSetSelection(
    const std::vector<FormattedText>& formatted_text_parts,
    size_t start_codepoint_index,
    size_t end_codepoint_index
) {
    std::vector<TextSelectionVertex> vertices;

    auto addRect = [&] (glm::vec2 pos, glm::vec2 size) {
        vertices.emplace_back(glm::vec2{pos.x, -pos.y + size.y});
        vertices.emplace_back(glm::vec2{pos.x, -pos.y});
        vertices.emplace_back(glm::vec2{pos.x + size.x, -pos.y});

        vertices.emplace_back(glm::vec2{pos.x, -pos.y + size.y});
        vertices.emplace_back(glm::vec2{pos.x + size.x, -pos.y});
        vertices.emplace_back(glm::vec2{pos.x + size.x, -pos.y + size.y});
    };

    glm::ivec2 offset {};
    size_t cp_index = 0u;
    uint32_t line_width = 0u;
    uint32_t line_height = 0u;

    for (const auto& formatted_text : formatted_text_parts) {
        const auto& text = formatted_text.text;
        const auto& font_stack = formatted_text.format.font_stack;
        const auto& cjk_variant = formatted_text.format.cjk_variant;

        if (font_stack.empty()) {
            throw font_error("empty font stack");
        }

        forEachUnicodeCodepoint(text, [&](uint32_t cp) {
            const auto& font = getFontForChar(cp, font_stack, cjk_variant);

            line_height = std::max(line_height, font_stack[0]->getFontSize());
            if (cp_index < start_codepoint_index) {
                // Skip until selection range starts.
                
                offset.x += font->getFontCharOrTofu(cp).advance >> 6;
                if (cp == '\n') {
                    offset.x = 0;
                    offset.y += line_height;
                    line_height = 0u;
                }
                ++cp_index;
                return true;

            } else if (cp_index < end_codepoint_index) {
                if (cp == '\n') {
                    if (line_width != 0) {
                        // Finish this selection line.
                        addRect({offset.x, offset.y + line_height/4}, glm::vec2{line_width, line_height});
                        line_width = 0;
                        offset.x = 0;
                        offset.y += line_height;
                        line_height = 0u;
                    } else {
                        // An empty line, still add small selection geometry for it.
                        line_width = font->getFontCharOrTofu(' ').advance >> 6;
                    }
                } else {
                        line_width += font->getFontCharOrTofu(cp).advance >> 6;
                }
                ++cp_index;
                return true;

            } else {
                return false;
            }
        });
    }

    addRect({offset.x, offset.y + line_height/4}, glm::vec2{line_width, line_height});


    return vertices;
}

bool TextFormat::sameFontStack(const TextFormat& lhs, const TextFormat& rhs) {
    if (lhs.font_stack.size() != rhs.font_stack.size()) {
        return false;
    }

    for (size_t i = 0; i < lhs.font_stack.size(); ++i) {
        if (lhs.font_stack[i].get() != rhs.font_stack[i].get()) {
            return false;
        }
    }

    return true;
}
