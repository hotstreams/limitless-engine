#include <limitless/core/texture/texture_uniform_setter.hpp>

#include <limitless/core/uniform/uniform_value.hpp>
#include <limitless/core/texture/bindless_texture.hpp>
#include <glm/vec4.hpp>

using namespace Limitless;

TextureUniformSetter::TextureUniformSetter(UniformValue<int32_t>& uniform) noexcept
    : uniform {uniform} {}

void TextureUniformSetter::visit(BindlessTexture& texture) noexcept {
    // make sure its resident before use
    texture.makeResident();

    // set handle to uniform
    glUniformHandleui64ARB(uniform.getLocation(), texture.getHandle());
}

void TextureUniformSetter::visit([[maybe_unused]] ExtensionTexture& texture) noexcept {
    // because it is not stored in uniform block in that case
    // just set texture unit as usual
    uniform.UniformValue::set();
}
