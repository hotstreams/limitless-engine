#pragma once

#include <limitless/core/texture/texture_visitor.hpp>
#include <cstddef>
#include <vector>

namespace Limitless {
    class ExtensionTexture;
    /*
     * Texture in Material may be used in Uniform Block if actual type is bindless
     *
     * if so, we should get right size for sampler which is the size of handle - uint64
     */
    class TextureExtensionCapturer : public TextureVisitor {
    private:
        std::vector<ExtensionTexture*>& textures;
    public:
        explicit TextureExtensionCapturer(std::vector<ExtensionTexture*>& textures) noexcept;

        void visit(BindlessTexture& texture) noexcept override;
        void visit(ExtensionTexture& texture) noexcept override;
    };
}
