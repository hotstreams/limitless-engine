#pragma once

namespace GraphicsEngine {
    class Texture;
    class BindlessTexture;

    class TextureVisitor {
    public:
        virtual void visit(BindlessTexture& texture) noexcept = 0;
        virtual void visit(Texture& texture) noexcept = 0;

        virtual ~TextureVisitor() = default;
    };
}
