#pragma once

#include <limitless/models/indexed_mesh.hpp>

namespace Limitless {
    struct Bone {
        std::string name;
        glm::mat4 node_transform {1.0f};
        glm::mat4 offset_matrix;

        Bone(std::string name, const glm::mat4& offset_matrix) noexcept : name{std::move(name)}, offset_matrix{offset_matrix} {}

        [[nodiscard]] auto isFake() const noexcept { return name[0] == '<'; }
    };

    struct VertexBoneWeight {
        static constexpr uint32_t BONE_COUNT = 4;

        std::array<uint32_t, BONE_COUNT> bone_index;
        std::array<float, BONE_COUNT> weight;

        void addBoneWeight(uint32_t id, float w) noexcept {
            uint32_t i;
            for (i = 0; i < BONE_COUNT && weight[i] != 0.0f; ++i);

            if (i >= BONE_COUNT) {/* no more weight slots */ return; }

            bone_index[i] = id;
            weight[i] = w;
        }
    };

    template<typename T, typename T1>
    class SkinnedMesh : public IndexedMesh<T, T1> {
    private:
        std::vector<VertexBoneWeight> bone_weights;
        std::unique_ptr<Buffer> bone_buffer;

        void initialize() {
            BufferBuilder builder;
            bone_buffer = builder.setTarget(Buffer::Type::Array)
                                 .setUsage(Buffer::Storage::Static)
                                 .setAccess(Buffer::ImmutableAccess::None)
                                 .setData(bone_weights.data())
                                 .setDataSize(bone_weights.size() * sizeof(VertexBoneWeight))
                                 .build();

            this->vertex_array << VertexAttribute{VertexBoneWeight::BONE_COUNT, GL_INT, GL_FALSE, sizeof(VertexBoneWeight), (GLvoid*)offsetof(VertexBoneWeight, bone_index), *bone_buffer}
                               << VertexAttribute{VertexBoneWeight::BONE_COUNT, GL_FLOAT, GL_FALSE, sizeof(VertexBoneWeight), (GLvoid*)offsetof(VertexBoneWeight, weight), *bone_buffer};
        };
    public:
        SkinnedMesh(std::vector<T>&& vertices, std::vector<T1>&& indices, std::vector<VertexBoneWeight>&& bones, std::string material, MeshDataType data_type, DrawMode draw_mode)
            : IndexedMesh<T, T1>{std::move(vertices), std::move(indices), std::move(material), data_type, draw_mode}, bone_weights{std::move(bones)} {
            initialize();
        }

        ~SkinnedMesh() override = default;

        SkinnedMesh(const SkinnedMesh&) noexcept = delete;
        SkinnedMesh& operator=(const SkinnedMesh&) noexcept = delete;

        SkinnedMesh(SkinnedMesh&&) noexcept = default;
        SkinnedMesh& operator=(SkinnedMesh&&) noexcept = default;
    };
}