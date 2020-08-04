#pragma once

#include <skeletal_model.hpp>
#include <filesystem.hpp>
#include <assimp/material.h>
#include <assimp/mesh.h>
#include <shader_storage.hpp>

namespace GraphicsEngine {
    class ModelLoader {
    private:
        static inline bool use_packed_format {false};

        template<typename T, typename T1>
        static std::shared_ptr<AbstractMesh> loadMesh(aiMesh* mesh, const fs::path& path, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map);
        static std::shared_ptr<Material> loadMaterial(aiMaterial* mat, const fs::path& path, const RequiredModelShaders& model_shaders);
    public:
        static std::shared_ptr<AbstractModel> loadModel(const fs::path& path, bool flip_uv = false);
    };
}
