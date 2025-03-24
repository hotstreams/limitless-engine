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
        
        std::vector<Mesh::LodData> lods_;
        Mesh::LodTransition transition_ = Mesh::LodTransition::None;
        Mesh::LodSelection selection_ = Mesh::LodSelection::CameraDistance;
        std::vector<float> distances_;
        
        std::unordered_map<std::string, BatchedMesh::Description> getDescriptions();
    public:
        Builder& name(const std::string& name);
        Builder& vertex_stream(const std::shared_ptr<VertexStream>& vertex_stream);
        Builder& batch(const std::shared_ptr<Mesh>& mesh);
        
        Builder& lods(const std::vector<Mesh::LodData>& lods);
        Builder& add_lod(const Mesh::LodData& lod);
        Builder& transition(Mesh::LodTransition transition);
        Builder& selection(Mesh::LodSelection selection);
        Builder& distances(const std::vector<float>& distances);

        std::shared_ptr<Mesh> build();
    };
}