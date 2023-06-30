#include <limitless/core/texture/texture_uniform_size_getter.hpp>

#include <glm/vec4.hpp>

using namespace Limitless;

TextureUniformSizeGetter::TextureUniformSizeGetter(size_t& size) noexcept
    : size {size} {}

void TextureUniformSizeGetter::visit([[maybe_unused]] BindlessTexture& texture) noexcept {
    size = sizeof(uint64_t);
}

void TextureUniformSizeGetter::visit([[maybe_unused]] ExtensionTexture& texture) noexcept {
    // because it is not stored in uniform block in that case
    size = 0;
}
