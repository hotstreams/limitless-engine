#pragma once

#include <limitless/loaders/model_loader.hpp>

namespace Limitless {
    class ThreadedModelLoader : protected ModelLoader {
    private:
        std::function<std::vector<std::shared_ptr<AbstractMesh>>()>
        loadMeshes(const aiScene* scene, const fs::path& path, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map, const ModelLoaderFlags& flags);

        template<typename T, typename T1>
        std::function<std::shared_ptr<AbstractMesh>()>
        loadMesh(aiMesh* mesh, const fs::path& path, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map, const ModelLoaderFlags& flags);
    public:
        ThreadedModelLoader(Context& context, Assets& assets) noexcept : ModelLoader {context, assets} {}

        std::function<std::shared_ptr<AbstractModel>()> loadModel(const fs::path& path, const ModelLoaderFlags& flags = {});
    };
}
