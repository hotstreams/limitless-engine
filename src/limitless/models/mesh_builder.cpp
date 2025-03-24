#include <iostream>
#include <limitless/models/mesh_builder.hpp>

using namespace Limitless;

std::unordered_map<std::string, BatchedMesh::Description> Mesh::Builder::getDescriptions() {
    std::unordered_map<std::string, BatchedMesh::Description> descriptions;

    size_t offset = 0;
    for (auto& mesh: meshes) {
        auto indices_size = dynamic_cast<IndexedStream&>(*mesh->stream).getIndices().size();

        descriptions.emplace(mesh->getName(), BatchedMesh::Description(mesh->getName(), indices_size, offset));
        offset += indices_size;
    }

    return descriptions;
}

Mesh::Builder& Mesh::Builder::name(const std::string& name) {
    name_ = name;
    return *this;
}

Mesh::Builder& Mesh::Builder::vertex_stream(const std::shared_ptr<VertexStream>& vertex_stream) {
    vertex_stream_ = vertex_stream;
    return *this;
}

Mesh::Builder& Mesh::Builder::batch(const std::shared_ptr<Mesh>& mesh) {
    meshes.emplace_back(mesh);
    return *this;
}

Mesh::Builder& Mesh::Builder::lods(const std::vector<Mesh::LodData>& lods) {
    lods_ = lods;
    return *this;
}

Mesh::Builder& Mesh::Builder::add_lod(const Mesh::LodData& lod) {
    lods_.push_back(lod);
    return *this;
}

Mesh::Builder& Mesh::Builder::transition(Mesh::LodTransition transition) {
    transition_ = transition;
    return *this;
}

Mesh::Builder& Mesh::Builder::selection(Mesh::LodSelection selection) {
    selection_ = selection;
    return *this;
}

Mesh::Builder& Mesh::Builder::distances(const std::vector<float>& distances) {
    distances_ = distances;
    return *this;
}

std::shared_ptr<Mesh> Mesh::Builder::build() {
    if (name_.empty()) {
        throw std::runtime_error("Mesh name cannot be empty.");
    }

    if (!vertex_stream_ && meshes.empty()) {
        throw std::runtime_error("Vertex stream cannot be empty.");
    }

    // For batched mesh
    if (!meshes.empty()) {
        auto descriptions = getDescriptions();

        // return batched mesh
        auto mesh = meshes[0];
        for (int i = 1; i < meshes.size(); ++i) {
            mesh->stream->merge(meshes[i]->getVertexStream());
        }

        return std::shared_ptr<BatchedMesh>(new BatchedMesh(std::move(mesh->stream), "batched", std::move(descriptions)));
    }
    
    return std::shared_ptr<Mesh>(new Mesh(
        std::move(name_), 
        std::move(vertex_stream_), 
        std::move(lods_), 
        transition_, 
        selection_, 
        std::move(distances_)
    ));
}