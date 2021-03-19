#pragma once

#include <util/filesystem.hpp>
#include <shader_types.hpp>
#include <assimp/scene.h>
#include <util/tree.hpp>
#include <glm/glm.hpp>
#include <stdexcept>
#include <memory>

namespace glm {
    glm::vec3 convert3f(const aiVector3D &aivec) noexcept;
    glm::vec2 convert2f(const aiVector3D &aivec) noexcept;
    glm::mat4 convert(const aiMatrix4x4& aimat) noexcept;
}

namespace LimitlessEngine {
    class AbstractModel;
    class SkeletalModel;
    class AbstractMesh;
    class Material;

    struct VertexBoneWeight;
    struct Animation;
    struct Bone;

    struct model_loader_error : public std::runtime_error {
        explicit model_loader_error(const char* error) noexcept : runtime_error(error) {}
    };

    class ModelLoader {
    private:
        std::vector<std::shared_ptr<AbstractMesh>> loadMeshes(const aiScene* scene, const fs::path& path, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map);

        template<typename T, typename T1>
        std::shared_ptr<AbstractMesh> loadMesh(aiMesh* mesh, const fs::path& path, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map);
    protected:
        uint32_t unnamed_material_index {};
        uint32_t unnamed_mesh_index {};

        std::vector<VertexBoneWeight> loadBoneWeights(aiMesh* mesh, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map) const;
        std::vector<Animation> loadAnimations(const aiScene* scene, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map) const;
        Tree<uint32_t> loadAnimationTree(const aiScene* scene, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map) const;
        std::shared_ptr<Material> loadMaterial(aiMaterial* mat, const fs::path& path, const ModelShaders& model_shaders);
        std::vector<std::shared_ptr<Material>>
        loadMaterials(const aiScene* scene, const fs::path& path, ModelShader model_shader);

        template<typename T>
        std::vector<T> loadVertices(aiMesh* mesh) const noexcept;

        template<typename T>
        std::vector<T> loadIndices(aiMesh* mesh) const noexcept;
    public:
        std::shared_ptr<AbstractModel> loadModel(const fs::path& path, bool flip_uv = false);

        void loadAnimations(const fs::path& path, const SkeletalModel& model);
    };
}
