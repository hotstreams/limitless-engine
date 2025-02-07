#pragma once

#include <limitless/core/vertex_stream/vertex_stream.hpp>

namespace Limitless {
    class IndexedStream : public VertexStream {
    protected:
        std::vector<uint32_t> indices;
        std::shared_ptr<Buffer> indices_buffer;

        friend class Builder;

        IndexedStream(
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
        );
    public:
        const std::vector<uint32_t>& getIndices() const;

        void draw() override;
        void draw_instanced(size_t instance_count) override;

        void draw(size_t offset, size_t count) override;
        void draw_instanced(size_t offset, size_t count, size_t instance_count) override;

        void draw(Draw mode, size_t offset, size_t count) override;
        void draw_instanced(Draw mode, size_t offset, size_t count, size_t instance_count) override;

        void map();
        void merge(const VertexStream &other) override;
    };
}