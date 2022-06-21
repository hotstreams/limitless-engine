#include <limitless/loaders/threaded_model_loader.hpp>

#include <limitless/models/skeletal_model.hpp>
#include <limitless/core/skeletal_stream.hpp>
#include <limitless/assets.hpp>

#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <limitless/util/glm.hpp>
#include <limitless/models/mesh.hpp>

using namespace Limitless;

template<typename V, typename I>
std::function<std::shared_ptr<AbstractMesh>()> ThreadedModelLoader::loadMesh(
        Assets& assets,
        aiMesh* m,
        const fs::path& path,
        std::vector<Bone>& bones,
        std::unordered_map<std::string, uint32_t>& bone_map,
        const ModelLoaderFlags& flags)
{
    static auto i = 0;
    auto mesh_name = m->mName.length != 0 ? m->mName.C_Str() : std::to_string(i++);
    std::string name = path.string() + PATH_SEPARATOR + mesh_name;

    if (flags.isPresent(ModelLoaderOption::GenerateUniqueMeshNames)) {
        name += std::to_string(i++);
    }

    auto vertices = loadVertices<V>(m);
    auto indices = loadIndices<I>(m);
    auto weights = loadBoneWeights(m, bones, bone_map);

    // class reference variable assets will be dead by the moment of lambda invocation
    // so we need to store the original reference to assets
    return [&asset_ptr = assets, vertices = std::move(vertices), indices = std::move(indices), name = std::move(name), weights = std::move(weights), skinned = !bone_map.empty()] () mutable {
        if (asset_ptr.meshes.contains(name)) {
            return asset_ptr.meshes[name];
        }


        auto stream = !skinned ?
            std::make_unique<IndexedVertexStream<V>>(std::move(vertices), std::move(indices), VertexStreamUsage::Static, VertexStreamDraw::Triangles) :
            std::make_unique<SkinnedVertexStream<V>>(std::move(vertices), std::move(indices), std::move(weights), VertexStreamUsage::Static, VertexStreamDraw::Triangles);

        std::shared_ptr<AbstractMesh> mesh = std::make_shared<Mesh>(std::move(stream), std::move(name));

//        auto mesh = !skinned ?
//                    std::shared_ptr<AbstractMesh>(new IndexedMesh<V, I>(std::move(vertices), std::move(indices), std::move(name), MeshDataType::Static, DrawMode::Triangles)) :
//                    std::shared_ptr<AbstractMesh>(new SkinnedMesh<V, I>(std::move(vertices), std::move(indices), std::move(weights), std::move(name), MeshDataType::Static, DrawMode::Triangles));

        asset_ptr.meshes.add(mesh->getName(), mesh);

        return mesh;
    };
}

std::function<std::shared_ptr<AbstractModel>()> ThreadedModelLoader::loadModel(Assets& assets, const fs::path& _path, const ModelLoaderFlags& flags) {
	const auto path = convertPathSeparators(_path);

	Assimp::Importer importer;
	const aiScene* scene;

	auto scene_flags = aiProcess_ValidateDataStructure |
	                   aiProcess_Triangulate |
	                   aiProcess_GenUVCoords |
	                   aiProcess_GenNormals |
	                   aiProcess_CalcTangentSpace |
	                   aiProcess_ImproveCacheLocality;

	if (flags.isPresent(ModelLoaderOption::GlobalScale)) {
		scene_flags |= aiProcess_GlobalScale;
		importer.SetPropertyFloat(AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, flags.scale_factor);
	}

	if (flags.isPresent(ModelLoaderOption::FlipUV)) {
		scene_flags |= aiProcess_FlipUVs;
	}

	if (flags.isPresent(ModelLoaderOption::FlipWindingOrder)) {
		scene_flags |= aiProcess_FlipWindingOrder;
	}

    scene = importer.ReadFile(path.string().c_str(), scene_flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        throw model_loader_error(importer.GetErrorString());
    }

    std::unordered_map<std::string, uint32_t> bone_map;
    std::vector<Bone> bones;

    auto meshes = loadMeshes(assets, scene, path, bones, bone_map, flags);

    std::vector<std::shared_ptr<ms::Material>> materials;
    if (!flags.isPresent(ModelLoaderOption::NoMaterials)) {
        materials = loadMaterials(assets, scene, path, bone_map.empty() ? ModelShader::Model : ModelShader::Skeletal);
    }

    auto animations = loadAnimations(scene, bones, bone_map);
    auto animation_tree = loadAnimationTree(scene, bones, bone_map);
    auto global_matrix = convert(scene->mRootNode->mTransformation);

    importer.FreeScene();

    return [meshes = std::move(meshes), materials = std::move(materials), bones = std::move(bones), bone_map = std::move(bone_map), animations = std::move(animations), animation_tree = std::move(animation_tree), global_matrix, name = path.stem().string()] () mutable {
        return bone_map.empty() ?
               std::shared_ptr<AbstractModel>(new Model(meshes(), std::move(materials), name)) :
               std::shared_ptr<AbstractModel>(new SkeletalModel(meshes(), std::move(materials), std::move(bones), std::move(bone_map), std::move(animation_tree), std::move(animations), glm::inverse(global_matrix), name));
    };
}

std::function<std::vector<std::shared_ptr<AbstractMesh>>()> ThreadedModelLoader::loadMeshes(
        Assets& assets,
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
//        if (auto indices_count = mesh->mNumFaces * 3; indices_count < std::numeric_limits<GLubyte>::max()) {
//            future_mesh = loadMesh<VertexNormalTangent, GLubyte>(mesh, path, bones, bone_map, flags);
//        } else if (indices_count < std::numeric_limits<GLushort>::max()) {
//            future_mesh = loadMesh<VertexNormalTangent, GLushort>(mesh, path, bones, bone_map, flags);
//        } else {
            future_mesh = loadMesh<VertexNormalTangent, GLuint>(assets, mesh, path, bones, bone_map, flags);
//        }

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
