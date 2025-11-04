#pragma once

#include <limitless/core/context_debug.hpp>
#include <limitless/util/data_type.hpp>
#include <limitless/core/vertex_stream/vertex_array.hpp>

#include <vector>
#include <map>
#include <string>
#include <cstring>

namespace Limitless {
    class VertexStream {
    public:
        enum class Type {
            Vertex,
            Indexed,
            Skeletal
        };

        enum class Attribute {
            Position,
            Normal,
            Tangent,
            Uv,
            BoneIndices,
            BoneWeights,
            MeshIndex,
            Color
        };

        enum class Usage {
            Static,
            Dynamic,
            Stream
        };

        enum class Draw {
            Triangles = GL_TRIANGLES,
            TriangleStrip = GL_TRIANGLE_STRIP,
            TriangleStripAdj = GL_TRIANGLE_STRIP_ADJACENCY,
            TriangleFan = GL_TRIANGLE_FAN,
            Lines = GL_LINES,
            LineLoop = GL_LINE_LOOP,
            Points = GL_POINTS,
            Patches = GL_PATCHES
        };

        using DataType = DataType;
        using InputType = std::map<uint8_t, DataType>;
        using AttributeIndex = uint8_t;
    protected:
        std::map<Attribute, AttributeIndex> attributes;
        std::map<AttributeIndex, std::string> name_mapping;
        Type type;
        InputType input_type;
        Draw mode;
        Usage usage;

        std::vector<std::byte> data;
        bool data_kept;

        std::shared_ptr<VertexArray> vertex_array;
        std::shared_ptr<Buffer> vertex_buffer;

        VertexStream(
            Type type,
            std::map<Attribute, AttributeIndex> attributes,
            std::map<uint8_t, std::string>&& name_mapping,
            InputType input_type,
            Draw mode,
            Usage usage,
            std::vector<std::byte> data,
            bool data_kept,
            std::shared_ptr<VertexArray> vertex_array,
            std::shared_ptr<Buffer> vertex_buffer
        );

    public:
        virtual ~VertexStream() = default;

        VertexStream(const VertexStream&) = delete;
        VertexStream(VertexStream&&) = default;

        const auto& getNameMappings() const noexcept { return name_mapping; }
        const auto& getType() const noexcept { return type; }
        const auto& getInputType() const noexcept { return input_type; }
        const auto& getDrawMode() const noexcept { return mode; }
        const auto& getUsage() const noexcept { return usage; }
        const auto& getData() const noexcept { return data; }
        const auto& getVertexArray() const noexcept { return vertex_array; }
        bool isDataKept() const noexcept { return data_kept; }

        virtual void draw();
        virtual void draw_instanced(size_t instance_count);

        virtual void draw(size_t offset, size_t count);
        virtual void draw_instanced(size_t offset, size_t count, size_t instance_count);

        virtual void draw(Draw mode, size_t offset, size_t count);
        virtual void draw_instanced(Draw mode, size_t offset, size_t count, size_t instance_count);

        virtual void merge(const VertexStream& other);

        void map() {
            const auto size = data.size();
            
            // Don't map empty buffers
            if (size == 0) {
                return;
            }

            //TODO: shrink to fit sometimes?
            if (size > vertex_buffer->getSize()) {
                vertex_array->resize(vertex_buffer, size);
            }

            vertex_buffer->mapData(data.data(), data.size());
        }

        void update(std::vector<std::byte> data);

        template<typename T>
        void forEach(Attribute attribute, std::function<void(const T&)> function) {
            if (attributes.find(attribute) == attributes.end()) {
                return;
            }

            const auto size = vertex_array->getAttributes().at(attributes.at(attribute)).stride;
            const auto offset = vertex_array->getAttributes().at(attributes.at(attribute)).pointer;

            for (size_t i = 0; i < data.size(); i += size) {
                const auto& value = reinterpret_cast<const T&>(data[i + reinterpret_cast<size_t>(offset)]);
                function(value);
            }
        }

        template<typename Vertex>
        void update(std::vector<Vertex> vertex_data) {
            data.resize(sizeof(Vertex) * vertex_data.size());
            std::memcpy(data.data(), vertex_data.data(), sizeof(Vertex) * vertex_data.size());
            map(); // Upload to GPU
        }

        class Builder;
        static Builder builder();
    };
}