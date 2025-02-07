#pragma once

#include <limitless/core/vertex_stream/indexed_stream.hpp>
#include <limitless/models/mesh.hpp>
#include <limitless/models/batched_mesh.hpp>

namespace Limitless {
    class Mesh::Builder {
    private:
        std::vector<std::shared_ptr<Mesh>> meshes;

        std::string name_;
        std::shared_ptr<VertexStream> vertex_stream_;

        std::unordered_map<std::string, BatchedMesh::Description> getDescriptions();
    public:
        Builder& name(const std::string& name);
        Builder& vertex_stream(const std::shared_ptr<VertexStream>& vertex_stream);
        Builder& batch(const std::shared_ptr<Mesh>& mesh);

        std::shared_ptr<Mesh> build();
    };
}