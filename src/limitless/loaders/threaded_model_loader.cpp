#include <limitless/loaders/threaded_model_loader.hpp>

#include <limitless/models/skeletal_model.hpp>
#include <limitless/assets.hpp>

#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <limitless/util/glm.hpp>

using namespace Limitless;

template<typename V, typename I>
std::function<std::shared_ptr<AbstractMesh>()> ThreadedModelLoader::loadMesh(
        aiMesh* m, const fs::path& path,
        std::vector<Bone>& bones,
        std::unordered_map<std::string, uint32_t>& bone_map,
        const ModelLoaderFlags& flags)
{
    auto mesh_name = m->mName.length != 0 ? m->mName.C_Str() : std::to_string(unnamed_mesh_index++);
    std::string name = path.string() + PATH_SEPARATOR + mesh_name;

    if (flags.find(ModelLoaderFlag::GenerateUniqueMeshNames) != flags.end()) {
        name += std::to_string(unnamed_mesh_index++);
    }

    auto vertices = loadVertices<V>(m, ModelLoaderFlags{});
    auto indices = loadIndices<I>(m);
    auto weights = loadBoneWeights(m, bones, bone_map, ModelLoaderFlags{});

    // class reference variable assets will be dead by the moment of lambda invocation
    // so we need to store the original reference to assets
    return [&asset_ptr = assets, vertices = std::move(vertices), indices = std::move(indices), name = std::move(name), weights = std::move(weights), skinned = !bone_map.empty()] () mutable {
        if (asset_ptr.meshes.exists(name)) {
            return asset_ptr.meshes[name];
        }

        auto mesh = !skinned ?
                    std::shared_ptr<AbstractMesh>(new IndexedMesh<V, I>(std::move(vertices), std::move(indices), std::move(name), MeshDataType::Static, DrawMode::Triangles)) :
                    std::shared_ptr<AbstractMesh>(new SkinnedMesh<V, I>(std::move(vertices), std::move(indices), std::move(weights), std::move(name), MeshDataType::Static, DrawMode::Triangles));

        asset_ptr.meshes.add(mesh->getName(), mesh);

        return mesh;
    };
}

std::function<std::shared_ptr<AbstractModel>()> ThreadedModelLoader::loadModel(const fs::path& _path, const ModelLoaderFlags& flags) {
    auto path = convertPathSeparators(_path);
    Assimp::Importer importer;
    const aiScene* scene;

    auto scene_flags = aiProcess_ValidateDataStructure |
                       aiProcess_Triangulate |
                       aiProcess_GenUVCoords |
                       aiProcess_GenNormals |
                       aiProcess_GenSmoothNormals |
                       aiProcess_CalcTangentSpace |
                       aiProcess_ImproveCacheLocality;

    if (flags.find(ModelLoaderFlag::FlipUV) != flags.end()) {
        scene_flags |= aiProcess_FlipUVs;
    }

    if (flags.find(ModelLoaderFlag::FlipWindingOrder) != flags.end()) {
        scene_flags |= aiProcess_FlipWindingOrder;
    }

    scene = importer.ReadFile(path.string().c_str(), scene_flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        throw model_loader_error(importer.GetErrorString());
    }

    unnamed_mesh_index = 0;
    unnamed_material_index = 0;

    std::unordered_map<std::string, uint32_t> bone_map;
    std::vector<Bone> bones;

    auto meshes = loadMeshes(scene, path, bones, bone_map, flags);

    std::vector<std::shared_ptr<ms::Material>> materials;
    if (!flags.count(ModelLoaderFlag::NoMaterials)) {
        materials = loadMaterials(scene, path, bone_map.empty() ? ModelShader::Model : ModelShader::Skeletal);
    }

    auto animations = loadAnimations(scene, bones, bone_map, flags);

    auto animation_tree = loadAnimationTree(scene, bones, bone_map, flags);

    auto global_matrix = convert(scene->mRootNode->mTransformation);

    if (flags.find(ModelLoaderFlag::FlipYZ) != flags.end()) {
        global_matrix = flipYZ(global_matrix);
    }

    importer.FreeScene();

    return [meshes = std::move(meshes), materials = std::move(materials), bones = std::move(bones), bone_map = std::move(bone_map), animations = std::move(animations), animation_tree = std::move(animation_tree), global_matrix] () mutable {
        return animations.empty() ?
               std::shared_ptr<AbstractModel>(new Model(meshes(), std::move(materials))) :
               std::shared_ptr<AbstractModel>(new SkeletalModel(meshes(), std::move(materials), std::move(bones), std::move(bone_map), std::move(animation_tree), std::move(animations), glm::inverse(global_matrix)));
    };
}

std::function<std::vector<std::shared_ptr<AbstractMesh>>()> ThreadedModelLoader::loadMeshes(
        const aiScene* scene,
        const fs::path& path,
        std::vector<Bone>& bones,
        std::unordered_map<std::string, uint32_t>& bone_map,
        const ModelLoaderFlags& flags)
{
    std::vector<std::function<std::shared_ptr<AbstractMesh>()>> future_meshes;

    for (uint32_t i = 0; i < scene->mNumMeshes; ++i) {
        auto* mesh = scene->mMeshes[i];

        std::function<std::shared_ptr<AbstractMesh>()> future_mesh;
        if (auto indices_count = mesh->mNumFaces * 3; indices_count < std::numeric_limits<GLubyte>::max()) {
            future_mesh = loadMesh<VertexNormalTangent, GLubyte>(mesh, path, bones, bone_map, flags);
        } else if (indices_count < std::numeric_limits<GLushort>::max()) {
            future_mesh = loadMesh<VertexNormalTangent, GLushort>(mesh, path, bones, bone_map, flags);
        } else {
            future_mesh = loadMesh<VertexNormalTangent, GLuint>(mesh, path, bones, bone_map, flags);
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
