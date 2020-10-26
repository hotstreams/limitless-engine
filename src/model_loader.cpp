#include <model_loader.hpp>

#include <material_system/material_builder.hpp>
#include <texture_loader.hpp>
#include <skeletal_model.hpp>
#include <assets.hpp>

#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

using namespace GraphicsEngine;

static uint32_t unnamed_mesh_index {0};
static uint32_t unnamed_material_index {0};

glm::mat4 glm::convert(const aiMatrix4x4& aimat) noexcept {
    return glm::mat4(aimat.a1, aimat.b1, aimat.c1, aimat.d1,
                     aimat.a2, aimat.b2, aimat.c2, aimat.d2,
                     aimat.a3, aimat.b3, aimat.c3, aimat.d3,
                     aimat.a4, aimat.b4, aimat.c4, aimat.d4);
}

glm::vec3 glm::convert3f(const aiVector3D &aivec) noexcept {
    return glm::vec3(aivec.x, aivec.y, aivec.z);
}

glm::vec2 glm::convert2f(const aiVector3D &aivec) noexcept {
    return glm::vec2(aivec.x, aivec.y);
}

std::shared_ptr<AbstractModel> ModelLoader::loadModel(const fs::path& path, bool flip_uv) {
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

    auto materials = loadMaterials(scene, path);

    auto animations = loadAnimations(scene, bones, bone_map);

    auto animation_tree = loadAnimationTree(scene, bones, bone_map);

    auto global_matrix = glm::convert(scene->mRootNode->mTransformation.Inverse());

    importer.FreeScene();

    auto model = animations.empty() ?
        std::shared_ptr<AbstractModel>(new Model(std::move(meshes), std::move(materials))) :
        std::shared_ptr<AbstractModel>(new SkeletalModel(std::move(meshes), std::move(materials), std::move(bones), std::move(bone_map), std::move(animation_tree), std::move(animations), global_matrix));

    return model;
}

template<typename T, typename T1>
std::shared_ptr<AbstractMesh> ModelLoader::loadMesh(aiMesh* m, const fs::path& path, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map) const {
    auto mesh_name = m->mName.length != 0 ? m->mName.C_Str() : std::to_string(unnamed_mesh_index++);
    auto name = path.string() + PATH_SEPARATOR + mesh_name;

    if (assets.meshes.isExist(name)) {
        return assets.meshes[name];
    }

    auto vertices = loadVertices<T>(m);
    auto indices = loadIndices<T1>(m);
    auto weights = loadBoneWeights(m, bones, bone_map);

    auto mesh = bone_map.empty() ?
        std::shared_ptr<AbstractMesh>(new IndexedMesh<T, T1>(std::move(vertices), std::move(indices), std::move(name), MeshDataType::Static, DrawMode::Triangles)) :
        std::shared_ptr<AbstractMesh>(new SkinnedMesh<T, T1>(std::move(vertices), std::move(indices), std::move(weights), std::move(name), MeshDataType::Static, DrawMode::Triangles));

    assets.meshes.add(mesh->getName(), mesh);

    return mesh;
}

std::shared_ptr<Material> ModelLoader::loadMaterial(aiMaterial* mat, const fs::path& path, const ModelShaders& model_shaders) const {
    aiString aname;
    mat->Get(AI_MATKEY_NAME, aname);

    auto path_str = path.parent_path().string();
    auto mat_name = aname.length != 0 ? aname.C_Str() : std::to_string(unnamed_material_index++);
    auto name = path_str + PATH_SEPARATOR + mat_name;

    if (assets.materials.isExist(name)) {
        return assets.materials[name];
    }

    MaterialBuilder builder;

    builder.create(std::move(name))
           .setShading(Shading::Lit);

    if (auto diffuse_count = mat->GetTextureCount(aiTextureType_DIFFUSE); diffuse_count != 0) {
        aiString texture_name;
        mat->GetTexture(aiTextureType_DIFFUSE, 0, &texture_name);

        builder.add(PropertyType::Diffuse, TextureLoader::load(path_str + PATH_SEPARATOR + texture_name.C_Str()));
    }

    if (auto normal_count = mat->GetTextureCount(aiTextureType_HEIGHT); normal_count != 0) {
        aiString texture_name;
        mat->GetTexture(aiTextureType_HEIGHT, 0, &texture_name);

        builder.add(PropertyType::Normal, TextureLoader::load(path_str + PATH_SEPARATOR + texture_name.C_Str()));
    }

    if (auto specular_count = mat->GetTextureCount(aiTextureType_SPECULAR); specular_count != 0) {
        aiString texture_name;
        mat->GetTexture(aiTextureType_SPECULAR, 0, &texture_name);

        builder.add(PropertyType::Specular, TextureLoader::load(path_str + PATH_SEPARATOR + texture_name.C_Str()));
    }

    float shininess = 16.0f;
    mat->Get(AI_MATKEY_SHININESS, shininess);
    builder.add(PropertyType::Shininess, shininess);

    if (auto opacity_mask = mat->GetTextureCount(aiTextureType_OPACITY); opacity_mask != 0) {
        aiString texture_name;
        mat->GetTexture(aiTextureType_OPACITY, 0, &texture_name);

        builder.add(PropertyType::BlendMask, TextureLoader::load(path_str + PATH_SEPARATOR + texture_name.C_Str()));
    }

    if (auto emissive_mask = mat->GetTextureCount(aiTextureType_EMISSIVE); emissive_mask != 0) {
        aiString texture_name;
        mat->GetTexture(aiTextureType_EMISSIVE, 0, &texture_name);

        builder.add(PropertyType::EmissiveMask, TextureLoader::load(path_str + PATH_SEPARATOR + texture_name.C_Str()));
    }

    {
        aiBlendMode blending;
        mat->Get(AI_MATKEY_BLEND_FUNC, blending);
        switch (blending) {
            case _aiBlendMode_Force32Bit:
            case aiBlendMode_Default:
                builder.setBlending(Blending::Opaque);
                break;
            case aiBlendMode_Additive:
                builder.setBlending(Blending::Additive);
                break;
        }
    }

    {
        aiColor3D color{0.0f};
        mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);

        float opacity{1.0f};
        mat->Get(AI_MATKEY_OPACITY, opacity);

        if (opacity != 1.0f) {
            builder.setBlending(Blending::Translucent);
        }

        if (color != aiColor3D{0.0f}) {
            builder.add(PropertyType::Color, glm::vec4{color.r, color.g, color.b, opacity});
        }
    }

    {
        aiColor3D color{0.0f};
        mat->Get(AI_MATKEY_COLOR_EMISSIVE, color);

        if (color != aiColor3D{0.0f}) {
            builder.add(PropertyType::EmissiveColor, glm::vec4{color.r, color.g, color.b, 1.0f});
            builder.setShading(Shading::Unlit);
        }
    }

    return builder.build(model_shaders);
}

template<typename T>
std::vector<T> ModelLoader::loadVertices(aiMesh* mesh) const noexcept {
    std::vector<T> vertices;
    vertices.reserve(mesh->mNumVertices);

    for (uint32_t j = 0; j < mesh->mNumVertices; ++j) {
        auto vertex = glm::convert3f(mesh->mVertices[j]);
        auto normal = glm::convert3f(mesh->mNormals[j]);
        auto tangent = glm::convert3f(mesh->mTangents[j]);
        auto uv = glm::convert2f(mesh->mTextureCoords[0][j]);

        if constexpr (std::is_same<T, VertexNormalTangent>::value) {
            vertices.emplace_back(T{vertex, normal, tangent, uv});
        }

        if constexpr (std::is_same<T, VertexPackedNormalTangent>::value) {
            auto packed_normal = pack(normal);
            auto packed_tangent = pack(tangent);
            auto packed_uv = glm::packHalf2x16(uv);

            vertices.emplace_back(T{vertex, packed_normal, packed_tangent, packed_uv});
        }
    }

    return vertices;
}

template<typename T>
std::vector<T> ModelLoader::loadIndices(aiMesh* mesh) const noexcept {
    std::vector<T> indices;
    indices.reserve(mesh->mNumFaces * 3);

    for (uint32_t k = 0; k < mesh->mNumFaces; ++k) {
        auto face = mesh->mFaces[k];
        indices.emplace_back(face.mIndices[0]);
        indices.emplace_back(face.mIndices[1]);
        indices.emplace_back(face.mIndices[2]);
    }

    return indices;
}

std::vector<VertexBoneWeight> ModelLoader::loadBoneWeights(aiMesh* mesh, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map) const {
    std::vector<VertexBoneWeight> bone_weights;
    if (mesh->HasBones()) {
        bone_weights.resize(mesh->mNumVertices);

        for (uint32_t j = 0; j < mesh->mNumBones; ++j) {
            std::string bone_name = mesh->mBones[j]->mName.C_Str();

            auto bi = bone_map.find(bone_name);
            auto& om = mesh->mBones[j]->mOffsetMatrix;
            auto offset_mat = glm::convert(om);

            uint32_t bone_index;
            if (bi == bone_map.end()) {
                bones.emplace_back(bone_name, offset_mat);
                bone_index = bones.size() - 1;
                bone_map.insert({bone_name, bone_index});
            } else {
                bone_index = bi->second;
            }

            for (uint32_t k = 0; k < mesh->mBones[j]->mNumWeights; ++k) {
                auto w = mesh->mBones[j]->mWeights[k];
                bone_weights.at(mesh->mBones[j]->mWeights[k].mVertexId).addBoneWeight(bone_index, w.mWeight);
            }
        }
    }

    if (!bone_map.empty() && (mesh->mNumVertices != bone_weights.size())) {
        throw model_loader_error("Bone weights for some vertices does not exist, model is corrupted.");
    }

    return bone_weights;
}

std::vector<std::shared_ptr<Material>> ModelLoader::loadMaterials(const aiScene* scene, const fs::path& path) const {
    std::vector<std::shared_ptr<Material>> materials;

    auto model_shader = scene->mNumAnimations != 0 ? ModelShader::Skeletal : ModelShader::Model;

    for (uint32_t i = 0; i < scene->mNumMeshes; ++i) {
        auto* mesh = scene->mMeshes[i];
        auto* material = scene->mMaterials[mesh->mMaterialIndex];
        materials.emplace_back(loadMaterial(material, path, {model_shader}));
    }

    return materials;
}

std::vector<Animation> ModelLoader::loadAnimations(const aiScene* scene, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map) const {
    std::vector<Animation> animations;
    for (unsigned int i = 0; i < scene->mNumAnimations; ++i) {
        auto* anim = scene->mAnimations[i];

        std::string anim_name(anim->mName.C_Str());
        std::vector<AnimationNode> anim_nodes;

        for (uint32_t j = 0; j < anim->mNumChannels; ++j) {
            auto* channel = anim->mChannels[j];

            auto bi = bone_map.find(channel->mNodeName.C_Str());
            if (bi == bone_map.end()) {
                bones.emplace_back(channel->mNodeName.C_Str(), glm::mat4(1.f));
                bone_map.emplace(channel->mNodeName.C_Str(), bones.size() - 1);
            }

            auto& bone = bones.at(bone_map.at(channel->mNodeName.C_Str()));
            std::vector<KeyFrame<glm::vec3>> pos_frames;
            std::vector<KeyFrame<glm::fquat>> rot_frames;
            std::vector<KeyFrame<glm::vec3>> scale_frames;

            for (uint32_t k = 0; k < channel->mNumPositionKeys; ++k) {
                auto& vec = channel->mPositionKeys[k].mValue;
                pos_frames.emplace_back(glm::vec3{vec.x, vec.y, vec.z}, channel->mPositionKeys[k].mTime);
            }

            for (uint32_t k = 0; k < channel->mNumRotationKeys; ++k) {
                auto& vec = channel->mRotationKeys[k].mValue;
                rot_frames.emplace_back(glm::fquat{vec.w, vec.x, vec.y, vec.z}, channel->mRotationKeys[k].mTime);
            }

            for (uint32_t k = 0; k < channel->mNumScalingKeys; ++k) {
                auto& vec = channel->mScalingKeys[k].mValue;
                scale_frames.emplace_back(glm::vec3{vec.x, vec.y, vec.z}, channel->mScalingKeys[k].mTime);
            }

            anim_nodes.emplace_back(pos_frames, rot_frames, scale_frames, bone);
        }

        animations.emplace_back(anim_name, anim->mDuration, anim->mTicksPerSecond > 0 ? anim->mTicksPerSecond : 25, anim_nodes);
    }

    return animations;
}

Tree<uint32_t> ModelLoader::loadAnimationTree(const aiScene* scene, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map) const {
    auto bone_finder = [&] (const std::string& str){
        if (auto bi = bone_map.find(str); bi != bone_map.end()) {
            return bi->second;
        } else {
            bones.emplace_back(str, glm::mat4(1.f));
            bone_map.emplace(str, bones.size() - 1);
            return static_cast<uint32_t>(bones.size() - 1);
        }
    };

    Tree<uint32_t> tree(bone_finder(scene->mRootNode->mName.C_Str()));

    std::function<void(Tree<uint32_t>& tree, const aiNode*, int)> dfs;
    dfs = [&] (Tree<uint32_t>& tree, const aiNode* node, int depth) {
        bones[*tree].node_transform = glm::convert(node->mTransformation);

        for (uint32_t i = 0; i < node->mNumChildren; ++i) {
            auto& child = tree.add(bone_finder(node->mChildren[i]->mName.C_Str()));
            dfs(child, node->mChildren[i], depth + 1);
        }
    };

    dfs(tree, scene->mRootNode, 0);

    return tree;
}

std::vector<std::shared_ptr<AbstractMesh>> ModelLoader::loadMeshes(const aiScene* scene, const fs::path& path, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map) const {
    std::vector<std::shared_ptr<AbstractMesh>> meshes;

    for (uint32_t i = 0; i < scene->mNumMeshes; ++i) {
        auto* mesh = scene->mMeshes[i];

        std::shared_ptr<AbstractMesh> loaded_mesh;
        // specifies template arguments based on INDICES_COUNT & TANGENTS & format
        if (auto indices_count = mesh->mNumFaces * 3; indices_count < std::numeric_limits<GLubyte>::max()) {
            loaded_mesh = loadMesh<VertexNormalTangent, GLubyte>(mesh, path, bones, bone_map);
        } else if (indices_count < std::numeric_limits<GLushort>::max()) {
            loaded_mesh = loadMesh<VertexNormalTangent, GLushort>(mesh, path, bones, bone_map);
        } else {
            loaded_mesh = loadMesh<VertexNormalTangent, GLuint>(mesh, path, bones, bone_map);
        }

        meshes.emplace_back(loaded_mesh);
    }

    return meshes;
}
