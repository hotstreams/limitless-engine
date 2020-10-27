#pragma once

#include <model_loader.hpp>

namespace GraphicsEngine {
    class ThreadedModelLoader : protected ModelLoader {
    private:
        std::function<std::vector<std::shared_ptr<AbstractMesh>>()> loadMeshes(const aiScene* scene, const fs::path& path, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map);

        template<typename T, typename T1>
        std::function<std::shared_ptr<AbstractMesh>()> loadMesh(aiMesh* mesh, const fs::path& path, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map);
    public:
        std::function<std::shared_ptr<AbstractModel>()> loadModel(const fs::path& path, bool flip_uv = false);
    };
}
