#include <limitless/models/text_selection_model.hpp>

#include <limitless/core/buffer/buffer_builder.hpp>

#include "limitless/core/vertex_stream/vertex_stream_builder.hpp"

using namespace Limitless;

TextSelectionModel::TextSelectionModel(std::vector<TextSelectionVertex>&& vertices)
    : vertices{std::move(vertices)}
    , vertex_stream(VertexStream::builder()
             .attribute(0, VertexStream::Attribute::Position, sizeof(TextSelectionVertex), offsetof(TextSelectionVertex, position))
             .vertices(vertices)
             .usage(VertexStream::Usage::Dynamic)
             .draw(VertexStream::Draw::Triangles)
             .build()
    ) {
}

TextSelectionModel::TextSelectionModel(size_t count)
    : vertices {count}
    , vertex_stream(VertexStream::builder()
             .attribute(0, VertexStream::Attribute::Position, sizeof(TextSelectionVertex), offsetof(TextSelectionVertex, position))
             .count(count)
             .usage(VertexStream::Usage::Dynamic)
             .draw(VertexStream::Draw::Triangles)
             .build()
    ) {
}

void TextSelectionModel::update(std::vector<TextSelectionVertex>&& _vertices) {
    vertices = std::move(_vertices);

    vertex_stream->update(vertices);
}

void TextSelectionModel::draw() const {
    vertex_stream->draw();
}

TextSelectionModel::~TextSelectionModel() = default;
