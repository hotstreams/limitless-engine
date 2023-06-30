#include <limitless/core/texture/texture_extension_capturer.hpp>

using namespace Limitless;

TextureExtensionCapturer::TextureExtensionCapturer(std::vector<ExtensionTexture*>& textures) noexcept
    : textures {textures} {}

void TextureExtensionCapturer::visit([[maybe_unused]] ExtensionTexture& texture) noexcept {
    textures.emplace_back(&texture);
}

void TextureExtensionCapturer::visit([[maybe_unused]] BindlessTexture& texture) noexcept {

}
