#pragma once

namespace GraphicsEngine {
    class ExtensionTexture;
    class BindlessTexture;

    class TextureVisitor {
    public:
        virtual void visit(BindlessTexture& texture) noexcept = 0;
        virtual void visit(ExtensionTexture& texture) noexcept = 0;

        virtual ~TextureVisitor() = default;
    };
}
