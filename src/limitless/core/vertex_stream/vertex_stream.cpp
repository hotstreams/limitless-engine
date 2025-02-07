#include <limitless/core/vertex_stream/vertex_stream.hpp>
#include <limitless/core/vertex_stream/vertex_stream_builder.hpp>
#include <utility>

using namespace Limitless;

VertexStream::VertexStream(
    Type type,
    std::map<uint8_t, std::string>&& name_mapping,
    InputType input_type,
    Draw mode,
    Usage usage,
    std::vector<std::byte> data,
    bool data_kept,
    std::shared_ptr<VertexArray> vertex_array,
    std::shared_ptr<Buffer> vertex_buffer
)
    : name_mapping(std::move(name_mapping))
    , type(type)
    , input_type(std::move(input_type))
    , mode(mode)
    , usage(usage)
    , data(std::move(data))
    , data_kept(data_kept)
    , vertex_array(std::move(vertex_array))
    , vertex_buffer(std::move(vertex_buffer))
{

}

void VertexStream::draw() {
    draw(mode, 0, data.size());
}

void VertexStream::draw_instanced(size_t instance_count) {
    draw_instanced(mode, 0, data.size(), instance_count);
}

void VertexStream::draw(size_t offset, size_t count) {
    draw(mode, offset, count);
}

void VertexStream::draw_instanced(size_t offset, size_t count, size_t instance_count) {
    draw_instanced(mode, offset, count, instance_count);
}

void VertexStream::draw(Limitless::VertexStream::Draw draw_mode, size_t offset, size_t count) {
    if (data.empty()) {
        return;
    }

    vertex_array->bind();

    glDrawArrays(static_cast<GLenum>(draw_mode), offset, count);

    vertex_buffer->fence();
}

void VertexStream::draw_instanced(Limitless::VertexStream::Draw draw_mode, size_t offset, size_t count, size_t instance_count) {
    if (data.empty()) {
        return;
    }

    vertex_array->bind();

    glDrawArraysInstanced(static_cast<GLenum>(draw_mode), offset, count, instance_count);

    vertex_buffer->fence();
}

void VertexStream::merge(const VertexStream& other) {
    if (type != other.type) {
        throw std::runtime_error("Vertex stream type mismatch");
    }

    if (name_mapping != other.name_mapping) {
        throw std::runtime_error("Vertex stream name mappings mismatch");
    }

    if (input_type != other.input_type) {
        throw std::runtime_error("Vertex stream input type mismatch");
    }

    data.insert(data.end(), other.data.begin(), other.data.end());

    map();
}

VertexStream::Builder VertexStream::builder() {
    return {};
}