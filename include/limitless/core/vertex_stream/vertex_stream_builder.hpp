#pragma once

#include <limitless/core/vertex_stream/vertex_stream.hpp>
#include <limitless/models/bones.hpp>

#include <memory>
#include <cstring>

namespace Limitless {
    class VertexStream::Builder {
    private:
        static inline std::unordered_map<Attribute, std::string> ATTRIBUTE_NAMINGS = {
            { Attribute::Position, "position" },
            { Attribute::Normal, "normal" },
            { Attribute::Tangent, "tangent" },
            { Attribute::Uv, "uv" },
            { Attribute::BoneIndices, "bone_index" },
            { Attribute::BoneWeights, "bone_weight" },
            { Attribute::MeshIndex, "mesh_index" }
        };

        std::unordered_map<GLuint, VertexArray::Attribute> attributes;
        std::map<uint8_t, std::string> name_mapping;
        InputType stream_type;

        std::vector<std::byte> vertex_data;
        std::vector<uint32_t> indices_data;
        std::vector<VertexBoneWeight> bone_weights;
        size_t count_;

        std::vector<std::shared_ptr<VertexStream>> batches;

        bool data_kept;
        Usage usage_mode;
        Draw draw_mode;

        bool isIndexedStream();
        bool isSkeletalStream();

        Type getStreamType();
        std::shared_ptr<Buffer> buildVertexBuffer();
        std::shared_ptr<Buffer> buildIndexBuffer();
        std::shared_ptr<Buffer> buildSkeletalBuffer();
        std::shared_ptr<VertexArray> buildVertexArray();

        DataType getDataType(Attribute attribute);
    public:
        Builder& from(const std::shared_ptr<VertexStream>& stream);
        Builder& attribute(uint8_t index, DataType type, const std::string& name, size_t stride, size_t offset);
        Builder& attribute(uint8_t index, Attribute attribute, size_t stride, size_t offset);
        Builder& normalized(size_t index, bool normalized);
        Builder& indices(const std::vector<uint32_t>& indices);
        Builder& bones(const std::vector<VertexBoneWeight>& bone_weights);
        Builder& keep_data(bool keep);
        Builder& usage(Usage usage);
        Builder& draw(Draw draw);

        Builder& count(size_t count);

        Builder& batch(const std::vector<std::shared_ptr<VertexStream>>& streams);

        std::shared_ptr<VertexStream> build();

        template<typename Vertex>
        Builder& vertices(const std::vector<Vertex>& vertices) {
            vertex_data.resize(sizeof(Vertex) * vertices.size());
            std::memcpy(vertex_data.data(), vertices.data(), sizeof(Vertex) * vertices.size());
            return *this;
        }
    };
}