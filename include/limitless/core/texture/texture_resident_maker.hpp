#pragma once

#include <limitless/core/texture/texture_visitor.hpp>
#include <cstdint>

namespace Limitless {
    /*
     * Makes bindless texture resident for use
     */
    class TextureResidentMaker : public TextureVisitor {
    public:
        void visit(BindlessTexture& texture) noexcept override;
        void visit(ExtensionTexture& texture) noexcept override;
    };
}
