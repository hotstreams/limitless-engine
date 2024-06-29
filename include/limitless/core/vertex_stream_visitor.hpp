#pragma once

namespace Limitless {
    class ExtensionTexture;
    class BindlessTexture;

    class VertexStreamVisitor {
    public:
        virtual void visit(VertexStream& texture) noexcept = 0;
        virtual void visit(ExtensionTexture& texture) noexcept = 0;

        virtual ~VertexStreamVisitor() = default;
    };
}
