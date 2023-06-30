#include <limitless/core/texture/texture_uniform_alignment_getter.hpp>

#include <glm/vec4.hpp>

using namespace Limitless;

TextureUniformAlignmentGetter::TextureUniformAlignmentGetter(size_t& alignment) noexcept
    : alignment {alignment} {}

void TextureUniformAlignmentGetter::visit([[maybe_unused]] BindlessTexture& texture) noexcept {
    alignment = sizeof(uint64_t);
}

void TextureUniformAlignmentGetter::visit([[maybe_unused]] ExtensionTexture& texture) noexcept {
    // because it is not stored in uniform block in that case
    alignment = 0;
}
