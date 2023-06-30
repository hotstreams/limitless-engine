#pragma once

#include <limitless/core/texture/texture_visitor.hpp>
#include <cstdint>

namespace Limitless {
    template<typename T>
    class UniformValue;
    /*
     * Texture in Material may be used in Uniform Block if actual type is bindless
     *
     * if so, we should get right size for sampler which is the size of handle - uint64
     */
    class TextureUniformSetter : public TextureVisitor {
    private:
        UniformValue<int32_t>& uniform;
    public:
        explicit TextureUniformSetter(UniformValue<int32_t>& uniform) noexcept;

        void visit(BindlessTexture& texture) noexcept override;
        void visit(ExtensionTexture& texture) noexcept override;
    };
}
