#pragma once

#include <limitless/core/vertex_stream/vertex_stream.hpp>
#include <limitless/core/indirect/indirect_draw_command.hpp>
#include <limitless/models/bones.hpp>

#include <memory>
#include <cstring>
#include <optional>

namespace Limitless {
    class VertexStream::Builder {
    private:
        static inline std::unordered_map<Attribute, std::string> ATTRIBUTE_NAMINGS = {
            { Attribute::Position, "position" },
            { Attribute::Normal, "normal" },
            { Attribute::Tangent, "tangent" }, // vec4 (xyz + handedness)
            { Attribute::Uv, "uv" },
            { Attribute::Uv1, "uv1" },
            { Attribute::Uv2, "uv2" },
            { Attribute::Uv3, "uv3" },
            { Attribute::Uv4, "uv4" },
            { Attribute::Uv5, "uv5" },
            { Attribute::BoneIndices, "bone_index" },
            { Attribute::BoneWeights, "bone_weight" },
            { Attribute::MeshIndex, "mesh_index" },
            { Attribute::Color, "color" }
        };

        // Thread-local storage for last build's draw info (for indirect mode)
        static thread_local std::optional<MeshDrawInfo> last_draw_info_;

        std::map<Attribute, AttributeIndex> attributes;
        std::map<AttributeIndex, VertexArray::Attribute> vertex_attributes;
        std::map<AttributeIndex, std::string> name_mapping;
        InputType stream_type;

        std::vector<std::byte> vertex_data;
        std::vector<uint32_t> indices_data;
        std::vector<VertexBoneWeight> bone_weights;
        size_t count_;

        std::vector<std::shared_ptr<VertexStream>> batches;

        bool data_kept;
        Usage usage_mode;
        Draw draw_mode;

        // Flag to enable batching for this build
        bool use_batching_ {false};

        bool isIndexedStream();
        bool isSkeletalStream();

        Type getStreamType();
        std::shared_ptr<Buffer> buildVertexBuffer();
        std::shared_ptr<Buffer> buildIndexBuffer();
        std::shared_ptr<Buffer> buildSkeletalBuffer();
        std::shared_ptr<VertexArray> buildVertexArray();

        // Build using GeometryPool for indirect draw batching
        std::shared_ptr<VertexStream> buildBatched();

        // Calculate vertex stride from attributes
        size_t calculateVertexStride() const;

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

        /**
         * Enable batching for indirect draw mode
         *
         * When enabled, geometry will be added to a global BatchedVertexStream
         * via GeometryPool instead of creating a separate buffer.
         */
        Builder& batched(bool enable = true);

        std::shared_ptr<VertexStream> build();

        /**
         * Get draw info from last build() call
         *
         * Only valid when batched mode was used.
         * Returns nullopt if last build didn't use batching.
         */
        static std::optional<MeshDrawInfo> getLastDrawInfo() { return last_draw_info_; }

        /**
         * Clear last draw info
         */
        static void clearLastDrawInfo() { last_draw_info_ = std::nullopt; }

        template<typename Vertex>
        Builder& vertices(const std::vector<Vertex>& vertices) {
            vertex_data.resize(sizeof(Vertex) * vertices.size());
            std::memcpy(vertex_data.data(), vertices.data(), sizeof(Vertex) * vertices.size());
            return *this;
        }
    };
}