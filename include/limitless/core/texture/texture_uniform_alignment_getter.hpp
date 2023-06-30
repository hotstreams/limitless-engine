#pragma once

#include <limitless/core/texture/texture_visitor.hpp>
#include <cstddef>

namespace Limitless {
    /*
     * Texture in Material may be used in Uniform Block if actual type is bindless
     *
     * if so, we should get right alignment for sampler which is rounded up to vec4
     */
    class TextureUniformAlignmentGetter : public TextureVisitor {
    private:
        size_t& alignment;
    public:
        explicit TextureUniformAlignmentGetter(size_t& alignment) noexcept;

        void visit(BindlessTexture& texture) noexcept override;
        void visit(ExtensionTexture& texture) noexcept override;
    };
}
