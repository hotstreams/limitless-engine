#include <limitless/text/font_atlas.hpp>

#define STB_RECT_PACK_IMPLEMENTATION
#include <stb_rect_pack.h>

#include <iostream>

using namespace Limitless;
using namespace std::literals::string_literals;

static constexpr const uint32_t UNDEFINED_GLYPH_INDEX = 0;
static constexpr const uint32_t UNDEFINED_GLYPH_CHAR_CODE = 0;

static constexpr const size_t TAB_WIDTH_IN_SPACES = 4;

static constexpr const size_t MAX_ATLAS_DIM_SIZE = 4096;

struct GlyphInfo {
    glm::ivec2 bearing;
    uint32_t advance;
    glm::uvec2 size;
    std::vector<std::byte> bitmap;
    bool is_icon;
};

struct AtlasPackResult {
    std::unordered_map<uint32_t, glm::uvec2> utf32_char_pos;
    glm::uvec2 atlas_size;
};

static AtlasPackResult packAtlas(const std::unordered_map<uint32_t, GlyphInfo>& glyph_for_char) {
    stbrp_context context;
    std::vector<stbrp_rect> packer_rects;
    std::vector<stbrp_node> packer_nodes;

    static_assert(sizeof(int) >= sizeof(uint32_t), "int must be at least as big as uint32_t");

    for (const auto& [char_code, glyph_info] : glyph_for_char) {
        packer_rects.emplace_back(stbrp_rect{(int)char_code, (int)glyph_info.size.x, (int)glyph_info.size.y, 0, 0, 0});
    }

    packer_nodes.resize(packer_rects.size());
    
    size_t atlas_dim_size = 256;

    while (atlas_dim_size <= MAX_ATLAS_DIM_SIZE) {
        stbrp_init_target(&context, atlas_dim_size, atlas_dim_size, packer_nodes.data(), packer_nodes.size());
        
        if (!stbrp_pack_rects(&context, packer_rects.data(), packer_rects.size())) {
            atlas_dim_size *= 2;
            continue;
        }
        
        std::unordered_map<uint32_t, glm::uvec2> utf32_char_pos;
        for (const auto& rect : packer_rects) {
            uint32_t char_code = *(reinterpret_cast<const uint32_t*>(&rect.id));
            utf32_char_pos.emplace(char_code, glm::uvec2{rect.x, rect.y});
        }

        return AtlasPackResult {
            std::move(utf32_char_pos),
            glm::uvec2{atlas_dim_size, atlas_dim_size}
        };
    }

    throw font_error {"failed to pack atlas into " + std::to_string(MAX_ATLAS_DIM_SIZE) + "x" + std::to_string(MAX_ATLAS_DIM_SIZE) + " texture"};
}

static std::shared_ptr<FontAtlas> makeAtlas(
    std::unordered_map<uint32_t, GlyphInfo> glyph_for_char,
    uint32_t pixel_size,
    size_t bytes_per_pixel,
    bool is_icon
) {
    std::unordered_map<uint32_t, FontChar> chars;

    auto atlas_pack_result = packAtlas(glyph_for_char);
    const auto atlas_size = atlas_pack_result.atlas_size;
    const auto& char_pos = atlas_pack_result.utf32_char_pos;

    std::vector<std::byte> data(atlas_size.x * atlas_size.y * bytes_per_pixel);

    for (const auto& [char_code, glyph] : glyph_for_char) {
        auto it = char_pos.find(char_code);
        if (it == char_pos.end()) {
            // TODO: show actual char code as hex.
            throw font_error {"failed to find char " + std::to_string(char_code) + " in generated atlas"};
        }

        const auto& glyph_pos = it->second;

        for (size_t row = 0; row < static_cast<size_t>(glyph.size.y); ++row) {
            const auto& char_bitmap = glyph.bitmap;
            const size_t glyph_pitch = char_bitmap.size() / glyph.size.y; // as bitmap size = pitch * rows
            const size_t atlas_pitch = data.size() / atlas_size.y;

            const ptrdiff_t input_offset = is_icon
                ? glyph_pitch * (glyph.size.y - 1 - row)
                : glyph_pitch * row;
            const ptrdiff_t output_offset = (glyph_pos.y + row)*atlas_pitch + (glyph_pos.x * bytes_per_pixel);

            memcpy(data.data() + output_offset, char_bitmap.data() + input_offset, glyph.size.x * bytes_per_pixel);
        }

        const glm::vec2 atlas_rect_tl_pos = {
            static_cast<float>(glyph_pos.x) / static_cast<float>(atlas_size.x),
            static_cast<float>(glyph_pos.y) / static_cast<float>(atlas_size.y)
        };

        const glm::vec2 atlas_rect_br_pos = {
            static_cast<float>(glyph_pos.x + glyph.size.x) / static_cast<float>(atlas_size.x),
            static_cast<float>(glyph_pos.y + glyph.size.y) / static_cast<float>(atlas_size.y)
        };

        chars.emplace(char_code, FontChar{
            {glyph.size.x, glyph.size.y},
            glyph.bearing,
            glyph.advance,
            {
                glm::vec2 { atlas_rect_tl_pos.x, atlas_rect_br_pos.y},
                glm::vec2 { atlas_rect_br_pos.x, atlas_rect_br_pos.y},
                glm::vec2 { atlas_rect_tl_pos.x, atlas_rect_tl_pos.y},
                glm::vec2 { atlas_rect_br_pos.x, atlas_rect_tl_pos.y}
            },
            glyph.is_icon
        });
    }

    if (chars.find(' ') == chars.end()) {
        // add fallback whitespace definition if missing,
        // otherwise the font atlas is too cursed.
        chars.emplace(' ', FontChar{
            {0, 0},
            {0, 0},
            pixel_size,
            {
                glm::vec2(0.f),
                glm::vec2(0.f),
                glm::vec2(0.f),
                glm::vec2(0.f)
            },
            /* is_icon = */ true // so that whitespace is not colored : )
        });
    }
    // TODO: remove as tabulation has to be handled by type setter.
    chars.emplace('\t', chars.at(' '));
    chars.at('\t').advance *= TAB_WIDTH_IN_SPACES;

    const auto internal_format = [bytes_per_pixel] {
        switch (bytes_per_pixel) {
            case 1: return Texture::InternalFormat::R8;
            case 4: return Texture::InternalFormat::RGBA8;
            default: throw font_error {"unsupported bytes per pixel: " + std::to_string(bytes_per_pixel)};
        }
    }();

    const auto format = [bytes_per_pixel] {
        switch (bytes_per_pixel) {
            case 1: return Texture::Format::Red;
            case 4: return Texture::Format::RGBA;
            default: throw font_error {"unsupported bytes per pixel: " + std::to_string(bytes_per_pixel)};
        }
    }();

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    auto texture = Texture::builder()
            .target(Texture::Type::Tex2D)
            .internal_format(internal_format)
            .size(atlas_size)
            .format(format)
            .data_type(Texture::DataType::UnsignedByte)
            .data(data.data())
            .wrap_s(Texture::Wrap::ClampToEdge)
            .wrap_t(Texture::Wrap::ClampToEdge)
            .min_filter(Texture::Filter::Linear)
            .mag_filter(Texture::Filter::Linear)
            .mipmap(false)
            .buildMutable();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    return std::make_shared<FontAtlas>(std::move(chars), std::move(texture), pixel_size, is_icon);
}

FontAtlas::FontAtlas(
    std::unordered_map<uint32_t, FontChar> chars,
    std::shared_ptr<Texture> texture,
    uint32_t pixel_size,
    bool _is_icon
)  
    : chars {std::move(chars)}
    , texture {std::move(texture)}
    , pixel_size {pixel_size}
    , is_icon (_is_icon)
{

}

std::shared_ptr<FontAtlas> FontAtlas::make(
    uint32_t font_size_in_pixels,
    std::unordered_map<uint32_t, std::shared_ptr<Texture>> icons
) {
    if (icons.empty()) {
        throw font_error {"no icons provided"};
    }

    std::unordered_map<uint32_t, GlyphInfo> glyph_for_char;

    const auto bytes_per_pixel = [&] {
        switch (icons.begin()->second->getInternalFormat()) {
            case Texture::InternalFormat::R8:
                std::cerr << "internal format r8\n";
                return 1;
            case Texture::InternalFormat::RGBA8:
                std::cerr << "internal format rgba8\n";
                return 4;
            default: throw font_error {"unsupported texture format"};
        }
    }();

    for (const auto& [char_code, texture] : icons) {
        glyph_for_char.emplace(char_code, GlyphInfo {
            {0, 3*texture->getSize().y/4},
            texture->getSize().x << 6,
            {texture->getSize().x, texture->getSize().y},
            texture->getPixels(),
            /* is_icon = */true
        });
    }

    return makeAtlas(std::move(glyph_for_char), font_size_in_pixels, bytes_per_pixel, /* is_icon = */ true);
}

std::shared_ptr<FontAtlas> FontAtlas::load(
    const fs::path& path,
    uint32_t pixel_size
) {
    static FT_Library ft {nullptr};

    if (!ft) {
        if (FT_Init_FreeType(&ft)) {
            throw font_error{"Failed to initialize Freetype"};
        }
    }

    FT_Face face;
    if (FT_New_Face(ft, path.string().c_str(), 0, &face)) {
        throw font_error{"Failed to load the font at path"s + path.string()};
    }

    FT_Set_Pixel_Sizes(face, 0, pixel_size);

    std::unordered_map<uint32_t, GlyphInfo> glyph_for_char;

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
            {static_cast<uint32_t>(glyph_bitmap.width), static_cast<uint32_t>(glyph_bitmap.rows)},
            std::move(char_bitmap),
            /* is_icon = */false // TODO: check if its emoji or w/e.
        });
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

    FT_Done_Face(face);

    return makeAtlas(std::move(glyph_for_char), pixel_size, /* bytes_per_pixel = */ 1, /* is_icon = */ false);
}

FontAtlas::~FontAtlas() = default;

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

std::vector<TextVertex> FontAtlas::generate(const std::string& text, const glm::vec4& color) const {
    std::vector<TextVertex> vertices;
    vertices.reserve(text.size());

    glm::vec2 offset {0.f, 0.f};

    forEachUnicodeCodepoint(text, [&](uint32_t cp) {
        if (cp == '\n') {
            offset.y -= pixel_size;
            offset.x = 0;
            return true;
        }

        const auto& fc = getFontChar(cp);

        float x = offset.x + fc.bearing.x;
        float y = offset.y + fc.bearing.y - fc.size.y;

        const auto vertex_color = fc.is_icon ? glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) : color;

        vertices.emplace_back(glm::vec2{x, y + fc.size.y}, fc.uvs[2], vertex_color);
        vertices.emplace_back(glm::vec2{x, y},             fc.uvs[0], vertex_color);
        vertices.emplace_back(glm::vec2{x + fc.size.x, y}, fc.uvs[1], vertex_color);

        vertices.emplace_back(glm::vec2{x, y + fc.size.y}, fc.uvs[2], vertex_color);
        vertices.emplace_back(glm::vec2{x + fc.size.x, y}, fc.uvs[1], vertex_color);
        vertices.emplace_back(glm::vec2{x + fc.size.x, y + fc.size.y}, fc.uvs[3], vertex_color);

        offset.x += (fc.advance >> 6);
        return true;
    });

    return vertices;
}

glm::vec2 FontAtlas::getTextSize(const std::string& text) const {
	glm::vec2 max_size {0, pixel_size};
	float size {};

    forEachUnicodeCodepoint(text, [&](uint32_t cp) {
        const auto& fc = getFontChar(cp);

        size += (fc.advance >> 6);

        if (cp == '\n') {
            max_size.y += pixel_size;
            size = 0;
        }

        max_size.x = std::max(max_size.x, size);
        return true;
    });

	return max_size;
}

std::vector<TextSelectionVertex> FontAtlas::getSelectionGeometry(std::string_view text, size_t begin, size_t end) const {
    std::vector<TextSelectionVertex> vertices;

    auto addRect = [&] (glm::vec2 pos, glm::vec2 size) {
        vertices.emplace_back(glm::vec2{pos.x, -pos.y + size.y});
        vertices.emplace_back(glm::vec2{pos.x, -pos.y});
        vertices.emplace_back(glm::vec2{pos.x + size.x, -pos.y});

        vertices.emplace_back(glm::vec2{pos.x, -pos.y + size.y});
        vertices.emplace_back(glm::vec2{pos.x + size.x, -pos.y});
        vertices.emplace_back(glm::vec2{pos.x + size.x, -pos.y + size.y});
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
                offset.y += pixel_size;
            }
            ++pos;
            return true;

        } else if (pos < end) {
            if (cp == '\n') {
                if (size != 0) {
                    // Finish this selection line.
                    addRect({offset.x, offset.y}, glm::vec2{size, pixel_size});
                    size = 0;
                    offset.x = 0;
                    offset.y += pixel_size;
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

    addRect({offset.x, offset.y}, glm::vec2{size, pixel_size});

    return vertices;
}

const FontChar& FontAtlas::getFontChar(uint32_t utf32_codepoint) const noexcept {
    auto it = chars.find(utf32_codepoint);
    if (it == chars.end()) {
        return chars.at(UNDEFINED_GLYPH_CHAR_CODE);
    }

    return it->second;
}
