#pragma once

#include <limitless/core/vertex_stream/vertex_stream.hpp>
#include <limitless/core/indirect/indirect_draw_command.hpp>

namespace Limitless {
    class IndexedStream : public VertexStream {
    protected:
        std::vector<uint32_t> indices;
        std::shared_ptr<Buffer> indices_buffer;

        friend class Builder;

        IndexedStream(
            Type type,
            std::map<Attribute, AttributeIndex> attributes,
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
        );
    public:
        const std::vector<uint32_t>& getIndices() const;

        void draw() override;
        void draw_instanced(size_t instance_count) override;
        void draw_instanced(size_t instance_count, uint32_t base_instance) override;

        void draw(size_t offset, size_t count) override;
        void draw_instanced(size_t offset, size_t count, size_t instance_count) override;
        void draw_instanced(size_t offset, size_t count, size_t instance_count, uint32_t base_instance) override;

        void draw(Draw mode, size_t offset, size_t count) override;
        void draw_instanced(Draw mode, size_t offset, size_t count, size_t instance_count) override;
        void draw_instanced(Draw mode, size_t offset, size_t count, size_t instance_count, uint32_t base_instance) override;

        /**
         * Draw a portion of batched geometry using base vertex
         *
         * Uses glDrawElementsBaseVertex to draw a specific subset of the
         * batched vertex stream, identified by index count, first index,
         * and base vertex offset.
         *
         * @param info MeshDrawInfo containing offsets and counts
         */
        void drawBatched(const MeshDrawInfo& info);

        /**
         * Draw instanced a portion of batched geometry using base vertex
         */
        void drawBatchedInstanced(const MeshDrawInfo& info, size_t instance_count);
        void drawBatchedInstanced(const MeshDrawInfo& info, size_t instance_count, uint32_t base_instance);

        void map();
        void merge(const VertexStream &other) override;
    };
}