#include <limitless/models/text_model.hpp>

#include <limitless/core/buffer/buffer_builder.hpp>

using namespace Limitless;

TextModel::TextModel(std::vector<TextVertex>&& _vertices)
    : vertices {_vertices}
    , vertex_array(
        VertexArray::builder()
            .build()
        ) {
    initialize(vertices.size());
}

//TextModel::TextModel(size_t count)
//    : vertices {} {
//    initialize(count);
//}

void TextModel::initialize(size_t count) {
    buffer = Buffer::builder()
            .target(Buffer::Type::Array)
            .data(vertices.empty() ? nullptr : vertices.data())
            .size(count * sizeof(TextVertex))
            .usage(Buffer::Usage::DynamicDraw)
            .access(Buffer::MutableAccess::WriteOrphaning)
            .build();

//    vertex_array << std::pair<TextVertex, const std::shared_ptr<Buffer>&>(TextVertex{}, buffer);
}

void TextModel::update(std::vector<TextVertex>&& _vertices) {
    vertices = std::move(_vertices);

    if (vertices.size() * sizeof(TextVertex) > buffer->getSize()) {
        buffer->resize(vertices.size() * sizeof(TextVertex));
    }

    buffer->mapData(vertices.data(), vertices.size() * sizeof(TextVertex));
}

void TextModel::draw() const {
    if (vertices.empty()) {
        return;
    }

//    vertex_array.bind();

    // glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}