#include <limitless/models/text_selection_model.hpp>

#include <limitless/core/buffer/buffer_builder.hpp>

using namespace Limitless;

TextSelectionModel::TextSelectionModel(std::vector<TextSelectionVertex>&& vertices)
    : vertices{std::move(vertices)}
{
    initialize(vertices.size());
}

TextSelectionModel::TextSelectionModel(size_t count) {
    initialize(count);
}

void TextSelectionModel::initialize(size_t count) {
    buffer = Buffer::builder()
            .target(Buffer::Type::Array)
            .data(vertices.empty() ? nullptr : vertices.data())
            .size(count * sizeof(TextSelectionVertex))
            .usage(Buffer::Usage::DynamicDraw)
            .access(Buffer::MutableAccess::WriteOrphaning)
            .build();

    vertex_array << std::pair<TextSelectionVertex, const std::shared_ptr<Buffer>&>(TextSelectionVertex{}, buffer);
}

void TextSelectionModel::update(std::vector<TextSelectionVertex>&& _vertices) {
    vertices = std::move(_vertices);

    if (vertices.size() * sizeof(TextSelectionVertex) > buffer->getSize()) {
        buffer->resize(vertices.size() * sizeof(TextSelectionVertex));
    }

    buffer->mapData(vertices.data(), vertices.size() * sizeof(TextSelectionVertex));
}

void TextSelectionModel::draw() const {
    vertex_array.bind();
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}
