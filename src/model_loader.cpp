#include <model_loader.hpp>

#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <texture_loader.hpp>
#include <material_builder.hpp>
#include <assets.hpp>
#include <iostream>

using namespace GraphicsEngine;

namespace glm {
    glm::mat4 convert(const aiMatrix4x4& aimat) {
        return glm::mat4(aimat.a1, aimat.b1, aimat.c1, aimat.d1,
                         aimat.a2, aimat.b2, aimat.c2, aimat.d2,
                         aimat.a3, aimat.b3, aimat.c3, aimat.d3,
                         aimat.a4, aimat.b4, aimat.c4, aimat.d4);
    }

    glm::vec3 convert3f(const aiVector3D &aivec) {
        return glm::vec3(aivec.x, aivec.y, aivec.z);
    }

    glm::vec2 convert2f(const aiVector3D &aivec) {
        return glm::vec2(aivec.x, aivec.y);
    }
}

std::shared_ptr<AbstractModel> ModelLoader::loadModel(const fs::path& path) {
    if (assets.models.isExist(path.string())) {
        return assets.models.get(path.string());
    }

    std::vector<std::shared_ptr<AbstractMesh>> meshes;
    std::vector<std::shared_ptr<Material>> materials;
    std::unordered_map<std::string, uint32_t> bone_map;
    std::vector<Bone> bones;
    std::vector<Animation> animations;

    Assimp::Importer importer;
    const aiScene* scene;

    scene = importer.ReadFile(path.string().c_str(), aiProcess_ValidateDataStructure |
                                                     aiProcess_Triangulate |
                                                     aiProcess_GenUVCoords |
                                                     aiProcess_GenNormals |
                                                     aiProcess_GenSmoothNormals |
                                                     //aiProcess_FlipUVs |
                                                     aiProcess_CalcTangentSpace |
                                                     aiProcess_ImproveCacheLocality);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        throw std::runtime_error("Assimp library error: " + std::string(importer.GetErrorString()));
    }

    for (uint32_t i = 0; i < scene->mNumMeshes; ++i) {
        auto* mesh = scene->mMeshes[i];
        auto* material = scene->mMaterials[mesh->mMaterialIndex];

	    if (material->GetTextureCount(aiTextureType_DIFFUSE) == 0) {
	        //throw std::runtime_error("No diffuse textures in model " + path.string());
	        continue;
	    }

        std::shared_ptr<AbstractMesh> loaded_mesh;
        auto indices_count = mesh->mNumFaces * 3;
        // specifies template arguments based on INDICES_COUNT & TANGENTS & format
            // loads into packed vertices
        if (use_packed_format) {
            // T = VertexPackedNormalTangent, T1 = [GLubyte, GLushort, GLuint]
            if (indices_count < std::numeric_limits<GLubyte>::max()) {
                loaded_mesh = loadMesh<VertexPackedNormalTangent, GLubyte>(mesh, path, bones, bone_map);
            } else if (indices_count < std::numeric_limits<GLushort>::max()) {
                loaded_mesh = loadMesh<VertexPackedNormalTangent, GLushort>(mesh, path, bones, bone_map);
            } else {
                loaded_mesh = loadMesh<VertexPackedNormalTangent, GLuint>(mesh, path, bones, bone_map);
            }
        } else {
            // VertexNormalTangent, T1 = [GLubyte, GLushort, GLuint]
            if (indices_count < std::numeric_limits<GLubyte>::max()) {
                loaded_mesh = loadMesh<VertexNormalTangent, GLubyte>(mesh, path, bones, bone_map);
            } else if (indices_count < std::numeric_limits<GLushort>::max()) {
                loaded_mesh = loadMesh<VertexNormalTangent, GLushort>(mesh, path, bones, bone_map);
            } else {
                loaded_mesh = loadMesh<VertexNormalTangent, GLuint>(mesh, path, bones, bone_map);
            }
        }

        // adds mesh
        assets.meshes.add(loaded_mesh->getName(), loaded_mesh);
        meshes.emplace_back(loaded_mesh);

        // adds material
        auto loaded_material = loadMaterial(material, path);
        materials.emplace_back(loaded_material);
    }

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

    auto bone_finder = [&] (const std::string& str){
        auto bi = bone_map.find(str);
        if (bi != bone_map.end()) {
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
            Tree<uint32_t>& child = tree.add(bone_finder(node->mChildren[i]->mName.C_Str()));
            dfs(child, node->mChildren[i], depth + 1);
        }
    };

    dfs(tree, scene->mRootNode, 0);

    auto global_matrix = glm::convert(scene->mRootNode->mTransformation.Inverse());

    importer.FreeScene();

    if (!animations.empty()) {
        auto model = std::shared_ptr<AbstractModel>(new SkeletalModel(std::move(meshes), std::move(materials), std::move(bones), std::move(bone_map), std::move(tree), std::move(animations), global_matrix));
        assets.models.add(path.string(), model);

        return model;
    } else {
        auto model = std::shared_ptr<AbstractModel>(new Model(std::move(meshes), std::move(materials)));
        assets.models.add(path.string(), model);

        return model;
    }
}

template<typename T, typename T1>
std::shared_ptr<AbstractMesh> ModelLoader::loadMesh(aiMesh* mesh, const fs::path& path, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map)
{
    std::vector<T> vertices;
    vertices.reserve(mesh->mNumVertices);

    std::vector<T1> indices;
    indices.reserve(mesh->mNumFaces * 3);

    for (uint32_t j = 0; j < mesh->mNumVertices; ++j) {
        if constexpr (std::is_same<T, VertexNormalTangent>::value) {
            vertices.emplace_back(T{glm::convert3f(mesh->mVertices[j]),
                                    glm::convert3f(mesh->mNormals[j]),
                                    glm::convert3f(mesh->mTangents[j]),
                                    glm::convert2f(mesh->mTextureCoords[0][j])});
        }
        if constexpr (std::is_same<T, VertexPackedNormalTangent>::value) {
            auto uv = glm::packHalf2x16(glm::convert2f(mesh->mTextureCoords[0][j]));
            auto normals = pack(glm::convert3f(mesh->mNormals[j]));
            auto tangent = pack(glm::convert3f(mesh->mTangents[j]));
            vertices.emplace_back(T{glm::convert3f(mesh->mVertices[j]),
                                    normals,
                                    tangent,
                                    uv});
        }
    }

    for (uint32_t k = 0; k < mesh->mNumFaces; ++k) {
        auto face = mesh->mFaces[k];
        indices.emplace_back(face.mIndices[0]);
        indices.emplace_back(face.mIndices[1]);
        indices.emplace_back(face.mIndices[2]);
    }

    std::vector<VertexBoneWeight> bone_weights;
    if (mesh->HasBones()) {
        bone_weights.resize(vertices.size());

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

    if (!bone_map.empty() && (vertices.size() != bone_weights.size())) {
        throw std::runtime_error("Bone weights for some vertices does not exist, model is corrupted.");
    }

    auto name = path.string() + PATH_SEPARATOR + mesh->mName.C_Str();
    if (!bone_map.empty()) {
        return std::shared_ptr<AbstractMesh>(new SkinnedMesh<T, T1>(std::move(vertices), std::move(indices), std::move(bone_weights), name, MeshDataType::Static, DrawMode::Triangles));
    }

    return std::shared_ptr<AbstractMesh>(new IndexedMesh<T, T1>(std::move(vertices), std::move(indices), name, MeshDataType::Static, DrawMode::Triangles));
}

std::shared_ptr<Material> ModelLoader::loadMaterial(aiMaterial* mat, const fs::path& path) {
    aiString str, aname;
    mat->Get(AI_MATKEY_NAME, aname);
    std::string name = path.parent_path().string() + PATH_SEPARATOR + aname.C_Str();

    if (assets.materials.isExist(name)) {
        return assets.materials.get(name);
    }

    size_t diffuse_count = mat->GetTextureCount(aiTextureType_DIFFUSE);
    std::vector<std::shared_ptr<Texture>> diffuse;
    for (size_t i = 0; i < diffuse_count; ++i) {
        mat->GetTexture(aiTextureType_DIFFUSE, i, &str);
        diffuse.emplace_back(TextureLoader::load(path.parent_path().string() + PATH_SEPARATOR + str.C_Str()));
    }

    size_t normal_count = mat->GetTextureCount(aiTextureType_HEIGHT);
    std::vector<std::shared_ptr<Texture>> normal;
    for (size_t i = 0; i < normal_count; ++i) {
        mat->GetTexture(aiTextureType_HEIGHT, i, &str);
        normal.emplace_back(TextureLoader::load(path.parent_path().string() + PATH_SEPARATOR + str.C_Str()));
    }

    size_t specular_count = mat->GetTextureCount(aiTextureType_SPECULAR);
    std::vector<std::shared_ptr<Texture>> specular;
    for (size_t i = 0; i < specular_count; ++i) {
        mat->GetTexture(aiTextureType_SPECULAR, i, &str);
        specular.emplace_back(TextureLoader::load(path.parent_path().string() + PATH_SEPARATOR + str.C_Str()));
    }
/*
    size_t displacement_count = mat->GetTextureCount(aiTextureType_DISPLACEMENT);
    std::vector<std::shared_ptr<Texture>> displacement;
    for (size_t i = 0; i < displacement_count; ++i) {
        mat->GetTexture(aiTextureType_DISPLACEMENT, i, &str);
        displacement.emplace_back(TextureLoader::load(path.string() + PATH_SEPARATOR + str.C_Str()));
    }*/

    float shininess = 16.0f;
    mat->Get(AI_MATKEY_SHININESS, shininess);

    MaterialBuilder builder;

    builder.setShading(Shading::Lit);

    builder.add(PropertyType::Diffuse, diffuse[0]);

    if (!specular.empty()) {
        builder.add(PropertyType::Specular, specular[0]);
    }

    builder.add(PropertyType::Shininess, shininess);

    if (!normal.empty()) {
        builder.add(PropertyType::Normal, normal[0]);
    }

    return builder.build(name);
}
