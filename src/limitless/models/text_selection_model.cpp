#include <limitless/models/text_selection_model.hpp>

#include <limitless/core/buffer/buffer_builder.hpp>

using namespace Limitless;

TextSelectionModel::TextSelectionModel(TextSelectionModel&& other) noexcept {
    swap(*this, other);
}

TextSelectionModel& TextSelectionModel::operator=(TextSelectionModel&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    swap(*this, other);
    return *this;
}

void Limitless::swap(TextSelectionModel& lhs, TextSelectionModel& rhs) noexcept {
    using std::swap;
    swap(lhs.vertex_array, rhs.vertex_array);
    swap(lhs.buffer, rhs.buffer);
    swap(lhs.vertices, rhs.vertices);
}


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

TextSelectionModel::~TextSelectionModel() = default;
