#include "cgltf.h"
#include <meshoptimizer.h>

#include <chrono>
#include <cstdio>
#include <iostream>
#include <limitless/assets.hpp>
#include <limitless/camera.hpp>
#include <limitless/core/context.hpp>
#include <limitless/core/vertex_stream/vertex_stream_builder.hpp>
#include <limitless/core/vertex_stream/indexed_stream.hpp>
#include <limitless/models/mesh_builder.hpp>
#include <limitless/core/vertex.hpp>
#include <limitless/instances/model_instance.hpp>
#include <limitless/instances/skeletal_instance.hpp>
#include <limitless/loaders/gltf_model_loader.hpp>
#include <limitless/models/model.hpp>
#include <limitless/models/bones.hpp>
#include <limitless/models/line.hpp>
#include <limitless/models/mesh.hpp>
#include <limitless/models/model_builder.h>
#include <limitless/models/skeletal_model.hpp>
#include <limitless/ms/property.hpp>
#include <limitless/renderer/shader_type.hpp>
#include <limitless/renderer/renderer.hpp>
#include <limitless/renderer/renderer_settings.hpp>
#include <limitless/scene.hpp>
#include <limitless/util/filesystem.hpp>
#include <memory>
#include <cstring>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <limits>
#include <optional>
#include <unordered_map>
#include <atomic>
#include <tuple>

namespace Limitless {

namespace {

// KHR_materials_variants + SpeedTree helpers: GltfModelLoader::Extensions (gltf_extensions.cpp).

size_t getTargetIndexCount(size_t index_count, const LodTarget& target) {
	size_t result = std::visit([&](auto&& arg) -> size_t {
		using T = std::decay_t<decltype(arg)>;
		if constexpr (std::is_same_v<T, LodSimplificationFactor>) {
			return static_cast<size_t>(index_count * arg);
		} else if constexpr (std::is_same_v<T, LodTargetIndicesCount>) {
			return arg;
		}
		throw ModelLoadError {"invalid lod target type"};
	}, target);

	// Ensure target is aligned to triangles (multiple of 3) and at least 3.
	result = std::max(result - result % 3, size_t {3});

	return result;
}

} // namespace

// ---------------------------------------------------------------------------
// Assemble plain / skeletal Model from parsed cgltf_data
// ---------------------------------------------------------------------------

std::shared_ptr<Model> loadSkeletalModel(
	const GltfModelLoader& owner,
	Assets& assets,
	const cgltf_data& src,
	const std::string& model_name)
{
	const ModelLoaderFlags& flags = owner.options().flags;
	std::vector<Bone> bones;
	std::unordered_map<const cgltf_node*, size_t> bone_indice_map;

	auto root_nodes = GltfModelLoader::SkinAnimation::findRootNodes(src);

	for (size_t i = 0; i < src.nodes_count; ++i) {
		const cgltf_node& node = src.nodes[i];
		auto bone_name =
			node.name ? std::string(node.name) : model_name + "_bone" + std::to_string(i);
		bones.emplace_back(Bone(i, bone_name, GltfModelLoader::AccessorIO::getNodeMatrix(node), glm::mat4(1.f)));
		bone_indice_map.emplace(&node, i);

		auto& bone = bones.back();
		if (node.has_translation) {
			bone.position = GltfModelLoader::AccessorIO::toVec3(node.translation);
		}
		if (node.has_rotation) {
			bone.rotation = GltfModelLoader::AccessorIO::toQuat(node.rotation);
		}
		if (node.has_scale) {
			bone.scale = GltfModelLoader::AccessorIO::toVec3(node.scale);
		}
	}

	// we do this last to avoid ptrs being invalidated if bones vector
	// reallocates.
	std::unordered_map<const cgltf_node*, Bone*> bone_map;

	for (const auto& [node, bone_index] : bone_indice_map) {
		bone_map.emplace(node, &bones[bone_index]);
	}

	for (size_t i = 0; i < src.skins_count; ++i) {
		const cgltf_skin& skin = src.skins[i];
		// Accessors of matrix type have data stored in column-major order.
		// glm stores them in column-major order.
		auto inverse_bind_matrices =
			GltfModelLoader::AccessorIO::copyFromAccessor<std::array<float, 16>>(*skin.inverse_bind_matrices);

		if (inverse_bind_matrices.size() != skin.joints_count) {
			throw ModelLoadError {"mismatched number of skin joints and inverse bind matrices"};
		}

		for (size_t j = 0; j < skin.joints_count; ++j) {
			const cgltf_node& joint_node = *skin.joints[j];
			auto* bone                   = bone_map.at(&joint_node);
			bone->offset_matrix          = GltfModelLoader::AccessorIO::toMat4(inverse_bind_matrices[j]);
			bone->joint_index            = j;
		}
	}

	std::vector<Animation> animations;
	for (size_t i = 0; i < src.animations_count; ++i) {
		auto anim_name = src.animations[i].name ? std::string(src.animations[i].name)
		                                        : "anim" + std::to_string(i);
		animations.emplace_back(
			GltfModelLoader::SkinAnimation::loadAnimation(std::move(anim_name), src.animations[i], bone_map));
	}

	auto bone_indices_tree = GltfModelLoader::SkinAnimation::makeBoneIndiceTrees(root_nodes, bone_map);

	InstanceTypes instance_types = flags.additional_instance_types;
	instance_types.emplace(InstanceType::Skeletal);
	auto loaded_materials = GltfModelLoader::Materials::loadMaterials(owner, model_name, instance_types);

	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<std::shared_ptr<ms::Material>> mesh_materials;
	std::vector<std::vector<std::shared_ptr<ms::Material>>> mesh_variant_rows;

	for (size_t i = 0; i < src.nodes_count; ++i) {
		const cgltf_node& node = src.nodes[i];

		if (node.mesh) {
			auto [more_meshes, more_mesh_materials, more_var_rows] =
				GltfModelLoader::VertexGeometry::loadMeshes(
				node, *node.mesh, node.skin, model_name, meshes.size(), loaded_materials, src, flags
			);
			meshes.insert(meshes.end(), more_meshes.begin(), more_meshes.end());
			mesh_materials.insert(
				mesh_materials.end(), more_mesh_materials.begin(), more_mesh_materials.end()
			);
			mesh_variant_rows.insert(mesh_variant_rows.end(), more_var_rows.begin(), more_var_rows.end());
		}
	}

	GltfModelLoader::Materials::fixMissingMaterials(mesh_materials, owner, model_name, instance_types);

	std::unordered_map<std::string, uint32_t> bone_indices_map;
	for (size_t i = 0; i < bones.size(); ++i) {
		bone_indices_map.emplace(bones[i].name, i);
	}

	auto variant_set = GltfModelLoader::Extensions::makeMaterialVariantSet(src, meshes, mesh_variant_rows, flags);

	auto builder = Model::builder()
		.name(model_name)
		.meshes(meshes)
		.materials(mesh_materials)
		.bones(std::move(bones))
		.bone_map(std::move(bone_indices_map))
		.skeletons(std::move(bone_indices_tree))
		.animations(std::move(animations));
	if (variant_set) {
		builder.materialVariantSet(std::move(variant_set));
	}
	if (owner.options().billboard_bundle) {
		builder.billboardLodBundle(owner.options().billboard_bundle);
	}
	return builder.build(assets);
}

std::shared_ptr<Model> loadPlainModel(
	const GltfModelLoader& owner,
	Assets& assets,
	const cgltf_data& src,
	const std::string& model_name)
{
	const ModelLoaderFlags& flags = owner.options().flags;
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<std::shared_ptr<ms::Material>> mesh_materials;
	std::vector<std::vector<std::shared_ptr<ms::Material>>> mesh_variant_rows;
	InstanceTypes instance_types = flags.additional_instance_types;
	instance_types.emplace(InstanceType::Model);

	auto loaded_materials = GltfModelLoader::Materials::loadMaterials(owner, model_name, instance_types);

	for (size_t i = 0; i < src.nodes_count; ++i) {
		const cgltf_node& node = src.nodes[i];

		if (node.mesh) {
			auto [more_meshes, more_mesh_materials, more_var_rows] =
				GltfModelLoader::VertexGeometry::loadMeshes(
				node, *node.mesh, nullptr, model_name, meshes.size(), loaded_materials, src, flags
			);
			meshes.insert(meshes.end(), more_meshes.begin(), more_meshes.end());
			mesh_materials.insert(
				mesh_materials.end(), more_mesh_materials.begin(), more_mesh_materials.end()
			);
			mesh_variant_rows.insert(mesh_variant_rows.end(), more_var_rows.begin(), more_var_rows.end());
		}
	}

	GltfModelLoader::Materials::fixMissingMaterials(mesh_materials, owner, model_name, instance_types);

	auto variant_set = GltfModelLoader::Extensions::makeMaterialVariantSet(src, meshes, mesh_variant_rows, flags);

	// SpeedTree exports often embed multiple LOD levels as separate meshes in a single file
	// (names like "LOD000", "LOD100", "LOD200"...). If we put them all into a single Model LOD,
	// we will render all LODs at once, which looks like "detached" floating leaves/branches even
	// with wind disabled.
	//
	// Detect this naming convention and build a real multi-LOD Model.
	auto parse_lod_level = [](const std::string& name) -> std::optional<uint32_t> {
		// Accept "LOD0", "LOD000", "LOD1_xxx", etc. Use the first digit after "LOD" as the level.
		if (name.size() < 4) return std::nullopt;
		if (!(name[0] == 'L' && name[1] == 'O' && name[2] == 'D')) return std::nullopt;
		const unsigned char c = static_cast<unsigned char>(name[3]);
		if (!std::isdigit(c)) return std::nullopt;
		return static_cast<uint32_t>(name[3] - '0');
	};

	std::unordered_map<uint32_t, std::vector<size_t>> lod_to_indices;
	lod_to_indices.reserve(4);

	size_t lod_named = 0;
	for (size_t i = 0; i < meshes.size(); ++i) {
		if (!meshes[i]) continue;
		const auto& n = meshes[i]->getName();
		if (auto lvl = parse_lod_level(n)) {
			++lod_named;
			lod_to_indices[*lvl].push_back(i);
		}
	}

	// Only enable LOD grouping when we have 2+ LOD levels and *all* meshes follow the convention.
	const bool enable_lod_group = (lod_to_indices.size() >= 2) && (lod_named == meshes.size());

	if (enable_lod_group) {
		std::vector<uint32_t> levels;
		levels.reserve(lod_to_indices.size());
		for (const auto& [lvl, _] : lod_to_indices) {
			levels.push_back(lvl);
		}
		std::sort(levels.begin(), levels.end());

		std::vector<std::shared_ptr<Model>> lod_models;
		lod_models.reserve(levels.size());

		for (auto lvl : levels) {
			std::vector<std::shared_ptr<Mesh>> lod_meshes;
			std::vector<std::shared_ptr<ms::Material>> lod_materials;
			for (auto idx : lod_to_indices[lvl]) {
				lod_meshes.emplace_back(meshes[idx]);
				lod_materials.emplace_back(mesh_materials[idx]);
			}
			lod_models.emplace_back(
				Model::builder()
					.name(model_name + "_lod" + std::to_string(lvl))
					.meshes(std::move(lod_meshes))
					.materials(std::move(lod_materials))
					.build(assets)
			);
		}

		// Build distance thresholds based on LOD0 bounding box size.
		// Distances are upper bounds per LOD index; last is infinite.
		const auto bb = lod_models.front()->getBoundingBox();
		// Limitless::Box stores center+size.
		const float radius = std::max(0.001f, 0.5f * glm::length(bb.size));

		std::vector<float> distances;
		distances.reserve(lod_models.size());
		for (size_t i = 0; i < lod_models.size(); ++i) {
			if (i + 1 == lod_models.size()) {
				distances.emplace_back(std::numeric_limits<float>::infinity());
			} else {
				// tuned for viewer/debug usage
				const float base = 10.0f;
				const float step = 3.0f;
				distances.emplace_back(radius * base * std::pow(step, static_cast<float>(i)));
			}
		}

		auto root = Model::builder()
			.name(model_name)
			.selection(LodSelection::CameraDistance)
			.transition(LodTransition::None)
			.add_lods(lod_models, distances);
		if (variant_set) {
			root.materialVariantSet(variant_set);
		}
		if (owner.options().billboard_bundle) {
			root.billboardLodBundle(owner.options().billboard_bundle);
		}
		return root.build(assets);
	}

	auto b = Model::builder()
		.name(model_name)
		.meshes(std::move(meshes))
		.materials(std::move(mesh_materials));
	if (variant_set) {
		b.materialVariantSet(std::move(variant_set));
	}
	if (owner.options().billboard_bundle) {
		b.billboardLodBundle(owner.options().billboard_bundle);
	}
	return b.build(assets);
}

std::shared_ptr<Model>
loadModelFromParsedGltf(const GltfModelLoader& owner, Assets& assets, const cgltf_data& src) {
	const auto& path = owner.options().path;
	const auto& opts = owner.options();

	owner.bindLoadContext(assets, src);
	struct LoadContextGuard {
		const GltfModelLoader& o;
		~LoadContextGuard() { o.clearLoadContext(); }
	} load_context_guard {owner};
	// For VAO recreation, we need to have a unique name for each model.
	static std::atomic<size_t> model_count{size_t{0}};
	auto model_name = path.stem().string() + "_gltf" + std::to_string(model_count.fetch_add(1));

	std::cout << "Loading model: " << model_name << " from " << path.string() << std::endl;

	if (opts.register_in_assets) {
		if (assets.models.contains(model_name)) {
			std::cout << "Model already loaded, removing from assets" << std::endl;
			assets.models.remove(model_name);
		}
	}

	auto model = src.skins_count > 0
		? std::shared_ptr<Model>(loadSkeletalModel(owner, assets, src, model_name))
		: std::shared_ptr<Model>(loadPlainModel(owner, assets, src, model_name));

	if (opts.register_in_assets) {
		std::cout << "Adding model to assets" << std::endl;
		assets.models.add(model_name, model);
	}

	return model;
}

std::shared_ptr<Mesh> GltfModelLoader::simplifyIndexedVertexNormalTangentMesh(
	std::shared_ptr<Mesh> original_mesh,
	const std::string& mesh_name,
	const LodOptions& options,
	const std::vector<unsigned char>& vertex_locks)
{
	const auto& m = static_cast<const Mesh&>(*original_mesh);
	if (m.getDrawInfo()) {
		return original_mesh;
	}
	const auto& vs = m.getVertexStream();
	const auto* indexed = dynamic_cast<const IndexedStream*>(&vs);
	if (!indexed) {
		throw ModelLoadError {"unsupported vertex stream type for mesh simplification"};
	}
	const auto& byte_data = indexed->getData();
	constexpr size_t k_stride = sizeof(VertexNormalTangent);
	if (byte_data.empty() || byte_data.size() % k_stride != 0) {
		throw ModelLoadError {"invalid indexed vertex data size for mesh simplification"};
	}
	std::vector<VertexNormalTangent> vertices(byte_data.size() / k_stride);
	std::memcpy(vertices.data(), byte_data.data(), byte_data.size());

	const auto& src_indices = indexed->getIndices();
	std::vector<GLuint> indices(src_indices.begin(), src_indices.end());

	const size_t target_index_count = getTargetIndexCount(indices.size(), options.target);
	if (indices.size() <= target_index_count) {
		return original_mesh;
	}

	std::vector<GLuint> simplified_indices(indices.size());
	const unsigned char* locks_ptr = vertex_locks.empty() ? nullptr : vertex_locks.data();

	size_t new_index_count = meshopt_simplify(
		simplified_indices.data(),
		indices.data(),
		indices.size(),
		reinterpret_cast<const float*>(vertices.data()),
		vertices.size(),
		sizeof(VertexNormalTangent),
		target_index_count,
		options.target_error,
		0,
		nullptr
	);

	if (new_index_count > target_index_count && options.forced) {
		new_index_count = meshopt_simplifySloppy(
			simplified_indices.data(),
			indices.data(),
			indices.size(),
			reinterpret_cast<const float*>(vertices.data()),
			vertices.size(),
			sizeof(VertexNormalTangent),
			locks_ptr,
			target_index_count,
			options.target_error,
			nullptr
		);
	}
	simplified_indices.resize(new_index_count);

	std::vector<VertexNormalTangent> optimized_vertices(vertices.size());
	const size_t unique_vertex_count = meshopt_optimizeVertexFetch(
		optimized_vertices.data(),
		simplified_indices.data(),
		simplified_indices.size(),
		vertices.data(),
		vertices.size(),
		sizeof(VertexNormalTangent)
	);
	optimized_vertices.resize(unique_vertex_count);

	std::vector<uint32_t> out_indices(simplified_indices.begin(), simplified_indices.end());

	const auto stream = VertexStream::builder()
		.attribute(0, VertexStream::Attribute::Position, sizeof(VertexNormalTangent), offsetof(VertexNormalTangent, position))
		.attribute(1, VertexStream::Attribute::Normal, sizeof(VertexNormalTangent), offsetof(VertexNormalTangent, normal))
		.attribute(2, VertexStream::Attribute::Tangent, sizeof(VertexNormalTangent), offsetof(VertexNormalTangent, tangent))
		.attribute(3, VertexStream::Attribute::Uv, sizeof(VertexNormalTangent), offsetof(VertexNormalTangent, uv))
		.vertices(optimized_vertices)
		.indices(out_indices)
		.usage(VertexStream::Usage::Static)
		.draw(VertexStream::Draw::Triangles)
		.batched(RendererSettings::geometry_batching_enabled)
		.build();

	return Mesh::builder()
		.name(mesh_name)
		.vertex_stream(stream)
		.build();
}

std::shared_ptr<Mesh> GltfModelLoader::simplifyMesh(
	std::shared_ptr<Mesh> original_mesh,
	const LodOptions& options,
	const std::vector<unsigned char>& vertex_locks
) {
	const auto& mesh_name = original_mesh->getName();
	return GltfModelLoader::simplifyIndexedVertexNormalTangentMesh(
		std::move(original_mesh), mesh_name, options, vertex_locks);
}

Assets& GltfModelLoader::assetsForLoad() const {
	if (!load_assets_) {
		throw ModelLoadError {"gltf load: assets context not bound"};
	}
	return *load_assets_;
}

const cgltf_data& GltfModelLoader::dataForLoad() const {
	if (!load_data_) {
		throw ModelLoadError {"gltf load: cgltf data context not bound"};
	}
	return *load_data_;
}

std::shared_ptr<Model> GltfModelLoader::load(Assets& assets) const {
	const auto& path = options_.path;

	if (!options_.mesh_replacements.empty()) {
		throw ModelLoadError {"gltf mesh_replacements are not implemented yet"};
	}

	cgltf_options opts = cgltf_options {
		cgltf_file_type_invalid, // autodetect
		0, // auto json token count
		cgltf_memory_options {nullptr, nullptr, nullptr},
		cgltf_file_options {nullptr, nullptr, nullptr}
    };
	cgltf_data* out_data = nullptr;

    const auto path_str = path.string();

	cgltf_result gltf = cgltf_parse_file(&opts, path_str.c_str(), &out_data);
	if (gltf != cgltf_result_success) {
		if (out_data) {
			cgltf_free(out_data);
		}
		throw ModelLoadError {
			"failed to parse GLTF model file " + path.string() + ": "
			+ std::to_string(static_cast<int>(gltf))};
	}

	auto result = cgltf_load_buffers(&opts, out_data, path_str.c_str());
	if (result != cgltf_result_success) {
		cgltf_free(out_data);
		throw ModelLoadError {
			"failed to load buffers: " + std::to_string(static_cast<int>(result))};
	}

	if (out_data->scenes == nullptr) {
		cgltf_free(out_data);
		throw ModelLoadError {"no scene"};
	}

	// Ensure node.parent pointers are set so we can compute full node world transforms.
	GltfModelLoader::AccessorIO::buildNodeParents(*out_data);

	std::shared_ptr<Model> loaded = loadModelFromParsedGltf(*this, assets, *out_data);
	cgltf_free(out_data);
	return loaded;
}

} // namespace Limitless
