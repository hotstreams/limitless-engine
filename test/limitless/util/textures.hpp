#pragma once

#include <limitless/core/texture/texture_builder.hpp>

namespace LimitlessTest {
    using namespace Limitless;

    class Textures {
    public:
        static inline std::shared_ptr<Texture> fake() {
            TextureBuilder builder {};

            return builder.setTarget(Texture::Type::Tex2D)
                    .setMipMap(false)
                    .setInternalFormat(Texture::InternalFormat::R8)
                    .setFormat(Texture::Format::Red)
                    .setDataType(Texture::DataType::UnsignedByte)
                    .build();
        }
    };
}