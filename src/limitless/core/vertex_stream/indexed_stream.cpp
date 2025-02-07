#include <algorithm>
#include <limitless/core/vertex_stream/indexed_stream.hpp>
#include <utility>

using namespace Limitless;

IndexedStream::IndexedStream(
    Type type,
    std::map<uint8_t, std::string>&& name_mapping,
    InputType input_type,
    Draw draw,
    Usage usage,
    std::vector<std::byte> data,
    bool data_kept,
    std::shared_ptr<VertexArray> vertex_array,
    std::shared_ptr<Buffer> vertex_buffer,
    std::vector<uint32_t> indices,
    std::shared_ptr<Buffer> indices_buffer
)
    : VertexStream(
        type,
        std::move(name_mapping),
        std::move(input_type),
        draw,
        usage,
        std::move(data),
        data_kept,
        std::move(vertex_array),
        std::move(vertex_buffer))
    , indices(std::move(indices))
    , indices_buffer(std::move(indices_buffer))
{

}

const std::vector<uint32_t>& IndexedStream::getIndices() const {
    return indices;
}

void IndexedStream::draw() {
    draw(mode, 0, indices.size());
}

void IndexedStream::draw_instanced(size_t instance_count) {
    draw_instanced(mode, 0, indices.size(), instance_count);
}

void IndexedStream::draw(size_t offset, size_t count) {
    draw(mode, offset, count);
}

void IndexedStream::draw_instanced(size_t offset, size_t count, size_t instance_count) {
    draw_instanced(mode, offset, count, instance_count);
}

void IndexedStream::draw(Draw draw_mode, size_t offset, size_t count) {
    if (data.empty()) {
        return;
    }

    vertex_array->bind();

    glDrawElements(static_cast<GLenum>(draw_mode), count, GL_UNSIGNED_INT, reinterpret_cast<const void*>(offset));

    vertex_buffer->fence();
    indices_buffer->fence();
}

void IndexedStream::draw_instanced(Draw draw_mode, size_t offset, size_t count, size_t instance_count) {
    if (data.empty()) {
        return;
    }

    vertex_array->bind();

    glDrawElementsInstanced(static_cast<GLenum>(draw_mode), count, GL_UNSIGNED_INT, reinterpret_cast<const void*>(offset), instance_count);

    vertex_buffer->fence();
    indices_buffer->fence();
}

void IndexedStream::map() {
    const auto size = indices.size() * sizeof(uint32_t);

    //TODO: shrink to fit sometimes?
    if (size > indices_buffer->getSize()) {
        indices_buffer->resize(size);
    }

    indices_buffer->mapData(indices.data(), size);
}

void IndexedStream::merge(const VertexStream& other) {
    auto vertex_size = data.size() / vertex_array->getAttributes().at(0).stride;
    VertexStream::merge(other);

    auto cp = static_cast<const IndexedStream&>(other).indices;

    std::transform(cp.cbegin(), cp.cend(), cp.begin(), [vertex_size](auto index) {
        return index + vertex_size;
    });

    indices.insert(indices.end(), cp.begin(), cp.end());

    map();
}
