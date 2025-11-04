#include <limitless/models/text_model.hpp>

#include <limitless/core/buffer/buffer_builder.hpp>

#include "limitless/core/vertex_stream/vertex_stream_builder.hpp"

using namespace Limitless;

TextModel::TextModel(std::vector<TextVertex>&& _vertices)
    : vertices {_vertices}
    , vertex_stream(VertexStream::builder()
                 .attribute(0, VertexStream::Attribute::Position, sizeof(TextVertex), offsetof(TextVertex, position))
                 .attribute(1, VertexStream::Attribute::Uv, sizeof(TextVertex), offsetof(TextVertex, uv))
                 .attribute(2, VertexStream::Attribute::Color, sizeof(TextVertex), offsetof(TextVertex, color))
                 .vertices(vertices)
                 .usage(VertexStream::Usage::Dynamic)
                 .draw(VertexStream::Draw::Triangles)
                 .build()
        ) {
}

TextModel::TextModel(size_t count)
    : vertices {count}
    , vertex_stream(VertexStream::builder()
             .attribute(0, VertexStream::Attribute::Position, sizeof(TextVertex), offsetof(TextVertex, position))
             .attribute(1, VertexStream::Attribute::Uv, sizeof(TextVertex), offsetof(TextVertex, uv))
             .attribute(2, VertexStream::Attribute::Color, sizeof(TextVertex), offsetof(TextVertex, color))
             .count(count)
             .usage(VertexStream::Usage::Dynamic)
             .draw(VertexStream::Draw::Triangles)
             .build()
    ) {
}

void TextModel::update(std::vector<TextVertex>&& _vertices) {
    vertices = std::move(_vertices);

    vertex_stream->update(vertices);
}

void TextModel::draw() const {
    vertex_stream->draw();
}
