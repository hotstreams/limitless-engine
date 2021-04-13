#include <limitless/models/text_model.hpp>

#include <limitless/core/buffer_builder.hpp>

using namespace LimitlessEngine;

TextModel::TextModel(std::vector<TextVertex>&& _vertices)
    : vertices {_vertices} {
    initialize(vertices.size());
}

TextModel::TextModel(size_t count)
    : vertices {} {
    initialize(count);
}

void TextModel::initialize(size_t count) {
    BufferBuilder builder;
    buffer = builder .setTarget(Buffer::Type::Array)
                     .setData(vertices.empty() ? nullptr : vertices.data())
                     .setDataSize(count * sizeof(TextVertex))
                     .setUsage(Buffer::Usage::DynamicDraw)
                    . setAccess(Buffer::MutableAccess::WriteOrphaning)
                     .build();

    vertex_array << std::pair<TextVertex, Buffer&>(TextVertex{}, *buffer);
}

void TextModel::update(std::vector<TextVertex>&& _vertices) {
    vertices = std::move(_vertices);

    if (vertices.size() * sizeof(TextVertex) > buffer->getSize()) {
        initialize(vertices.size());
        //TODO:: buffer resize function ??? wtf???
    } else {
        buffer->mapData(vertices.data(), vertices.size() * sizeof(TextVertex));
    }
}

void TextModel::draw() const {
    if (vertices.empty()) {
        return;
    }

    vertex_array.bind();

    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}