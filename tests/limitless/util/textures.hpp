#pragma once

#include <limitless/core/texture/texture_builder.hpp>

namespace LimitlessTest {
    using namespace Limitless;

    class Textures {
    public:
        static inline std::shared_ptr<Texture> fake() {
            Texture::Builder builder = Texture::builder();

            return builder.target(Texture::Type::Tex2D)
                    .mipmap(false)
                    .internal_format(Texture::InternalFormat::R8)
                    .format(Texture::Format::Red)
                    .data_type(Texture::DataType::UnsignedByte)
                    .build();
        }
    };
}