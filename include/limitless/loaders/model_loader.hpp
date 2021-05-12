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
        NoMaterials
    };

    using ModelLoaderFlags = std::set<ModelLoaderFlag>;

    class ModelLoader {
    private:
        std::vector<std::shared_ptr<AbstractMesh>> loadMeshes(const aiScene *scene, const fs::path& path, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map, const ModelLoaderFlags& flags);
        template<typename T, typename T1>
        std::shared_ptr<AbstractMesh> loadMesh(aiMesh *mesh, const fs::path& path, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map, const ModelLoaderFlags& flags);
    protected:
        uint32_t unnamed_material_index {};
        uint32_t unnamed_mesh_index {};

        Context& context;
        Assets& assets;
        const RenderSettings& settings;

        std::vector<VertexBoneWeight> loadBoneWeights(aiMesh* mesh, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map, const ModelLoaderFlags& flags) const;
        std::vector<Animation> loadAnimations(const aiScene* scene, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map, const ModelLoaderFlags& flags) const;
        Tree<uint32_t> loadAnimationTree(const aiScene* scene, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map, const ModelLoaderFlags& flags) const;
        std::shared_ptr<ms::Material> loadMaterial(aiMaterial* mat, const fs::path& path, const ModelShaders& model_shaders);
        std::vector<std::shared_ptr<ms::Material>> loadMaterials(const aiScene* scene, const fs::path& path, ModelShader model_shader);
        template<typename T> std::vector<T> loadVertices(aiMesh* mesh, const ModelLoaderFlags& flags) const noexcept;
        template<typename T> std::vector<T> loadIndices(aiMesh* mesh) const noexcept;
    public:
        ModelLoader(Context& context, Assets& assets, const RenderSettings& settings) noexcept;
        virtual ~ModelLoader() = default;

        std::shared_ptr<AbstractModel> loadModel(const fs::path& path, const ModelLoaderFlags& flags = {});

        void addAnimations(const fs::path& path, const std::shared_ptr<AbstractModel>& skeletal);
    };
}
