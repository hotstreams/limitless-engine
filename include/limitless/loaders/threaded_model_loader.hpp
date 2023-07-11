#pragma once

#include <limitless/loaders/model_loader.hpp>
#include <functional>

namespace Limitless {
    class ThreadedModelLoader : protected ModelLoader {
    private:
        static std::function<std::vector<std::shared_ptr<AbstractMesh>>()>
        loadMeshes(Assets& assets, const aiScene* scene, const fs::path& path, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map, const ModelLoaderFlags& flags);

        template<typename T, typename T1>
        static std::function<std::shared_ptr<AbstractMesh>()>
        loadMesh(Assets& assets, aiMesh* mesh, const fs::path& path, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map, const ModelLoaderFlags& flags);

        ThreadedModelLoader() = default;
        ~ThreadedModelLoader() override = default;
    public:
        static std::function<std::shared_ptr<AbstractModel>()> loadModel(Assets& assets, const fs::path& path, const ModelLoaderFlags& flags = {});
    };
}
