#pragma once

#include <limitless/core/vertex_array.hpp>
#include <limitless/core/buffer/buffer_builder.hpp>
#include <limitless/core/abstract_vertex_stream.hpp>

namespace Limitless {
    template <typename Vertex>
    class VertexStream : public AbstractVertexStream {
    protected:
        std::shared_ptr<Buffer> vertex_buffer;
        VertexArray vertex_array;
        std::vector<Vertex> stream;
        VertexStreamUsage usage;
        VertexStreamDraw mode;

        void initialize(size_t count) {
            BufferBuilder builder;
            builder .setTarget(Buffer::Type::Array)
                    .setDataSize(count * sizeof(Vertex))
                    .setData(stream.empty() ? nullptr : stream.data());

            switch (usage) {
                case VertexStreamUsage::Static:
                    builder .setUsage(Buffer::Storage::Static)
                            .setAccess(Buffer::ImmutableAccess::None);
                    break;
                case VertexStreamUsage::Dynamic:
                    builder .setUsage(Buffer::Usage::DynamicDraw)
                            .setAccess(Buffer::MutableAccess::WriteOrphaning);
                    break;
                case VertexStreamUsage::Stream:
                    //TODO: should be x3 vertex array && x3 buffer
                    builder .setUsage(Buffer::Storage::DynamicCoherentWrite)
                            .setAccess(Buffer::ImmutableAccess::WriteCoherent);
                    break;
            }

            vertex_buffer = builder.build();

            //TODO: make top vertex-struct interface (something like bytebuffer + ct string)
            vertex_array << std::pair<Vertex, const std::shared_ptr<Buffer>&>(Vertex{}, vertex_buffer);
        }
    public:
        explicit VertexStream(std::vector<Vertex>&& _stream, VertexStreamUsage _usage, VertexStreamDraw _draw) noexcept
            : stream {std::move(_stream)}
            , usage {_usage}
            , mode {_draw} {
            initialize(stream.size());
        }

        explicit VertexStream(size_t count, VertexStreamUsage _usage, VertexStreamDraw _draw) noexcept
            : usage {_usage}
            , mode {_draw} {
            stream.reserve(count);
            initialize(count);
        }

        ~VertexStream() override = default;

        VertexStream(const VertexStream& rhs)
            : vertex_buffer {rhs.vertex_buffer->clone()}
            , vertex_array {rhs.vertex_array}
            , stream {rhs.stream}
            , usage {rhs.usage}
            , mode {rhs.mode} {
        }

        VertexStream(VertexStream&&) noexcept = default;

        VertexStream& operator+(const VertexStream& rhs) {
            stream.insert(rhs.stream.begin(), rhs.stream.end());
            map();
        }

        auto& getVertices() noexcept { return stream; }
        const auto& getVertices() const noexcept { return stream; }

        void map() {
            const auto size = stream.size() * sizeof(Vertex);

            if (size > vertex_buffer->getSize()) {
                vertex_buffer->resize(size);
                vertex_array << std::pair<Vertex, const std::shared_ptr<Buffer>&>(Vertex{}, vertex_buffer);
            }

            vertex_buffer->mapData(stream.data(), size);
        }

        template <typename Vertices>
        void update(Vertices&& vertices) {
            stream = std::forward<Vertices>(vertices);
            map();
        }

        void draw(VertexStreamDraw draw_mode) noexcept override {
            if (stream.empty()) {
                return;
            }

            vertex_array.bind();

            glDrawArrays(static_cast<GLenum>(draw_mode), 0, stream.size());

            vertex_buffer->fence();
        }

        void draw_instanced(VertexStreamDraw draw_mode, std::size_t count) noexcept override {
            if (stream.empty()) {
                return;
            }

            vertex_array.bind();

            glDrawArraysInstanced(static_cast<GLenum>(draw_mode), 0, stream.size(), count);

            vertex_buffer->fence();
        }

        void draw() noexcept override {
            draw(mode);
        }

        void draw_instanced(std::size_t count) noexcept override {
            draw_instanced(mode, count);
        }
    };
}
