#pragma once

namespace GraphicsEngine {
    class Texture;
    class BindlessTexture;

    class TextureVisitor {
    public:
        virtual void visit(const BindlessTexture& texture) noexcept = 0;
        virtual void visit(const Texture& texture) noexcept = 0;
    };
}
