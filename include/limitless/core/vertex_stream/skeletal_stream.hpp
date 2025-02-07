#pragma once

#include <limitless/core/vertex_stream/indexed_stream.hpp>
#include <limitless/models/bones.hpp>

namespace Limitless {
    class SkeletalStream : public IndexedStream {
    private:
        std::vector<VertexBoneWeight> bone_weights;
        std::shared_ptr<Buffer> bone_buffer;

        friend class Builder;

        SkeletalStream(
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
        );
    public:
        auto& getBoneWeights() noexcept { return bone_weights; }
        const auto& getBoneWeights() const noexcept { return bone_weights; }

        void map();
        void merge(const VertexStream &other) override;
    };
}