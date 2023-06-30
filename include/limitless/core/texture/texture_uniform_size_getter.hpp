#pragma once

#include <limitless/core/texture/texture_visitor.hpp>
#include <cstddef>

namespace Limitless {
    /*
     * Texture in Material may be used in Uniform Block if actual type is bindless
     *
     * if so, we should get right size for sampler which is the size of handle - uint64
     */
    class TextureUniformSizeGetter : public TextureVisitor {
    private:
        size_t& size;
    public:
        explicit TextureUniformSizeGetter(size_t& size) noexcept;

        void visit(BindlessTexture& texture) noexcept override;
        void visit(ExtensionTexture& texture) noexcept override;
    };
}
