#pragma once

#include <limitless/fx/modules/module.hpp>
#include <limitless/models/indexed_mesh.hpp>
#include <limitless/instances/skeletal_instance.hpp>

namespace Limitless::fx {
    template<typename Particle>
    class InitialMeshLocation : public Module<Particle> {
    protected:
        std::variant<std::shared_ptr<AbstractMesh>, std::shared_ptr<AbstractModel>> mesh;
        std::default_random_engine generator;

        ModelInstance* instance {};

        glm::vec3 getPositionOnTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, float r1, float r2) noexcept {
            r1 = glm::sqrt(r1);
            const auto m1 = 1.0f - r1;
            const auto m2 = r1 * (1.0f - r2);
            const auto m3 = r2 * r1;

            return (m1 * a) + (m2 * b) + (m3 * c);
        }

        glm::vec3 getPositionOnMesh(const std::shared_ptr<AbstractMesh>& _mesh, size_t vertex_index, float r1, float r2) {
            const auto& indexed_mesh = dynamic_cast<IndexedMesh<VertexNormalTangent, GLuint>&>(*_mesh);
            const auto& vertices = indexed_mesh.getVertices();
            const auto& indices = indexed_mesh.getIndices();

            const auto v_index1 = indices[vertex_index];
            const auto v_index2 = indices[vertex_index + 1];
            const auto v_index3 = indices[vertex_index + 2];

            if (instance) {
                if (instance->getShaderType() == ModelShader::Skeletal) {
                    const auto& skeletal_instance = static_cast<SkeletalInstance&>(*instance);
                    const auto pos1 = skeletal_instance.getSkinnedVertexPosition(_mesh, v_index1);
                    const auto pos2 = skeletal_instance.getSkinnedVertexPosition(_mesh, v_index2);
                    const auto pos3 = skeletal_instance.getSkinnedVertexPosition(_mesh, v_index3);

                    return getPositionOnTriangle(pos1, pos2, pos3, r1, r2);
                }

                return instance->getModelMatrix() * glm::vec4(getPositionOnTriangle(vertices[v_index1].position,
                                                                                    vertices[v_index2].position,
                                                                                    vertices[v_index3].position,
                                                                                    r1, r2), 1.0f);
            }

            return getPositionOnTriangle(vertices[v_index1].position,
                                         vertices[v_index2].position,
                                         vertices[v_index3].position,
                                         r1, r2);
        }

        InitialMeshLocation(ModuleType type, std::shared_ptr<AbstractMesh> _mesh) noexcept
            : Module<Particle>(type)
            , mesh {std::move(_mesh)}
            , generator {std::random_device()()} {
        }

        InitialMeshLocation(ModuleType type, std::shared_ptr<AbstractModel> _mesh) noexcept
            : Module<Particle>(type)
            , mesh {std::move(_mesh)}
            , generator {std::random_device()()} {
        }

        auto getSelectedMesh() {
            std::shared_ptr<AbstractMesh> selected_mesh;
            if (std::holds_alternative<std::shared_ptr<AbstractMesh>>(mesh)) {
                selected_mesh = std::get<std::shared_ptr<AbstractMesh>>(mesh);
            } else {
                const auto& model = std::get<std::shared_ptr<AbstractModel>>(mesh);
                const auto& meshes = model->getMeshes();

                using vector_size_type = typename std::remove_reference_t<decltype(meshes)>::size_type;
                auto int_distribution = std::uniform_int_distribution(static_cast<vector_size_type>(0), meshes.size() - 1);
                const auto mesh_index = int_distribution(generator);

                selected_mesh = meshes[mesh_index];
            }

            return selected_mesh;
        }

        auto getVertexIndex(const std::shared_ptr<AbstractMesh>& selected_mesh) {
            const auto& indexed_mesh = dynamic_cast<IndexedMesh<VertexNormalTangent, GLuint>&>(*selected_mesh);
            const auto& indices = indexed_mesh.getIndices();
            using vector_size_type = typename std::remove_reference_t<decltype(indices)>::size_type;
            auto int_distribution = std::uniform_int_distribution(static_cast<vector_size_type>(0), indices.size() - 4);
            return int_distribution(generator);
        }

        auto getTrianglePosition() {
            auto triangle_distribution = std::uniform_real_distribution<float>(0.0f, 1.0f);
            const auto r1 = glm::sqrt(triangle_distribution(generator));
            const auto r2 = triangle_distribution(generator);
            return std::pair{r1, r2};
        }
    public:
        explicit InitialMeshLocation(std::shared_ptr<AbstractMesh> _mesh) noexcept
            : Module<Particle>(ModuleType::InitialMeshLocation)
            , mesh {std::move(_mesh)}
            , generator {std::random_device()()} {
        }

        explicit InitialMeshLocation(std::shared_ptr<AbstractModel> _mesh) noexcept
            : Module<Particle>(ModuleType::InitialMeshLocation)
            , mesh {std::move(_mesh)}
            , generator {std::random_device()()} {
        }

        ~InitialMeshLocation() override = default;

        InitialMeshLocation(const InitialMeshLocation&) = default;
        InitialMeshLocation& operator=(const InitialMeshLocation&) noexcept = default;

        void attachModelInstance(ModelInstance* _instance) noexcept {
            instance = _instance;
        }

        auto& getMesh() noexcept { return mesh; }
        const auto& getMesh() const noexcept { return mesh; }

        void initialize([[maybe_unused]] AbstractEmitter& emitter, Particle& particle, [[maybe_unused]] size_t index) noexcept override {
            const auto selected_mesh = getSelectedMesh();
            const auto vertex_index = getVertexIndex(selected_mesh);
            const auto triangle_pos = getTrianglePosition();
            particle.getPosition() += getPositionOnMesh(selected_mesh, vertex_index, triangle_pos.first, triangle_pos.second);
        }

        [[nodiscard]] InitialMeshLocation* clone() const noexcept override {
            return new InitialMeshLocation(*this);
        }
    };
}