#include <limitless/core/vertex_stream/vertex_stream_builder.hpp>

#include <limitless/core/vertex_stream/vertex_array_builder.hpp>
#include <limitless/core/vertex_stream/skeletal_stream.hpp>
#include <limitless/core/buffer/buffer_builder.hpp>
#include <iostream>

using namespace Limitless;

bool VertexStream::Builder::isIndexedStream() {
    return !indices_data.empty();
}

bool VertexStream::Builder::isSkeletalStream() {
    return !bone_weights.empty();
}

VertexStream::Type VertexStream::Builder::getStreamType() {
    if (isSkeletalStream()) {
        return VertexStream::Type::Skeletal;
    }

    if (isIndexedStream()) {
        return VertexStream::Type::Indexed;
    }

    return VertexStream::Type::Vertex;
}

std::shared_ptr<Buffer> VertexStream::Builder::buildVertexBuffer() {
    auto& builder = Buffer::builder()
        .target(Buffer::Type::Array)
        .size(!vertex_data.empty() ? vertex_data.size() : count_)
        .data(vertex_data.empty() ? nullptr : vertex_data.data());

    switch (usage_mode) {
        case Usage::Static:
            // builder.usage(Buffer::Storage::Static).access(Buffer::ImmutableAccess::None);
            // builder.usage(Buffer::Usage::StaticDraw).access(Buffer::MutableAccess::WriteOrphaning);
        builder.usage(Buffer::Usage::DynamicDraw).access(Buffer::MutableAccess::WriteOrphaning);

            break;
        case Usage::Dynamic:
            builder.usage(Buffer::Usage::DynamicDraw).access(Buffer::MutableAccess::WriteOrphaning);
            break;
        case Usage::Stream:
            //TODO: should be x3 vertex array && x3 buffer
            builder.usage(Buffer::Storage::DynamicCoherentWrite).access(Buffer::ImmutableAccess::WriteCoherent);
            break;
    }

    return builder.build();
}

std::shared_ptr<Buffer> VertexStream::Builder::buildIndexBuffer() {
    auto& builder = Buffer::builder()
        .target(Buffer::Type::Element)
        .data(indices_data.data())
        .size(indices_data.size() * sizeof(uint32_t));

    switch (usage_mode) {
        case Usage::Static:
            // builder.usage(Buffer::Storage::Static).access(Buffer::ImmutableAccess::None);
        // builder.usage(Buffer::Usage::StaticDraw).access(Buffer::MutableAccess::WriteOrphaning);
        builder.usage(Buffer::Usage::DynamicDraw).access(Buffer::MutableAccess::WriteOrphaning);

            break;
        case Usage::Dynamic:
            builder.usage(Buffer::Usage::DynamicDraw).access(Buffer::MutableAccess::WriteOrphaning);
            break;
        case Usage::Stream:
            //TODO: should be x3 vertex array && x3 buffer
            builder.usage(Buffer::Storage::DynamicCoherentWrite).access(Buffer::ImmutableAccess::WriteCoherent);
            break;
    }

    return builder.build();
}

std::shared_ptr<Buffer> VertexStream::Builder::buildSkeletalBuffer() {
    auto& builder = Buffer::builder()
            .target(Buffer::Type::Array)
            .data(bone_weights.data())
            .size(bone_weights.size() * sizeof(VertexBoneWeight));

    switch (usage_mode) {
        case Usage::Static:
            // builder.usage(Buffer::Usage::StaticDraw).access(Buffer::MutableAccess::WriteOrphaning);
        builder.usage(Buffer::Usage::DynamicDraw).access(Buffer::MutableAccess::WriteOrphaning);

            // builder.usage(Buffer::Storage::Static).access(Buffer::ImmutableAccess::None);
            break;
        case Usage::Dynamic:
            builder.usage(Buffer::Usage::DynamicDraw).access(Buffer::MutableAccess::WriteOrphaning);
            break;
        case Usage::Stream:
            //TODO: should be x3 vertex array && x3 buffer
            builder.usage(Buffer::Storage::DynamicCoherentWrite).access(Buffer::ImmutableAccess::WriteCoherent);
            break;
    }

    return builder.build();
}

std::shared_ptr<VertexArray> VertexStream::Builder::buildVertexArray() {
    auto builder = VertexArray::builder();

    for (const auto& [index, attribute] : attributes) {
        builder.attribute(index, attribute);
    }

    return builder.build();
}


DataType VertexStream::Builder::getDataType(Attribute attribute) {
    switch (attribute) {
        case Attribute::Position: return DataType::Vec3;
        case Attribute::Normal: return DataType::Vec3;
        case Attribute::Tangent: return DataType::Vec3;
        case Attribute::Uv: return DataType::Vec2;
        case Attribute::BoneIndices: return DataType::IVec4;
        case Attribute::BoneWeights: return DataType::Vec4;
        case Attribute::MeshIndex: return DataType::Int;
    }
}

VertexStream::Builder& VertexStream::Builder::attribute(uint8_t index, VertexStream::DataType type, const std::string& name, size_t stride, size_t offset) {
    stream_type.emplace(index, type);
    auto [size, gl_data_type] = getOpenGLDataType(type);
    attributes.emplace(index, VertexArray::Attribute(size, gl_data_type, GL_FALSE, stride, reinterpret_cast<const GLvoid*>(offset), nullptr));
    name_mapping.emplace(index, name);
    return *this;
}

VertexStream::Builder& VertexStream::Builder::attribute(uint8_t index, VertexStream::Attribute attribute, size_t stride, size_t offset) {
    auto data_type = getDataType(attribute);
    stream_type.emplace(index, data_type);
    auto [size, gl_data_type] = getOpenGLDataType(data_type);
    attributes.emplace(index, VertexArray::Attribute(size, gl_data_type, GL_FALSE, stride, reinterpret_cast<const GLvoid*>(offset), nullptr));
    name_mapping.emplace(index, ATTRIBUTE_NAMINGS[attribute]);
    return *this;
}

VertexStream::Builder& VertexStream::Builder::normalized(size_t index, bool normalized) {
    attributes.at(index).normalized = normalized;
    return *this;
}

VertexStream::Builder& VertexStream::Builder::indices(const std::vector<uint32_t>& indices) {
    indices_data = indices;
    return *this;
}

VertexStream::Builder& VertexStream::Builder::bones(const std::vector<VertexBoneWeight>& bones) {
    bone_weights = bones;
    return *this;
}

VertexStream::Builder& VertexStream::Builder::count(size_t count) {
    count_ = count;
    return *this;
}

VertexStream::Builder& VertexStream::Builder::batch(const std::vector<std::shared_ptr<VertexStream>>& streams) {
    batches = streams;
    return *this;
}

std::shared_ptr<VertexStream> VertexStream::Builder::build() {
    if (vertex_data.empty()) {
        throw std::runtime_error("Vertex data should be specified");
    }

    auto vertex_buffer = buildVertexBuffer();
    for (auto& [index, attribute]: attributes) {
        attribute.buffer = vertex_buffer;
    }

    VertexStream* stream {};
    switch (getStreamType()) {
        case VertexStream::Type::Vertex: {
            auto vertex_array = buildVertexArray();
            stream = new VertexStream(
                    getStreamType(),
                    std::move(name_mapping),
                    std::move(stream_type),
                    draw_mode,
                    usage_mode,
                    std::move(vertex_data),
                    data_kept,
                    vertex_array,
                    vertex_buffer
            );
        }
            break;
        case VertexStream::Type::Indexed: {
            auto vertex_array = buildVertexArray();
            auto index_buffer = buildIndexBuffer();
            vertex_array->setElementBuffer(index_buffer);
            stream = new IndexedStream(
                    getStreamType(),
                    std::move(name_mapping),
                    std::move(stream_type),
                    draw_mode,
                    usage_mode,
                    std::move(vertex_data),
                    data_kept,
                    vertex_array,
                    vertex_buffer,
                    std::move(indices_data),
                    index_buffer
            );
        }
            break;
        case VertexStream::Type::Skeletal: {

            auto skeletal_buffer = buildSkeletalBuffer();

            for (int i = 4; i < attributes.size(); ++i) {
                attributes.at(i).buffer = skeletal_buffer;
            }
            auto vertex_array = buildVertexArray();
            auto index_buffer = buildIndexBuffer();

            vertex_array->setElementBuffer(index_buffer);
            stream = new SkeletalStream(
                    getStreamType(),
                    std::move(name_mapping),
                    std::move(stream_type),
                    draw_mode,
                    usage_mode,
                    std::move(vertex_data),
                    data_kept,
                    vertex_array,
                    vertex_buffer,
                    std::move(indices_data),
                    index_buffer,
                    std::move(bone_weights),
                    skeletal_buffer
            );
        }
            break;
    }

    return std::shared_ptr<VertexStream>(stream);

    //
    // if (!batches.empty()) {
    //     for (int i = 0; i < batches.size() - 1; ++i) {
    //         if (batches[i]->getType() != batches[i + 1]->getType()) {
    //             throw std::runtime_error("Vertex stream types do not match");
    //         }
    //         if (batches[i]->getInputType() != batches[i + 1]->getInputType()) {
    //             throw std::runtime_error("Vertex stream input types do not match");
    //         }
    //     }
    //
    //     from(batches[0]);
    //
    //     if () {
    //
    //     }
    // }
}

VertexStream::Builder& VertexStream::Builder::from(const std::shared_ptr<VertexStream>& stream) {
    for (const auto& [index, att] : stream->getVertexArray()->getAttributes()) {
        auto [size, type, normalized, stride, pointer, buffer] = att;
        auto& mappings = stream->getNameMappings();
        attribute(index, Limitless::getDataType(type, size), mappings.at(index), stride, (size_t)pointer);
    }

    return *this;
}

VertexStream::Builder& VertexStream::Builder::usage(Usage usage) {
    usage_mode = usage;
    return *this;
}

VertexStream::Builder& VertexStream::Builder::draw(Draw draw) {
    draw_mode = draw;
    return *this;
}
