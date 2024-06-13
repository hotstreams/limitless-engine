#include <limitless/text/font_atlas.hpp>

#define STB_RECT_PACK_IMPLEMENTATION
#include <stb_rect_pack.h>

#include <iostream>

using namespace Limitless;
using namespace std::literals::string_literals;

static constexpr const uint32_t UNDEFINED_GLYPH_INDEX = 0;
static constexpr const uint32_t UNDEFINED_GLYPH_CHAR_CODE = 0;

FontAtlas::FontAtlas(const fs::path& path, uint32_t pixel_size)
    : font_size {pixel_size} {
    static FT_Library ft {nullptr};

    if (!ft) {
        if (FT_Init_FreeType(&ft)) {
            throw font_error{"Failed to initialize Freetype"};
        }
    }

    if (FT_New_Face(ft, path.string().c_str(), 0, &face)) {
        throw font_error{"Failed to load the font at path"s + path.string()};
    }

    FT_Set_Pixel_Sizes(face, 0, font_size);

    struct GlyphInfo {
        glm::ivec2 bearing;
        uint32_t advance;
        std::vector<std::byte> bitmap;
    };

    std::unordered_map<uint32_t, GlyphInfo> glyph_for_char;

    stbrp_context context;
    std::vector<stbrp_rect> packer_rects;
    std::vector<stbrp_node> packer_nodes;

    FT_ULong char_code;
    FT_UInt glyph_index;

    auto loadGlyphFrom = [&](const FT_GlyphSlot ft_glyph, uint32_t char_code){
        const auto& glyph_bitmap = ft_glyph->bitmap;
        if (glyph_bitmap.pitch < 0) {
            throw font_error {"font has negative glyph bitmap pitch, which is not supported"};
        }

        auto char_bitmap = std::vector<std::byte>(glyph_bitmap.pitch * glyph_bitmap.rows);
        memcpy(char_bitmap.data(), glyph_bitmap.buffer, char_bitmap.size());

        auto [_, emplaced] = glyph_for_char.emplace(char_code, GlyphInfo {
            {ft_glyph->bitmap_left, ft_glyph->bitmap_top},
            static_cast<uint32_t>(ft_glyph->advance.x),
            std::move(char_bitmap)
        });

        // TODO: support char glyph variants?
        if (emplaced) {
            packer_rects.emplace_back(stbrp_rect{char_code, glyph_bitmap.width, glyph_bitmap.rows, 0, 0, 0});
        }
    };

    if (FT_Load_Glyph(face, UNDEFINED_GLYPH_INDEX, FT_LOAD_RENDER) != 0) {
        throw font_error {"Failed to load tofu (missing) glyph"};
    }

    // Put "tofu" as 0 char code glyph -- null terminators (\0) are not normally rendered anyway.
    loadGlyphFrom(face->glyph, UNDEFINED_GLYPH_CHAR_CODE);

    for (
        char_code = FT_Get_First_Char(face, &glyph_index);
        glyph_index != 0;
        char_code = FT_Get_Next_Char(face, char_code, &glyph_index)
    ) {
        if (FT_Load_Char(face, char_code, FT_LOAD_RENDER) != 0) {
            throw font_error {"Failed to load char with code " + std::to_string(char_code)};
        }

        loadGlyphFrom(face->glyph, char_code);
    }
    packer_nodes.resize(packer_rects.size());

    // TODO: pass atlas size as parameter.
    constexpr const size_t atlas_dim_size = 4096;
    const glm::uvec2 atlas_size = glm::uvec2(atlas_dim_size);

    stbrp_init_target(&context, atlas_size.x, atlas_size.y, packer_nodes.data(), packer_nodes.size());
    stbrp_pack_rects(&context, packer_rects.data(), packer_rects.size());

    std::vector<std::byte> data(atlas_size.x * atlas_size.y);

    for (const auto& rect : packer_rects) {
        if (!rect.was_packed) {
            // TODO: better to try again with larger size.
            throw font_error {"failed to pack chars into atlas"};
        }

        uint32_t char_code = *(reinterpret_cast<const uint32_t*>(&rect.id));
        auto it = glyph_for_char.find(char_code);
        if (it == glyph_for_char.end()) {
            throw font_error {"failed to find glyph for char " + std::to_string(char_code)};
        }

        const auto& glyph_info = it->second;

        for (size_t row = 0; row < rect.h; ++row) {
            const auto& char_bitmap = glyph_info.bitmap;
            const size_t glyph_pitch = char_bitmap.size() / rect.h; // because bitmap size = pitch * rows
            const ptrdiff_t input_offset = glyph_pitch * row;
            const ptrdiff_t output_offset = (rect.y + row)*atlas_size.y + rect.x;

            memcpy(data.data() + output_offset, char_bitmap.data() + input_offset, rect.w);
        }

        const glm::vec2 atlas_rect_tl_pos = {
            static_cast<float>(rect.x) / static_cast<float>(atlas_size.x),
            static_cast<float>(rect.y) / static_cast<float>(atlas_size.y)
        };

        const glm::vec2 atlas_rect_br_pos = {
            static_cast<float>(rect.x + rect.w) / static_cast<float>(atlas_size.x),
            static_cast<float>(rect.y + rect.h) / static_cast<float>(atlas_size.y)
        };

        chars.emplace(char_code, FontChar{
            {rect.w, rect.h},
            glyph_info.bearing,
            glyph_info.advance,
            {
                glm::vec2 { atlas_rect_tl_pos.x, atlas_rect_br_pos.y},
                glm::vec2 { atlas_rect_br_pos.x, atlas_rect_br_pos.y},
                glm::vec2 { atlas_rect_tl_pos.x, atlas_rect_tl_pos.y},
                glm::vec2 { atlas_rect_br_pos.x, atlas_rect_tl_pos.y}
            }}
        );
    }

    chars.emplace('\t', chars.at(' '));
    chars.at('\t').advance *= TAB_WIDTH_IN_SPACES;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    texture = Texture::builder()
            .target(Texture::Type::Tex2D)
            .internal_format(Texture::InternalFormat::R)
            .size(atlas_size)
            .format(Texture::Format::Red)
            .data_type(Texture::DataType::UnsignedByte)
            .data(data.data())
            .wrap_s(Texture::Wrap::ClampToEdge)
            .wrap_t(Texture::Wrap::ClampToEdge)
            .min_filter(Texture::Filter::Linear)
            .mag_filter(Texture::Filter::Linear)
            .mipmap(false)
            .buildMutable();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

FontAtlas::~FontAtlas() {
    if (face) {
        FT_Done_Face(face);
    }
}

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
        for (int j = 1; j < char_len; ++j) {
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

std::vector<TextVertex> FontAtlas::generate(const std::string& text) const {
    std::vector<TextVertex> vertices;
    vertices.reserve(text.size());

    glm::vec2 offset {0.f, 0.f};

    forEachUnicodeCodepoint(text, [&](uint32_t cp) {
        if (cp == '\n') {
            offset.y -= font_size;
            offset.x = 0;
            return true;
        }

        const auto& fc = getFontChar(cp);

        float x = offset.x + fc.bearing.x;
        float y = offset.y + fc.bearing.y - fc.size.y;

        vertices.emplace_back(glm::vec2{x, y + fc.size.y}, fc.uvs[2]);
        vertices.emplace_back(glm::vec2{x, y},             fc.uvs[0]);
        vertices.emplace_back(glm::vec2{x + fc.size.x, y}, fc.uvs[1]);

        vertices.emplace_back(glm::vec2{x, y + fc.size.y}, fc.uvs[2]);
        vertices.emplace_back(glm::vec2{x + fc.size.x, y}, fc.uvs[1]);
        vertices.emplace_back(glm::vec2{x + fc.size.x, y + fc.size.y}, fc.uvs[3]);

        offset.x += (fc.advance >> 6);
        return true;
    });

    return vertices;
}

glm::vec2 FontAtlas::getTextSize(const std::string& text) const {
	glm::vec2 max_size {0, font_size};
	float size {};

    forEachUnicodeCodepoint(text, [&](uint32_t cp) {
        const auto& fc = getFontChar(cp);

        size += (fc.advance >> 6);

        if (cp == '\n') {
            max_size.y += font_size;
            size = 0;
        }

        max_size.x = std::max(max_size.x, size);
        return true;
    });

	return max_size;
}

std::vector<TextVertex> FontAtlas::getSelectionGeometry(std::string_view text, size_t begin, size_t end) const {
    std::vector<TextVertex> vertices;

    auto addRect = [&] (glm::vec2 pos, glm::vec2 size) {
        vertices.emplace_back(glm::vec2{pos.x, -pos.y + size.y}, glm::vec2{});
        vertices.emplace_back(glm::vec2{pos.x, -pos.y}, glm::vec2{});
        vertices.emplace_back(glm::vec2{pos.x + size.x, -pos.y}, glm::vec2{});

        vertices.emplace_back(glm::vec2{pos.x, -pos.y + size.y}, glm::vec2{});
        vertices.emplace_back(glm::vec2{pos.x + size.x, -pos.y}, glm::vec2{});
        vertices.emplace_back(glm::vec2{pos.x + size.x, -pos.y + size.y}, glm::vec2{});
    };

    glm::ivec2 offset{};

    // finds character max y value
    for (const auto& [c, fc] : chars) {
        offset.y = std::max(offset.y, fc.size.y - fc.bearing.y);
    }

    size_t pos = 0;
    size_t size = 0;

    forEachUnicodeCodepoint(text, [&](uint32_t cp) {
        if (pos < begin) {
            // Skip until selection range starts.
            offset.x += getFontChar(cp).advance >> 6;
            if (cp == '\n') {
                offset.x = 0;
                offset.y += font_size;
            }
            ++pos;
            return true;

        } else if (pos < end) {
            if (cp == '\n') {
                if (size != 0) {
                    // Finish this selection line.
                    addRect({offset.x, offset.y}, glm::vec2{size, font_size});
                    size = 0;
                    offset.x = 0;
                    offset.y += font_size;
                } else {
                    // An empty line, still add small selection geometry for it.
                    size = getFontChar(' ').advance >> 6;
                }
            } else {
                size += getFontChar(cp).advance >> 6;
            }
            ++pos;
            return true;

        } else {
            return false;
        }
    });

    addRect({offset.x, offset.y}, glm::vec2{size, font_size});

    return vertices;
}

FontChar& FontAtlas::fontCharFor(uint32_t utf32_codepoint) noexcept {
    auto it = chars.find(utf32_codepoint);
    if (it == chars.end()) {
        return chars.at(UNDEFINED_GLYPH_CHAR_CODE);
    }

    return it->second;
}

const FontChar& FontAtlas::getFontChar(uint32_t utf32_codepoint) const noexcept {
    auto it = chars.find(utf32_codepoint);
    if (it == chars.end()) {
        return chars.at(UNDEFINED_GLYPH_CHAR_CODE);
    }

    return it->second;
}
