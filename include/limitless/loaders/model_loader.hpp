#pragma once

#include <limitless/util/filesystem.hpp>
#include <limitless/pipeline/shader_pass_types.hpp>
#include <assimp/scene.h>
#include <limitless/util/tree.hpp>
#include <glm/glm.hpp>
#include <stdexcept>
#include <limitless/core/vertex.hpp>
#include <limitless/core/context_debug.hpp>
#include <memory>
#include <set>

namespace Limitless::ms {
    class Material;
}

namespace Limitless {
    class AbstractModel;
    class SkeletalModel;
    class AbstractMesh;
    class Assets;
    class Context;
    class RenderSettings;

    struct VertexBoneWeight;
    struct Animation;
    struct Bone;

    struct model_loader_error : public std::runtime_error {
        explicit model_loader_error(const char* error) noexcept : runtime_error(error) {}
    };

    enum class ModelLoaderFlag {
        FlipUV,
        GenerateUniqueMeshNames,
        FlipYZ,
        FlipWindingOrder,
        NoMaterials,

        // batches meshes based on material equality (not material type)
        NaiveBatch
    };

    using ModelLoaderFlags = std::set<ModelLoaderFlag>;

    class ModelLoader {
    private:
        static std::vector<std::shared_ptr<AbstractMesh>> loadMeshes(Assets& assets, const aiScene *scene, const fs::path& path, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map, const ModelLoaderFlags& flags);
        template<typename T, typename T1>
        static std::shared_ptr<AbstractMesh> loadMesh(Assets& assets, aiMesh *mesh, const fs::path& path, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map, const ModelLoaderFlags& flags);
    protected:
        static std::vector<VertexBoneWeight> loadBoneWeights(aiMesh* mesh, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map, const ModelLoaderFlags& flags);
        static std::vector<Animation> loadAnimations(const aiScene* scene, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map, const ModelLoaderFlags& flags);
        static Tree<uint32_t> loadAnimationTree(const aiScene* scene, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map, const ModelLoaderFlags& flags);
        static std::shared_ptr<ms::Material> loadMaterial(Assets& assets, aiMaterial* mat, const fs::path& path, const ModelShaders& model_shaders);
        static std::vector<std::shared_ptr<ms::Material>> loadMaterials(Assets& assets, const aiScene* scene, const fs::path& path, ModelShader model_shader);
        template<typename T> static std::vector<T> loadVertices(aiMesh* mesh, const ModelLoaderFlags& flags);
        template<typename T> static std::vector<T> loadIndices(aiMesh* mesh) noexcept;

        ModelLoader() = default;
        virtual ~ModelLoader() = default;
    public:
        static std::shared_ptr<AbstractModel> loadModel(Assets& assets, const fs::path& path, const ModelLoaderFlags& flags = {});
        static void addAnimations(const fs::path& path, const std::shared_ptr<AbstractModel>& skeletal);
    };
}
