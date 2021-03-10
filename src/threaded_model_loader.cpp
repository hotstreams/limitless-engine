#include <threaded_model_loader.hpp>

#include <skeletal_model.hpp>
#include <assets.hpp>

#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

using namespace LimitlessEngine;

template<typename V, typename I>
std::function<std::shared_ptr<AbstractMesh>()> ThreadedModelLoader::loadMesh(aiMesh* m, const fs::path& path, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map) {
    auto mesh_name = m->mName.length != 0 ? m->mName.C_Str() : std::to_string(unnamed_mesh_index++);
    auto name = path.string() + PATH_SEPARATOR + mesh_name;

    auto vertices = loadVertices<V>(m);
    auto indices = loadIndices<I>(m);
    auto weights = loadBoneWeights(m, bones, bone_map);

    return [vertices = std::move(vertices), indices = std::move(indices), name = std::move(name), weights = std::move(weights), skinned = !bone_map.empty()] () mutable {
        if (assets.meshes.isExist(name)) {
            return assets.meshes[name];
        }

        auto mesh = !skinned ?
                    std::shared_ptr<AbstractMesh>(new IndexedMesh<V, I>(std::move(vertices), std::move(indices), std::move(name), MeshDataType::Static, DrawMode::Triangles)) :
                    std::shared_ptr<AbstractMesh>(new SkinnedMesh<V, I>(std::move(vertices), std::move(indices), std::move(weights), std::move(name), MeshDataType::Static, DrawMode::Triangles));

        assets.meshes.add(mesh->getName(), mesh);

        return mesh;
    };
}

std::function<std::shared_ptr<AbstractModel>()> ThreadedModelLoader::loadModel(const fs::path& path, bool flip_uv) {
    Assimp::Importer importer;
    const aiScene* scene;

    auto scene_flags = aiProcess_ValidateDataStructure |
                       aiProcess_Triangulate |
                       aiProcess_GenUVCoords |
                       aiProcess_GenNormals |
                       aiProcess_GenSmoothNormals |
                       aiProcess_CalcTangentSpace |
                       aiProcess_ImproveCacheLocality;
    if (flip_uv) {
        scene_flags |= aiProcess_FlipUVs;
    }

    scene = importer.ReadFile(path.string().c_str(), scene_flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        throw model_loader_error(importer.GetErrorString());
    }

    unnamed_mesh_index = 0;
    unnamed_material_index = 0;

    std::unordered_map<std::string, uint32_t> bone_map;
    std::vector<Bone> bones;

    auto meshes = loadMeshes(scene, path, bones, bone_map);

    auto materials = loadMaterials(scene, path, ModelShader::Instanced);

    auto animations = loadAnimations(scene, bones, bone_map);

    auto animation_tree = loadAnimationTree(scene, bones, bone_map);

    auto global_matrix = glm::convert(scene->mRootNode->mTransformation.Inverse());

    importer.FreeScene();

    return [meshes = std::move(meshes), materials = std::move(materials), bones = std::move(bones), bone_map = std::move(bone_map), animations = std::move(animations), animation_tree = std::move(animation_tree), global_matrix] () mutable {
        return animations.empty() ?
               std::shared_ptr<AbstractModel>(new Model(meshes(), std::move(materials))) :
               std::shared_ptr<AbstractModel>(new SkeletalModel(meshes(), std::move(materials), std::move(bones), std::move(bone_map), std::move(animation_tree), std::move(animations), global_matrix));
    };
}

std::function<std::vector<std::shared_ptr<AbstractMesh>>()> ThreadedModelLoader::loadMeshes(const aiScene* scene, const fs::path& path, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map) {
    std::vector<std::function<std::shared_ptr<AbstractMesh>()>> future_meshes;

    for (uint32_t i = 0; i < scene->mNumMeshes; ++i) {
        auto* mesh = scene->mMeshes[i];

        std::function<std::shared_ptr<AbstractMesh>()> future_mesh;
        if (auto indices_count = mesh->mNumFaces * 3; indices_count < std::numeric_limits<GLubyte>::max()) {
            future_mesh = loadMesh<VertexNormalTangent, GLubyte>(mesh, path, bones, bone_map);
        } else if (indices_count < std::numeric_limits<GLushort>::max()) {
            future_mesh = loadMesh<VertexNormalTangent, GLushort>(mesh, path, bones, bone_map);
        } else {
            future_mesh = loadMesh<VertexNormalTangent, GLuint>(mesh, path, bones, bone_map);
        }

        future_meshes.emplace_back(future_mesh);
    }

    return [future_meshes = std::move(future_meshes)] () {
        std::vector<std::shared_ptr<AbstractMesh>> meshes;
        meshes.reserve(future_meshes.size());

        for (auto& future_mesh : future_meshes) {
            meshes.emplace_back(future_mesh());
        }

        return meshes;
    };
}