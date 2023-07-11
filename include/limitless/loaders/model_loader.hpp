#pragma once

#include <limitless/util/filesystem.hpp>
#include <limitless/pipeline/shader_type.hpp>
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

    enum class ModelLoaderOption {
        FlipUV,
        GenerateUniqueMeshNames,
        FlipWindingOrder,
        NoMaterials,
		GlobalScale
    };

    class ModelLoaderFlags {
    public:
	    std::set<ModelLoaderOption> options;
	    float scale_factor {1.0f};

	    auto isPresent(ModelLoaderOption option) const { return options.count(option) != 0; }
    };

    class ModelLoader {
    private:
        static std::vector<std::shared_ptr<AbstractMesh>> loadMeshes(Assets& assets, const aiScene *scene, const fs::path& path, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map, const ModelLoaderFlags& flags);
        template<typename T, typename T1>
        static std::shared_ptr<AbstractMesh> loadMesh(Assets& assets, aiMesh *mesh, const fs::path& path, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map, const ModelLoaderFlags& flags);
    protected:
        static std::vector<VertexBoneWeight> loadBoneWeights(aiMesh* mesh, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map);
        static std::vector<Animation> loadAnimations(const aiScene* scene, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map);
        static Tree<uint32_t> loadAnimationTree(const aiScene* scene, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map);
        static std::shared_ptr<ms::Material> loadMaterial(Assets& assets, aiMaterial* mat, const fs::path& path, const InstanceTypes& model_shaders);
        static std::vector<std::shared_ptr<ms::Material>> loadMaterials(Assets& assets, const aiScene* scene, const fs::path& path, InstanceType model_shader);
        template<typename T> static std::vector<T> loadVertices(aiMesh* mesh);
        template<typename T> static std::vector<T> loadIndices(aiMesh* mesh) noexcept;

        ModelLoader() = default;
        virtual ~ModelLoader() = default;
    public:
        static std::shared_ptr<AbstractModel> loadModel(Assets& assets, const fs::path& path, const ModelLoaderFlags& flags = {});
        static void addAnimations(const fs::path& path, const std::shared_ptr<AbstractModel>& skeletal, const ModelLoaderFlags& flags = {});
        static void addAnimations(const std::vector<fs::path>& paths, const std::shared_ptr<AbstractModel>& skeletal, const ModelLoaderFlags& flags = {});
    };
}
