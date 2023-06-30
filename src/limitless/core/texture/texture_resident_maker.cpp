#include <limitless/core/texture/texture_resident_maker.hpp>

#include <limitless/core/bindless_texture.hpp>

using namespace Limitless;

void TextureResidentMaker::visit(BindlessTexture& texture) noexcept {
    texture.makeResident();
}

void TextureResidentMaker::visit([[maybe_unused]] ExtensionTexture& texture) noexcept {

}
