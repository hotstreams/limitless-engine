#include <limitless/core/vertex_stream/skeletal_stream.hpp>
#include <utility>

using namespace Limitless;

SkeletalStream::SkeletalStream(
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
    std::shared_ptr<Buffer> indices_buffer,
    std::vector<VertexBoneWeight> bone_weights,
    std::shared_ptr<Buffer> bone_buffer
)
    : IndexedStream(
        type,
        std::move(name_mapping),
        std::move(input_type),
        draw,
        usage,
        std::move(data),
        data_kept,
        std::move(vertex_array),
        std::move(vertex_buffer),
        std::move(indices),
        std::move(indices_buffer)
    )
    , bone_weights(std::move(bone_weights))
    , bone_buffer(std::move(bone_buffer))
{

}

void SkeletalStream::map() {
    const auto size = bone_weights.size() * sizeof(VertexBoneWeight);

    //TODO: shrink to fit sometimes?
    if (size > bone_buffer->getSize()) {
        bone_buffer->resize(size);
    }

    bone_buffer->mapData(bone_weights.data(), size);
}

void SkeletalStream::merge(const VertexStream& other) {
    IndexedStream::merge(other);

    auto& rhs = static_cast<const SkeletalStream&>(other);

    bone_weights.insert(bone_weights.end(), rhs.bone_weights.begin(), rhs.bone_weights.end());

    map();
}

