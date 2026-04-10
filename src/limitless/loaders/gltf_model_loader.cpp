#include "cgltf.h"

#include <chrono>
#include <cstdio>
#include <iostream>
#include <limitless/assets.hpp>
#include <limitless/camera.hpp>
#include <limitless/core/context.hpp>
#include <limitless/core/context.hpp>
#include <limitless/core/vertex_stream/vertex_stream_builder.hpp>
#include <limitless/models/mesh_builder.hpp>
#include <limitless/core/vertex.hpp>
#include <limitless/instances/model_instance.hpp>
#include <limitless/instances/skeletal_instance.hpp>
#include <limitless/loaders/gltf_model_loader.hpp>
#include <limitless/models/mesh.hpp>
#include <limitless/models/model.hpp>
#include <limitless/models/bones.hpp>
#include <limitless/models/line.hpp>
#include <limitless/models/mesh.hpp>
#include <limitless/models/model_builder.h>
#include <limitless/models/skeletal_model.hpp>
#include <limitless/ms/material_builder.hpp>
#include <limitless/ms/property.hpp>
#include <limitless/renderer/shader_type.hpp>
#include <limitless/renderer/renderer.hpp>
#include <limitless/renderer/renderer_settings.hpp>
#include <limitless/scene.hpp>
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

using namespace Limitless;

static std::string toString(cgltf_type type) {
	switch (type) {
	case cgltf_type_invalid:
		return "invalid";
	case cgltf_type_scalar:
		return "scalar";
	case cgltf_type_vec2:
		return "vec2";
	case cgltf_type_vec3:
		return "vec3";
	case cgltf_type_vec4:
		return "vec4";
	case cgltf_type_mat2:
		return "mat2";
	case cgltf_type_mat3:
		return "mat3";
	case cgltf_type_mat4:
		return "mat4";
	default:
		return "unknown(" + std::to_string(static_cast<int>(type)) + ")";
	}
}

static std::string toString(cgltf_component_type component_type) {
	switch (component_type) {
	case cgltf_component_type_invalid:
		return "invalid";
	case cgltf_component_type_r_8:
		return "signed byte";
	case cgltf_component_type_r_8u:
		return "unsigned byte";
	case cgltf_component_type_r_16:
		return "signed short";
	case cgltf_component_type_r_16u:
		return "unsigned short";
	case cgltf_component_type_r_32u:
		return "unsigned int";
	case cgltf_component_type_r_32f:
		return "float";
	default:
		return "unknown(" + std::to_string(static_cast<int>(component_type)) + ")";
	}
}

template <typename ElemType>
static std::vector<ElemType> copyFromAccessor(const cgltf_accessor& accessor) {
	if (accessor.is_sparse) {
		throw ModelLoadError {"sparse accessors not supported"};
	}

	if (std::is_same_v<ElemType, GLuint>
	    && (accessor.type != cgltf_type_scalar
	        || accessor.component_type != cgltf_component_type_r_32u)) {
		throw ModelLoadError {
			"GLuint should use scalar u32 type accessor, got " + toString(accessor.component_type)};
	}

	if (std::is_same_v<ElemType, glm::vec2>
	    && (accessor.type != cgltf_type_vec2
	        || accessor.component_type != cgltf_component_type_r_32f)) {
		throw ModelLoadError {"vec2 should use vec2 float type accessor"};
	}

	if (std::is_same_v<ElemType, glm::vec3>
	    && (accessor.type != cgltf_type_vec3
	        || accessor.component_type != cgltf_component_type_r_32f)) {
		throw ModelLoadError {
			"vec3 should use vec3 float type accessor, got " + toString(accessor.type) + ", "
			+ toString(accessor.component_type)};
	}

	if (std::is_same_v<ElemType, glm::vec4>
	    && (accessor.type != cgltf_type_vec4
	        || accessor.component_type != cgltf_component_type_r_32f)) {
		throw ModelLoadError {
			"vec4 should use vec4 float type accessor, got " + toString(accessor.type) + ", "
			+ toString(accessor.component_type)};
	}

	if (std::is_same_v<ElemType, unsigned short[4]>
	    && (accessor.type != cgltf_type_vec4
	        || accessor.component_type != cgltf_component_type_r_16u)) {
		throw ModelLoadError {
			"ushort4 should use vec4 short type accessor, got " + toString(accessor.type) + ", "
			+ toString(accessor.component_type)};
	}

	std::vector<ElemType> result;
	result.reserve(accessor.count);

	const uint8_t* data = static_cast<const uint8_t*>(accessor.buffer_view->buffer->data)
	                      + accessor.buffer_view->offset + accessor.offset;

	// Calculate element size based on component type and count
	size_t element_size = 0;
	switch (accessor.component_type) {
		case cgltf_component_type_r_8:
		case cgltf_component_type_r_8u:
			element_size = 1;
			break;
		case cgltf_component_type_r_16:
		case cgltf_component_type_r_16u:
			element_size = 2;
			break;
		case cgltf_component_type_r_32f:
		case cgltf_component_type_r_32u:
			element_size = 4;
			break;
		default:
			throw ModelLoadError {"unsupported component type"};
	}

	// Calculate components per element based on type
	size_t components_per_element = 1;
	switch (accessor.type) {
		case cgltf_type_scalar:
			components_per_element = 1;
			break;
		case cgltf_type_vec2:
			components_per_element = 2;
			break;
		case cgltf_type_vec3:
			components_per_element = 3;
			break;
		case cgltf_type_vec4:
			components_per_element = 4;
			break;
		case cgltf_type_mat2:
			components_per_element = 4;
			break;
		case cgltf_type_mat3:
			components_per_element = 9;
			break;
		case cgltf_type_mat4:
			components_per_element = 16;
			break;
		default:
			throw ModelLoadError {"unsupported accessor type"};
	}

	size_t actual_stride = accessor.stride > 0 ? accessor.stride : (element_size * components_per_element);

	for (cgltf_size i = 0; i < accessor.count; ++i) {
		result.emplace_back(*reinterpret_cast<const ElemType*>(data));
		data += actual_stride;
	}

	return result;
}

static glm::vec4 toVec4(const float (&src)[4]) {
	return glm::vec4 {src[0], src[1], src[2], src[3]};
}

static glm::vec3 toVec3(const float (&src)[3]) {
	return glm::vec3 {src[0], src[1], src[2]};
}

static glm::quat toQuat(const float (&src)[4]) {
	glm::quat result;
	// GLM quaternion storage depends on GLM_FORCE_QUAT_DATA_XYZW macro
	// definition. Enforce proper assignment in any case.
	result.x = src[0];
	result.y = src[1];
	result.z = src[2];
	result.w = src[3];

	return result;
}

// Convert GLTF quaternion to GLM one.
// GLTF quaternion array has (x, y, z, w) order, where w is the scalar.
static glm::quat toQuat(const std::array<float, 4>& src) {
	glm::quat result;
	// GLM quaternion storage depends on GLM_FORCE_QUAT_DATA_XYZW macro
	// definition. Enforce proper assignment in any case.
	result.x = src[0];
	result.y = src[1];
	result.z = src[2];
	result.w = src[3];

	return result;
}

// Specialized function for loading normalized quaternions
static std::vector<std::array<float, 4>> copyNormalizedQuaternionsFromAccessor(const cgltf_accessor& accessor) {
	if (!accessor.normalized) {
		throw ModelLoadError {"accessor is not normalized"};
	}

	if (accessor.type != cgltf_type_vec4) {
		throw ModelLoadError {"accessor is not vec4 type"};
	}

	std::vector<std::array<float, 4>> result;
	result.reserve(accessor.count);

	const uint8_t* data = static_cast<const uint8_t*>(accessor.buffer_view->buffer->data)
	                      + accessor.buffer_view->offset + accessor.offset;

	size_t actual_stride = accessor.stride > 0 ? accessor.stride : 8; // 4 components * 2 bytes

	for (cgltf_size i = 0; i < accessor.count; ++i) {
		std::array<float, 4> normalized_quat;

		if (accessor.component_type == cgltf_component_type_r_16) {
			const int16_t* src = reinterpret_cast<const int16_t*>(data);
			normalized_quat[0] = src[0] / 32767.0f;
			normalized_quat[1] = src[1] / 32767.0f;
			normalized_quat[2] = src[2] / 32767.0f;
			normalized_quat[3] = src[3] / 32767.0f;
		} else if (accessor.component_type == cgltf_component_type_r_16u) {
			const uint16_t* src = reinterpret_cast<const uint16_t*>(data);
			normalized_quat[0] = src[0] / 65535.0f;
			normalized_quat[1] = src[1] / 65535.0f;
			normalized_quat[2] = src[2] / 65535.0f;
			normalized_quat[3] = src[3] / 65535.0f;
		} else {
			throw ModelLoadError {"unsupported component type for normalized quaternions"};
		}

		result.emplace_back(normalized_quat);
		data += actual_stride;
	}

	return result;
}

static glm::mat4 toMat4(const float (&src)[16]) {
	return glm::mat4 {
		src[0],
		src[1],
		src[2],
		src[3],
		src[4],
		src[5],
		src[6],
		src[7],
		src[8],
		src[9],
		src[10],
		src[11],
		src[12],
		src[13],
		src[14],
		src[15]};
}

static glm::mat4 toMat4(const std::array<float, 16>& src) {
	return glm::mat4 {
		src[0],
		src[1],
		src[2],
		src[3],
		src[4],
		src[5],
		src[6],
		src[7],
		src[8],
		src[9],
		src[10],
		src[11],
		src[12],
		src[13],
		src[14],
		src[15]};
}

static glm::mat4 getNodeMatrix(const cgltf_node& joint) {
	if (joint.has_matrix) {
		return toMat4(joint.matrix);

	} else {
		glm::mat4 translation = glm::mat4(1.f);
		glm::mat4 rotation    = glm::mat4(1.f);
		glm::mat4 scaling     = glm::mat4(1.f);

		if (joint.has_translation) {
			translation = glm::translate(glm::mat4(1.f), toVec3(joint.translation));
		}

		if (joint.has_rotation) {
			glm::quat rot_quat = glm::normalize(toQuat(joint.rotation));
			rotation           = glm::mat4_cast(rot_quat);
		}

		if (joint.has_scale) {
			scaling = glm::scale(glm::mat4(1.f), toVec3(joint.scale));
		}

		glm::mat4 node_transform = translation * rotation * scaling;

		return node_transform;
	}
}

// cgltf does not always populate node.parent pointers depending on parse/fixup path.
// We build them from children arrays so we can compute world transforms reliably.
static void buildNodeParents(cgltf_data& data) {
	for (cgltf_size i = 0; i < data.nodes_count; ++i) {
		data.nodes[i].parent = nullptr;
	}
	for (cgltf_size i = 0; i < data.nodes_count; ++i) {
		cgltf_node& n = data.nodes[i];
		for (cgltf_size c = 0; c < n.children_count; ++c) {
			if (n.children[c]) {
				n.children[c]->parent = &n;
			}
		}
	}
}

// Compute full transform for a node including parent chain.
// glTF node transforms are hierarchical; for mesh nodes under a rotated parent,
// using only the local node transform will result in rotated/offset meshes.
static glm::mat4 getNodeWorldMatrix(const cgltf_node& node) {
	glm::mat4 m = getNodeMatrix(node);
	for (auto* p = node.parent; p != nullptr; p = p->parent) {
		m = getNodeMatrix(*p) * m;
	}
	return m;
}

// SpeedTree "packed normal in float" codec (matches shaders/functions/wind.glsl: st_unpack_normal_from_float).
// This is a 3x4-bit packing commonly used by SpeedTree/Unity to store a direction vector in a single float:
//   packed = x_i + y_i/16 + z_i/256, where i are 4-bit integers of (n*0.5+0.5)*16.
static glm::vec3 stUnpackNormalFromFloat(float f) {
	auto fractf = [](float x) -> float { return x - std::floor(x); };
	const float x = fractf(f / 16.0f);
	const float y = fractf(f);
	const float z = fractf(f * 16.0f);
	return glm::vec3(x, y, z) * 2.0f - 1.0f;
}

static float stPackNormalToFloat(const glm::vec3& n) {
	// Map [-1,1] -> [0,1) and quantize to 4-bit per component.
	glm::vec3 v = glm::clamp(n * 0.5f + 0.5f, 0.0f, 0.999999f);
	const int xi = static_cast<int>(std::floor(v.x * 16.0f));
	const int yi = static_cast<int>(std::floor(v.y * 16.0f));
	const int zi = static_cast<int>(std::floor(v.z * 16.0f));
	return static_cast<float>(xi) + static_cast<float>(yi) / 16.0f + static_cast<float>(zi) / 256.0f;
}

// Preserve UE4/SpeedTree "phase" encoded as +16*N in the packed float.
// UnpackNormalFromFloat() ignores integer multiples of 16 (due to frac), but Oscillate/Turbulence
// use the raw float as a phase offset. If we unpack->rotate->repack without preserving the +16*N
// part, wind can become visibly synchronized/banded.
static void stRotatePackedNormalFloatPreservePhase(float& packed, const glm::mat3& R) {
	// Extract integer phase bucket in units of 16.0 (seed injection uses +16*N).
	const float phase = std::floor(packed / 16.0f + 1e-6f);
	glm::vec3 dir = stUnpackNormalFromFloat(packed);
	dir = glm::normalize(R * dir);
	packed = stPackNormalToFloat(dir) + 16.0f * phase;
}

static int stDecodeGeomType(const glm::vec2& uv5_payloadc_geom) {
	// Mirrors shaders/functions/wind.glsl geometryType decode.
	const int GEOM_TYPE_BRANCH = 0;
	const int GEOM_TYPE_FROND = 1;
	const int GEOM_TYPE_LEAF = 2;
	const int GEOM_TYPE_FACINGLEAF = 3;

	float gEnc = uv5_payloadc_geom.y;
	int geomType = GEOM_TYPE_BRANCH;
	int gInt = static_cast<int>(gEnc + 0.25f);
	if (gInt >= 0 && gInt <= 6) {
		geomType = gInt;
		if (geomType > GEOM_TYPE_FACINGLEAF) {
			geomType -= 2;
		}
		geomType = std::clamp(geomType, GEOM_TYPE_BRANCH, GEOM_TYPE_FACINGLEAF);
	} else {
		int origGeom = static_cast<int>(uv5_payloadc_geom.x + 0.25f);
		geomType = GEOM_TYPE_BRANCH;
		if (origGeom == 3) geomType = GEOM_TYPE_FROND;
		else if (origGeom == 4) geomType = GEOM_TYPE_LEAF;
		else if (origGeom == 5) geomType = GEOM_TYPE_FACINGLEAF;
	}
	return geomType;
}

// -------------------------------------------------------------------------------------------------
// SpeedTree payload space fixup (Blender/glTF pipeline)
//
// Problem:
//  - Blender/glTF export can apply coordinate-system conversion (e.g., Z-up -> Y-up, or artist-applied
//    rotations) to POSITION data, but not to "payload" stored in extra TEXCOORD sets.
//  - This breaks SpeedTree leaf anchors and packed direction vectors: leaf tumble pivots become wrong
//    and leaves "explode"/detach.
//
// Approach:
//  - For leaf vertices, the per-vertex anchor should be near the vertex position (leaf card verts cluster
//    around their anchor).
//  - We brute-force a small set of orthonormal axis permutations (48 rotations/reflections) to find the
//    transform that best aligns anchors with positions (minimizes median |pos - R*anchor|).
//  - If it is a clear improvement, we apply it to:
//      * leaf anchors (uv2.xy + uv3.x)
//      * packed direction floats (uv1.y, uv4.x, uv4.y) by unpack -> rotate -> repack
//
// This keeps the pipeline forgiving without requiring Blender scripts.
// -------------------------------------------------------------------------------------------------
static std::vector<glm::mat3> stCandidateOrthonormalAxisTransforms() {
	// Generate all 48 axis-aligned orthonormal transforms (permutations + sign flips),
	// including reflections (determinant -1).
	std::vector<glm::mat3> out;
	out.reserve(48);

	int p[3] = {0, 1, 2};

	// simple permutation loop (6 perms)
	do {
		for (int sx : {-1, 1}) {
			for (int sy : {-1, 1}) {
				for (int sz : {-1, 1}) {
					glm::vec3 c0(0.0f), c1(0.0f), c2(0.0f);
					c0[p[0]] = float(sx);
					c1[p[1]] = float(sy);
					c2[p[2]] = float(sz);
					glm::mat3 m(c0, c1, c2);
					out.emplace_back(m);
				}
			}
		}
	} while (std::next_permutation(p, p + 3));

	// Ensure uniqueness (some permutations/signs can collide depending on generation order)
	// but we keep it simple: 24 is small and this is only called per-primitive.
	return out;
}

static float medianOf(std::vector<float>& v) {
	if (v.empty()) return 0.0f;
	const size_t mid = v.size() / 2;
	std::nth_element(v.begin(), v.begin() + mid, v.end());
	return v[mid];
}

[[maybe_unused]] static std::optional<glm::mat3> stFindBestPayloadRotation(const std::vector<VertexNormalTangentUv6>& verts) {
	// Collect leaf samples: (pos, anchor)
	std::vector<glm::vec3> pos;
	std::vector<glm::vec3> anchor;
	pos.reserve(1024);
	anchor.reserve(1024);

	glm::vec3 pmin(std::numeric_limits<float>::infinity());
	glm::vec3 pmax(-std::numeric_limits<float>::infinity());

	for (const auto& v : verts) {
		const int geomType = stDecodeGeomType(v.uv5);
		const bool isLeaf = geomType >= 2;
		if (!isLeaf) continue;

		const glm::vec3 a(v.uv2.x, v.uv2.y, v.uv3.x);
		// Skip missing anchors (older exports)
		if (glm::length2(a) < 1e-8f) continue;

		pos.push_back(v.position);
		anchor.push_back(a);
		pmin = glm::min(pmin, v.position);
		pmax = glm::max(pmax, v.position);

		if (pos.size() >= 1024) break;
	}

	if (pos.size() < 64) {
		return std::nullopt;
	}

	const float diag = glm::length(pmax - pmin);
	if (diag < 1e-6f) {
		return std::nullopt;
	}

	auto candidates = stCandidateOrthonormalAxisTransforms();
	glm::mat3 best(1.0f);
	float bestMed = std::numeric_limits<float>::infinity();

	for (const auto& R : candidates) {
		std::vector<float> d;
		d.reserve(pos.size());
		for (size_t i = 0; i < pos.size(); ++i) {
			const glm::vec3 a = R * anchor[i];
			d.push_back(glm::length(pos[i] - a));
		}
		float med = medianOf(d);
		if (med < bestMed) {
			bestMed = med;
			best = R;
		}
	}

	// Baseline (identity)
	{
		std::vector<float> d0;
		d0.reserve(pos.size());
		for (size_t i = 0; i < pos.size(); ++i) {
			d0.push_back(glm::length(pos[i] - anchor[i]));
		}
		const float baseMed = medianOf(d0);

		// Apply only if it's a strong win and yields a "reasonable" anchor proximity.
		// Leaf card verts are typically within a few percent of the tree bbox diagonal from their anchor.
		const bool strongWin = (bestMed < baseMed * 0.25f);
		const bool reasonable = (bestMed < diag * 0.10f);
		if (!strongWin || !reasonable) {
			return std::nullopt;
		}
	}

	std::cout << "[speedtree] payload appears in different basis vs geometry; applying payload-space fixup\n";
	return best;
}

// Fix leaf anchor scale/translation when DCC bakes transforms into POSITION but leaves payload untouched.
// We solve a simple similarity transform for anchors only:
//   a' = s * a + t
// and apply it to leaf anchors (uv2.xy + uv3.x) if it significantly reduces |pos - anchor| for leaf verts.
[[maybe_unused]] static void stFixLeafAnchorScaleTranslation(std::vector<VertexNormalTangentUv6>& verts) {
	if (verts.empty()) return;

	std::vector<glm::vec3> pos;
	std::vector<glm::vec3> anchor;
	pos.reserve(1024);
	anchor.reserve(1024);

	glm::vec3 pmin(std::numeric_limits<float>::infinity());
	glm::vec3 pmax(-std::numeric_limits<float>::infinity());

	for (const auto& v : verts) {
		const int geomType = stDecodeGeomType(v.uv5);
		const bool isLeaf = geomType >= 2;
		if (!isLeaf) continue;

		const glm::vec3 a(v.uv2.x, v.uv2.y, v.uv3.x);
		if (glm::length2(a) < 1e-8f) continue;

		pos.push_back(v.position);
		anchor.push_back(a);
		pmin = glm::min(pmin, v.position);
		pmax = glm::max(pmax, v.position);
		if (pos.size() >= 1024) break;
	}

	if (pos.size() < 64) return;

	const float diag = glm::length(pmax - pmin);
	if (diag < 1e-6f) return;

	// Robust-ish uniform scale from bbox extents ratio.
	glm::vec3 amin(std::numeric_limits<float>::infinity());
	glm::vec3 amax(-std::numeric_limits<float>::infinity());
	for (const auto& a : anchor) {
		amin = glm::min(amin, a);
		amax = glm::max(amax, a);
	}
	const glm::vec3 eP = pmax - pmin;
	const glm::vec3 eA = amax - amin;

	std::vector<float> scales;
	scales.reserve(3);
	if (std::abs(eA.x) > 1e-6f) scales.push_back(eP.x / eA.x);
	if (std::abs(eA.y) > 1e-6f) scales.push_back(eP.y / eA.y);
	if (std::abs(eA.z) > 1e-6f) scales.push_back(eP.z / eA.z);
	if (scales.empty()) return;

	auto medianOf = [](std::vector<float>& v) -> float {
		const size_t mid = v.size() / 2;
		std::nth_element(v.begin(), v.begin() + mid, v.end());
		return v[mid];
	};
	float s = medianOf(scales);
	if (!std::isfinite(s)) return;
	// Clamp to a sane range to avoid catastrophic mistakes.
	s = std::max(1e-4f, std::min(1e4f, s));

	// Translation to align centers.
	glm::vec3 pc(0.0f), ac(0.0f);
	for (size_t i = 0; i < pos.size(); ++i) {
		pc += pos[i];
		ac += anchor[i];
	}
	pc /= float(pos.size());
	ac /= float(anchor.size());
	const glm::vec3 t = pc - s * ac;

	// Evaluate improvement using median distance.
	std::vector<float> d0;
	std::vector<float> d1;
	d0.reserve(pos.size());
	d1.reserve(pos.size());
	for (size_t i = 0; i < pos.size(); ++i) {
		d0.push_back(glm::length(pos[i] - anchor[i]));
		d1.push_back(glm::length(pos[i] - (s * anchor[i] + t)));
	}
	const float med0 = medianOf(d0);
	const float med1 = medianOf(d1);

	const bool strongWin = (med1 < med0 * 0.25f);
	const bool reasonable = (med1 < diag * 0.10f);
	if (!strongWin || !reasonable) return;

	std::cout << "[speedtree] leaf anchors appear in different scale/offset; applying anchor fix (s=" << s
			  << ")\n";

	for (auto& v : verts) {
		const int geomType = stDecodeGeomType(v.uv5);
		const bool isLeaf = geomType >= 2;
		if (!isLeaf) continue;
		glm::vec3 a(v.uv2.x, v.uv2.y, v.uv3.x);
		if (glm::length2(a) < 1e-8f) continue;
		a = s * a + t;
		v.uv2.x = a.x;
		v.uv2.y = a.y;
		v.uv3.x = a.z;
	}
}

static void stRotatePackedNormalFloat(float& packed, const glm::mat3& R) {
	stRotatePackedNormalFloatPreservePhase(packed, R);
}

// If the imported mesh is Z-up (height along +Z), rotate it to Y-up so the engine/viewer
// coordinate conventions remain consistent. This must rotate BOTH geometry and SpeedTree payload.
[[maybe_unused]] static void stMaybeRotateZUpToYUp(std::vector<VertexNormalTangentUv6>& verts) {
	if (verts.empty()) return;

	// Estimate extents from positions (model space, after bake_transform).
	glm::vec3 mn(std::numeric_limits<float>::infinity());
	glm::vec3 mx(-std::numeric_limits<float>::infinity());
	for (const auto& v : verts) {
		mn = glm::min(mn, v.position);
		mx = glm::max(mx, v.position);
	}
	const glm::vec3 e = mx - mn;

	// Heuristic: treat as Z-up when Z extent is clearly dominant.
	if (!(e.z > e.y * 1.25f && e.z > e.x * 1.25f)) {
		return;
	}

	std::cout << "[speedtree] detected Z-up mesh; rotating payload/geometry to Y-up\n";
	// Rotate +90° around X: (x, y, z) -> (x, z, -y)
	const glm::mat3 Rx90 = glm::mat3(
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, -1.0f, 0.0f)
	);

	for (auto& v : verts) {
		// Geometry
		v.position = Rx90 * v.position;
		v.normal = glm::normalize(Rx90 * v.normal);
		glm::vec3 t = glm::vec3(v.tangent);
		t = glm::normalize(Rx90 * t);
		// keep tangent orthogonal to normal
		t = glm::normalize(t - v.normal * glm::dot(t, v.normal));
		v.tangent = glm::vec4(t, v.tangent.w);

		// Payload anchors (uv2.xy + uv3.x) are object-space positions
		glm::vec3 a(v.uv2.x, v.uv2.y, v.uv3.x);
		a = Rx90 * a;
		v.uv2.x = a.x;
		v.uv2.y = a.y;
		v.uv3.x = a.z;

		// Packed direction floats (must respect payload layout by geometry type).
		// - uv1.y: branch1 packed offset dir (all geometry types)
		// - uv4: leaves store (packedGrowthDir, packedRippleDir)
		// - uv4: branches/fronds store (branch2_weight, packedBranch2Dir)
		const int geomType = stDecodeGeomType(v.uv5);
		const bool isLeaf = geomType >= 2;
		stRotatePackedNormalFloat(v.uv1.y, Rx90);
		if (isLeaf) {
			stRotatePackedNormalFloat(v.uv4.x, Rx90);
			stRotatePackedNormalFloat(v.uv4.y, Rx90);
		} else {
			stRotatePackedNormalFloat(v.uv4.y, Rx90);
		}
	}
}

static std::string generateMeshName(const std::string& model_name, size_t mesh_index) {
	return model_name + "_mesh" + std::to_string(mesh_index);
}

// TODO: return std::vector of mesh + material.
// Note that material pointer can be empty if mesh does not have material.
static std::pair<std::vector<std::shared_ptr<Mesh>>, std::vector<std::shared_ptr<ms::Material>>>
loadMeshes(
	const cgltf_node& node,
	const cgltf_mesh& mesh,
	const cgltf_skin* skin,
	const std::string& model_name,
	size_t mesh_index,
	const std::vector<std::shared_ptr<ms::Material>>& materials,
	const cgltf_data& data,
    const ModelLoaderFlags& flags
) {
	auto base_mesh_name =
		std::string(mesh.name ? mesh.name : generateMeshName(model_name, mesh_index));
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<std::shared_ptr<ms::Material>> mesh_materials;

	auto select_mesh_material = [&](const cgltf_primitive& primitive) -> std::shared_ptr<ms::Material> {
		if (!primitive.material) {
			return {nullptr};
		}
		return materials.at(cgltf_material_index(&data, primitive.material));
	};

	auto mesh_matrix = getNodeWorldMatrix(node);

    struct WindPrimTmp {
        std::string mesh_name;
        const cgltf_primitive* primitive = nullptr;
        std::vector<GLuint> indices;
        std::vector<VertexNormalTangentUv6> wind_vertices;
        bool has_speedtree_payload = false;
    };
    std::vector<WindPrimTmp> wind_prims;

	for (cgltf_size i = 0, n = mesh.primitives_count; i < n; ++i) {
		auto mesh_name = base_mesh_name + (n == 1 ? std::string() : std::to_string(i));
		std::vector<glm::vec3> positions;
		std::vector<glm::vec4> tangents;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec2> uvs1;
		std::vector<glm::vec2> uvs2;
		std::vector<glm::vec2> uvs3;
		std::vector<glm::vec2> uvs4;
		std::vector<glm::vec2> uvs5;
		std::vector<GLuint> indices;
		std::vector<VertexNormalTangent> vertices;
		std::vector<std::array<GLuint, 4>> bone_indices;
		std::vector<std::array<float, 4>> bone_weights;

		const cgltf_primitive& primitive = mesh.primitives[i];

		if (primitive.type != cgltf_primitive_type_triangles) {
			throw ModelLoadError {"non-triangle primitives are not supported yet"};
		}

		if (primitive.indices) {
			if (primitive.indices->count % 3 != 0) {
				throw ModelLoadError {"triangle indices count is not divisible by 3"};
			}

			switch (primitive.indices->component_type) {
			case cgltf_component_type_r_32u:
				indices = copyFromAccessor<GLuint>(*primitive.indices);
				break;
			case cgltf_component_type_r_16u: {
				auto trash_indices = copyFromAccessor<uint16_t>(*primitive.indices);
				for (auto trash_indice : trash_indices) {
					indices.emplace_back(static_cast<GLuint>(trash_indice));
				}
				break;
			}
			case cgltf_component_type_r_8u: {
				auto trash_indices = copyFromAccessor<uint8_t>(*primitive.indices);
				for (auto trash_indice : trash_indices) {
					indices.emplace_back(static_cast<GLuint>(trash_indice));
				}
				break;
			}
			case cgltf_component_type_invalid:
			case cgltf_component_type_r_8:
			case cgltf_component_type_r_16:
			case cgltf_component_type_r_32f:
			case cgltf_component_type_max_enum:
				throw ModelLoadError {"invalid indice component type"};
			}
		} else {
			// has no indices.
			throw ModelLoadError {"no indices in model"};
		}

		for (cgltf_size j = 0; j < primitive.attributes_count; ++j) {
			const cgltf_attribute& attribute = primitive.attributes[j];

			switch (attribute.type) {
			case cgltf_attribute_type_tangent:
				tangents = copyFromAccessor<glm::vec4>(*attribute.data);
				break;
			case cgltf_attribute_type_normal:
				normals = copyFromAccessor<glm::vec3>(*attribute.data);
				break;
			case cgltf_attribute_type_position:
				positions = copyFromAccessor<glm::vec3>(*attribute.data);
				break;
			case cgltf_attribute_type_texcoord:
				if (attribute.index == 0) {
					uvs = copyFromAccessor<glm::vec2>(*attribute.data);
				} else if (flags.isPresent(ModelLoaderOption::Wind)) {
					// SpeedTree-style wind payload uses multiple TEXCOORD sets.
					// We keep them as vec2 to match glTF TEXCOORD_n (VEC2) accessors.
					switch (attribute.index) {
						case 1: uvs1 = copyFromAccessor<glm::vec2>(*attribute.data); break;
						case 2: uvs2 = copyFromAccessor<glm::vec2>(*attribute.data); break;
						case 3: uvs3 = copyFromAccessor<glm::vec2>(*attribute.data); break;
						case 4: uvs4 = copyFromAccessor<glm::vec2>(*attribute.data); break;
						case 5: uvs5 = copyFromAccessor<glm::vec2>(*attribute.data); break;
						default:
							// ignore
							break;
					}
				}
				break;
			case cgltf_attribute_type_joints:
				// TODO: handle host big endianess, as gltf data is little
				// endian.
				// TODO: make this more efficient by promoting in
				// copyFromAccessor.
				if (attribute.data->component_type == cgltf_component_type_r_16u) {
					auto loaded_bone_indices =
						copyFromAccessor<std::array<unsigned short, 4>>(*attribute.data);
					for (auto& [u1, u2, u3, u4] : loaded_bone_indices) {
						bone_indices.emplace_back(std::array<GLuint, 4> {u1, u2, u3, u4});
					}
				} else if (attribute.data->component_type == cgltf_component_type_r_8u) {
					auto loaded_bone_indices =
						copyFromAccessor<std::array<unsigned char, 4>>(*attribute.data);
					for (auto& [u1, u2, u3, u4] : loaded_bone_indices) {
						bone_indices.emplace_back(std::array<GLuint, 4> {u1, u2, u3, u4});
					}
				} else {
					throw "unsupported accessor type "
						+ std::to_string(attribute.data->component_type) + " for bone joint IDs";
				}

				break;

			case cgltf_attribute_type_weights:
				if (attribute.data->component_type == cgltf_component_type_r_16u) {
					auto loaded_bone_weights =
						copyFromAccessor<std::array<unsigned short, 4>>(*attribute.data);

					for (auto& [u1, u2, u3, u4] : loaded_bone_weights) {
						bone_weights.emplace_back(std::array<float, 4> {
							u1 / 65535.f, u2 / 65535.f, u3 / 65535.f, u4 / 65535.f});
					}
				} else if (attribute.data->component_type == cgltf_component_type_r_8u) {
					auto loaded_bone_weights =
						copyFromAccessor<std::array<unsigned char, 4>>(*attribute.data);
					for (auto& [u1, u2, u3, u4] : loaded_bone_weights) {
						bone_weights.emplace_back(std::array<float, 4> {
							u1 / 255.f, u2 / 255.f, u3 / 255.f, u4 / 255.f});
					}
				} else if (attribute.data->component_type == cgltf_component_type_r_32f) {
					bone_weights = copyFromAccessor<std::array<float, 4>>(*attribute.data);
				} else {
					throw ModelLoadError {
						"unsupported accessor type "
						+ std::to_string(attribute.data->component_type) + " for bone weights"};
				}
				break;
            case cgltf_attribute_type_color: {
//                copyFromAccessor<glm::vec4>(*attribute.data);
                break;
            }
			default:
				throw ModelLoadError {"unsupported attribute type " + std::to_string(attribute.type)};
				break;
			}
		}

		if (normals.empty()) {
			// model has no normals, generating dummy ones.
			normals = std::vector<glm::vec3>(positions.size(), glm::vec3 {0.0f, 1.0f, 0.0f});
		}

		if (tangents.empty()) {
			// model has no tangents, generating dummy ones.
			// TODO: calculate?
			tangents = std::vector<glm::vec4>(positions.size(), glm::vec4 {0.0f, 1.0f, 0.0f, 0.0f});
		}

		if (uvs.empty()) {
			uvs = std::vector<glm::vec2>(positions.size(), glm::vec2 {0.0f, 0.0f});
		}

		if (flags.isPresent(ModelLoaderOption::Wind)) {
			// Default missing wind UV sets to 0 so shader reads a stable value.
			if (uvs1.empty()) uvs1 = std::vector<glm::vec2>(positions.size(), glm::vec2 {0.0f});
			if (uvs2.empty()) uvs2 = std::vector<glm::vec2>(positions.size(), glm::vec2 {0.0f});
			if (uvs3.empty()) uvs3 = std::vector<glm::vec2>(positions.size(), glm::vec2 {0.0f});
			if (uvs4.empty()) uvs4 = std::vector<glm::vec2>(positions.size(), glm::vec2 {0.0f});
			if (uvs5.empty()) uvs5 = std::vector<glm::vec2>(positions.size(), glm::vec2 {0.0f});
		}

		if (positions.size() != normals.size() || positions.size() != tangents.size()
		    || positions.size() != uvs.size()) {
			throw ModelLoadError {
				"mismatching count of vertex attributes: " + std::to_string(positions.size())
				+ " positions, " + std::to_string(normals.size()) + " normals, "
				+ std::to_string(tangents.size()) + " tangents, " + std::to_string(uvs.size())
				+ " and UVs"};
		}

		if (flags.isPresent(ModelLoaderOption::Wind) && !skin) {
			// Validate additional UV set sizes if present (only for plain meshes).
			auto check = [&](const char* name, const std::vector<glm::vec2>& v) {
				if (v.size() != positions.size()) {
					throw ModelLoadError {
						std::string("mismatching count of wind UV set ") + name + ": "
						+ std::to_string(v.size()) + " != " + std::to_string(positions.size())
					};
				}
			};
			check("TEXCOORD_1", uvs1);
			check("TEXCOORD_2", uvs2);
			check("TEXCOORD_3", uvs3);
			check("TEXCOORD_4", uvs4);
			check("TEXCOORD_5", uvs5);
		}


        // Track whether this primitive actually contains SpeedTree payload attributes.
        // If any required TEXCOORD_1..5 are missing, feeding zeros into the SpeedTree path can make
        // different parts of the tree move incoherently. In that case we fall back to simple wind
        // for that mesh/material (keeps the whole tree moving together).
        const bool has_uv1 = !uvs1.empty();
        const bool has_uv2 = !uvs2.empty();
        const bool has_uv3 = !uvs3.empty();
        const bool has_uv4 = !uvs4.empty();
        const bool has_uv5 = !uvs5.empty();
        const bool has_speedtree_payload = has_uv1 && has_uv2 && has_uv3 && has_uv4 && has_uv5;

		vertices.reserve(positions.size());
		std::vector<VertexNormalTangentUv6> wind_vertices;
		const bool build_wind_vertices = flags.isPresent(ModelLoaderOption::Wind) && !skin;
		if (build_wind_vertices) {
			wind_vertices.reserve(positions.size());
		}

        // Wind payload UV flip:
        // In our SpeedTree->FBX->Blender->glTF pipeline, Blender flips the V coordinate for ALL UV layers,
        // including uv1..uv5 that we use as wind payload. We must undo that (y := 1 - y) for payload.
        //
        // NOTE: This is a pipeline contract. We keep it deterministic.
        const bool payload_unflip = build_wind_vertices;

		for (size_t i = 0; i < positions.size(); ++i) {
			auto maybeFlipUv0 = [&](glm::vec2 v) {
				return flags.isPresent(Limitless::ModelLoaderOption::FlipUV) ? v : glm::vec2(v.x, 1.0f - v.y);
			};
            auto maybeFixPayload = [&](glm::vec2 v) {
                return payload_unflip ? glm::vec2(v.x, 1.0f - v.y) : v;
            };

			auto uv = maybeFlipUv0(uvs[i]);

			if (build_wind_vertices) {
				wind_vertices.emplace_back(VertexNormalTangentUv6{
					positions[i],
					normals[i],
					tangents[i], // glTF tangent is vec4: xyz=tangent, w=handedness
					uv,
					maybeFixPayload(uvs1[i]),
					maybeFixPayload(uvs2[i]),
					maybeFixPayload(uvs3[i]),
					maybeFixPayload(uvs4[i]),
					maybeFixPayload(uvs5[i])
				});
			} else {
				vertices.emplace_back(VertexNormalTangent {
					positions[i],
					normals[i],
					tangents[i], // glTF tangent is vec4: xyz=tangent, w=handedness
					uv});
			}
		}

		if (!skin) {
			// plain mesh.

			// move all vertices from mesh space into model space.
			// Also rotate normals/tangents to match the baked transform, otherwise lighting and
			// wind (leaf tumble using normals) can look incorrect and assets may appear rotated.
			const glm::mat3 M = glm::mat3(mesh_matrix);
            // Pure rotation part for transforming direction payloads.
            // Using M (which contains scale) and normalizing can skew directions under non-uniform scaling.
            const glm::vec3 c0 = glm::vec3(mesh_matrix[0]);
            const glm::vec3 c1 = glm::vec3(mesh_matrix[1]);
            const glm::vec3 c2 = glm::vec3(mesh_matrix[2]);
            glm::mat3 R = glm::mat3(
                glm::normalize(c0),
                glm::normalize(c1),
                glm::normalize(c2)
            );
            // If the node transform includes a reflection (negative determinant), fix it so we use a proper rotation.
            // Reflections can flip packed direction payloads and make different parts move in opposite directions.
            if (glm::determinant(R) < 0.0f) {
                R[0] = -R[0];
            }
			const glm::mat3 N = glm::transpose(glm::inverse(M));

			auto bake_transform = [&](auto& vert) {
				auto model_position = mesh_matrix * glm::vec4(vert.position, 1.f);
				vert.position = glm::vec3(model_position.x, model_position.y, model_position.z);

				// transform and normalize normal
				vert.normal = glm::normalize(N * vert.normal);

				// transform tangent as a direction (preserve handedness in .w)
				glm::vec3 t = glm::vec3(vert.tangent);
				t = glm::normalize(M * t);
				// re-orthogonalize tangent to normal
				t = glm::normalize(t - vert.normal * glm::dot(t, vert.normal));
				vert.tangent = glm::vec4(t, vert.tangent.w);
			};

			if (flags.isPresent(ModelLoaderOption::Wind)) {
				for (auto& vert : wind_vertices) {
					bake_transform(vert);

					// SpeedTree payload note:
					// Some SpeedTree exports store object-space anchor positions in extra TEXCOORD sets
					// (commonly TEXCOORD_2.xy + TEXCOORD_3.x). Since we bake node transforms into
					// POSITION/NORMAL/TANGENT, we must bake these anchor positions too, otherwise leaf
					// tumble/ripple will use mismatched spaces and can look chaotic or "explode".
					glm::vec3 anchor_os {vert.uv2.x, vert.uv2.y, vert.uv3.x};
					auto anchor_ms = mesh_matrix * glm::vec4(anchor_os, 1.f);
					vert.uv2.x = anchor_ms.x;
					vert.uv2.y = anchor_ms.y;
					vert.uv3.x = anchor_ms.z;

					// Also bake packed direction vectors stored in floats (Unity/SpeedTree convention).
					// These encode object-space directions; if we bake node transforms into vertex positions,
					// we must rotate these directions too or different sub-mesh parts can move incoherently.
					auto rotatePacked = [&](float& packed) {
						stRotatePackedNormalFloatPreservePhase(packed, R);
					};

					const int geomType = stDecodeGeomType(vert.uv5);
					const bool isLeaf = geomType >= 2; // LEAF or FACING_LEAF

					// Branch payload: uv1 = (weight, packedOffsetDir)
					rotatePacked(vert.uv1.y);

					if (isLeaf) {
						// Leaf payload (SpeedTree/Unity common convention):
						// uv4 = (packedGrowthDir, packedRippleDir)
						rotatePacked(vert.uv4.x);
						rotatePacked(vert.uv4.y);
						// NOTE: uv3.y is leafScale (scalar), do not transform it as a position.
					} else {
						// Optional branch2 payload (if present) historically used uv4.y; keep best-effort rotation.
						rotatePacked(vert.uv4.y);
					}
				}

				// Align payload basis (anchors + packed dirs) to geometry if DCC tools converted POSITION
				// but left TEXCOORD payload untouched. This is deterministic and only applied on a strong win.
				if (has_speedtree_payload) {
					if (auto rotOpt = stFindBestPayloadRotation(wind_vertices); rotOpt) {
						const glm::mat3 P = *rotOpt;
						auto rotatePackedExtra = [&](float& packed) {
							stRotatePackedNormalFloatPreservePhase(packed, P);
						};

						for (auto& vert : wind_vertices) {
							const int geomType = stDecodeGeomType(vert.uv5);
							const bool isLeaf = geomType >= 2;
							if (isLeaf) {
								glm::vec3 a(vert.uv2.x, vert.uv2.y, vert.uv3.x);
								a = P * a;
								vert.uv2.x = a.x;
								vert.uv2.y = a.y;
								vert.uv3.x = a.z;

								rotatePackedExtra(vert.uv4.x);
								rotatePackedExtra(vert.uv4.y);
							} else {
								// Branch/frond payload: uv4 = (branch2_weight, packedBranch2Dir)
								rotatePackedExtra(vert.uv4.y);
							}
							// Branch packed direction is used for all geometry types.
							rotatePackedExtra(vert.uv1.y);
						}
					}
				}
			} else {
				for (auto& vertice : vertices) {
					bake_transform(vertice);
				}
			}
            if (build_wind_vertices) {
                WindPrimTmp tmp;
                tmp.mesh_name = mesh_name + std::to_string(i);
                tmp.primitive = &primitive;
                tmp.indices = std::move(indices);
                tmp.wind_vertices = std::move(wind_vertices);
                tmp.has_speedtree_payload = has_speedtree_payload;
                wind_prims.emplace_back(std::move(tmp));
            } else {
			    meshes.emplace_back(
				    Mesh::builder()
					    .name(mesh_name + std::to_string(i))
					    .vertex_stream(
						    VertexStream::builder()
							    .attribute(0, VertexStream::Attribute::Position, sizeof(VertexNormalTangent), offsetof(VertexNormalTangent, position))
							    .attribute(1, VertexStream::Attribute::Normal, sizeof(VertexNormalTangent), offsetof(VertexNormalTangent, normal))
							    .attribute(2, VertexStream::Attribute::Tangent, sizeof(VertexNormalTangent), offsetof(VertexNormalTangent, tangent))
							    .attribute(3, VertexStream::Attribute::Uv, sizeof(VertexNormalTangent), offsetof(VertexNormalTangent, uv))
							    .vertices(vertices)
							    .indices(indices)
							    .usage(VertexStream::Usage::Static)
							    .draw(VertexStream::Draw::Triangles)
							    .batched(RendererSettings::geometry_batching_enabled)
							    .build()
					    )
					    .build()
			    );
			    mesh_materials.emplace_back(select_mesh_material(primitive));
            }
		} else {
			// skeletal mesh.
			std::vector<VertexBoneWeight> vertex_bone_weights;
			if (positions.size() != bone_weights.size()
			    || positions.size() != bone_indices.size()) {
				throw "mismatching count of vertex bone attributes: "
					+ std::to_string(positions.size()) + " positions, "
					+ std::to_string(bone_weights.size()) + " bone weights,"
					+ std::to_string(bone_indices.size()) + " bone indices";
			}

			vertex_bone_weights.reserve(positions.size());
			for (size_t i = 0; i < positions.size(); ++i) {
				vertex_bone_weights.emplace_back(VertexBoneWeight {bone_indices[i], bone_weights[i]}
				);
			}

			meshes.emplace_back(
				Mesh::builder()
					.name(mesh_name + std::to_string(i))
					.vertex_stream(
						VertexStream::builder()
							.attribute(0, VertexStream::Attribute::Position, sizeof(VertexNormalTangent), offsetof(VertexNormalTangent, position))
							.attribute(1, VertexStream::Attribute::Normal, sizeof(VertexNormalTangent), offsetof(VertexNormalTangent, normal))
							.attribute(2, VertexStream::Attribute::Tangent, sizeof(VertexNormalTangent), offsetof(VertexNormalTangent, tangent))
							.attribute(3, VertexStream::Attribute::Uv, sizeof(VertexNormalTangent), offsetof(VertexNormalTangent, uv))
							.attribute(4, VertexStream::Attribute::BoneIndices, sizeof(VertexBoneWeight), offsetof(VertexBoneWeight, bone_index))
							.attribute(5, VertexStream::Attribute::BoneWeights, sizeof(VertexBoneWeight), offsetof(VertexBoneWeight, weight))
							.vertices(vertices)
							.indices(indices)
							.bones(vertex_bone_weights)
							.usage(VertexStream::Usage::Static)
							.draw(VertexStream::Draw::Triangles)
							// Note: skeletal meshes don't support batching yet
							.build()
					)
					.build()
			);

			mesh_materials.emplace_back(select_mesh_material(primitive));
		}
	}

    // If we buffered wind primitives, stitch payload across all of them before building meshes.
    if (!wind_prims.empty()) {
        // Build final meshes/materials.
        for (auto& prim : wind_prims) {
            meshes.emplace_back(
                Mesh::builder()
                    .name(prim.mesh_name)
                    .vertex_stream(
                        VertexStream::builder()
                            .attribute(0, VertexStream::Attribute::Position, sizeof(VertexNormalTangentUv6), offsetof(VertexNormalTangentUv6, position))
                            .attribute(1, VertexStream::Attribute::Normal, sizeof(VertexNormalTangentUv6), offsetof(VertexNormalTangentUv6, normal))
                            .attribute(2, VertexStream::Attribute::Tangent, sizeof(VertexNormalTangentUv6), offsetof(VertexNormalTangentUv6, tangent))
                            .attribute(3, VertexStream::Attribute::Uv, sizeof(VertexNormalTangentUv6), offsetof(VertexNormalTangentUv6, uv0))
                            .attribute(4, VertexStream::Attribute::Uv1, sizeof(VertexNormalTangentUv6), offsetof(VertexNormalTangentUv6, uv1))
                            .attribute(5, VertexStream::Attribute::Uv2, sizeof(VertexNormalTangentUv6), offsetof(VertexNormalTangentUv6, uv2))
                            .attribute(6, VertexStream::Attribute::Uv3, sizeof(VertexNormalTangentUv6), offsetof(VertexNormalTangentUv6, uv3))
                            .attribute(7, VertexStream::Attribute::Uv4, sizeof(VertexNormalTangentUv6), offsetof(VertexNormalTangentUv6, uv4))
                            .attribute(8, VertexStream::Attribute::Uv5, sizeof(VertexNormalTangentUv6), offsetof(VertexNormalTangentUv6, uv5))
                            .vertices(prim.wind_vertices)
                            .indices(prim.indices)
                            .usage(VertexStream::Usage::Static)
                            .draw(VertexStream::Draw::Triangles)
                            .batched(RendererSettings::geometry_batching_enabled)
                            .build()
                    )
                    .build()
            );

            auto mat = prim.primitive ? select_mesh_material(*prim.primitive) : std::shared_ptr<ms::Material>{nullptr};
            if (flags.isPresent(ModelLoaderOption::Wind) && mat) {
                if (!prim.has_speedtree_payload) {
                    auto mat_copy = std::make_shared<ms::Material>(*mat);
                    mat_copy->setWindMode(1u);
                    mat = std::move(mat_copy);
                }
            }
            mesh_materials.emplace_back(std::move(mat));
        }
    }

	return {meshes, mesh_materials};
}

Animation loadAnimation(
	std::string anim_name,
	const cgltf_animation& animation,
	const std::unordered_map<const cgltf_node*, Bone*>& bone_map
) {
	std::unordered_map<cgltf_node*, AnimationNode> anim_nodes;
	double max_time = 0.f;

	for (size_t i = 0; i < animation.channels_count; ++i) {
		const cgltf_animation_channel& channel = animation.channels[i];
		if (channel.sampler == nullptr) {
			throw ModelLoadError {"one of animation " + anim_name + " channels has no sampler"};
		}

		const auto& sampler = *channel.sampler;
		if (sampler.input == nullptr || sampler.output == nullptr) {
			throw ModelLoadError {"sampler input or output accessor is missing"};
		}

		auto keyframe_times = copyFromAccessor<float>(*sampler.input);

		// Debug output
		std::cout << "Animation " << anim_name << " - Channel " << i << ":" << std::endl;
		std::cout << "  Input accessor: count=" << sampler.input->count << ", type=" << toString(sampler.input->type) << ", component_type=" << toString(sampler.input->component_type) << std::endl;
		std::cout << "  Output accessor: count=" << sampler.output->count << ", type=" << toString(sampler.output->type) << ", component_type=" << toString(sampler.output->component_type) << std::endl;
		std::cout << "  Keyframe times count: " << keyframe_times.size() << std::endl;

		if (bone_map.find(channel.target_node) == bone_map.end()) {
			throw ModelLoadError {"failed to find bone for this node"};
		}

		Bone& bone = *bone_map.at(channel.target_node);
		// Get or create animation node for this bone if missing.
		auto it = anim_nodes.emplace(channel.target_node, AnimationNode({}, {}, {}, bone));
		AnimationNode& anim_node = it.first->second;

		switch (sampler.interpolation) {
		case cgltf_interpolation_type_linear:
			// do nothing, only linear is supported anyway.
			break;

		case cgltf_interpolation_type_step:
			throw ModelLoadError {"step interpolation is not supported yet"};

		case cgltf_interpolation_type_cubic_spline:
			throw ModelLoadError {"cubic spline interpolation is not supported yet"};

		default:
			throw ModelLoadError {
				"invalid interpolation type " + std::to_string(sampler.interpolation)};
		}

		switch (channel.target_path) {
		case cgltf_animation_path_type_translation: {
			if (!anim_node.positions.empty()) {
				throw ModelLoadError {"multiple translation animation tracks for single bone"};
			}

			auto translations = copyFromAccessor<glm::vec3>(*sampler.output);
			if (translations.size() != keyframe_times.size()) {
				throw ModelLoadError {"translations count != keyframe_times count"};
			}

			for (size_t i = 0; i < translations.size(); ++i) {
				anim_node.positions.emplace_back(KeyFrame(translations[i], keyframe_times[i]));
				max_time = std::max(max_time, anim_node.positions[i].time);
			}

			break;
		}

		case cgltf_animation_path_type_rotation: {
			if (!anim_node.rotations.empty()) {
				throw ModelLoadError {"multiple rotation animation tracks for single bone"};
			}

			std::vector<std::array<float, 4>> rotations;
			if (sampler.output->normalized) {
				rotations = copyNormalizedQuaternionsFromAccessor(*sampler.output);
			} else {
				rotations = copyFromAccessor<std::array<float, 4>>(*sampler.output);
			}

			if (rotations.size() != keyframe_times.size()) {
				throw ModelLoadError {"rotations count != keyframe_times count"};
			}

			for (size_t i = 0; i < rotations.size(); ++i) {
				anim_node.rotations.emplace_back(
					KeyFrame(glm::normalize(toQuat(rotations[i])), keyframe_times[i])
				);
				max_time = std::max(max_time, anim_node.rotations[i].time);
			}
			break;
		}

		case cgltf_animation_path_type_scale: {
			if (!anim_node.scales.empty()) {
				throw ModelLoadError {"multiple scaling animation tracks for single bone"};
			}

			auto scales = copyFromAccessor<glm::vec3>(*sampler.output);

			if (scales.size() != keyframe_times.size()) {
				throw ModelLoadError {"scales count != keyframe_times count"};
			}

			for (size_t i = 0; i < scales.size(); ++i) {
				anim_node.scales.emplace_back(KeyFrame(scales[i], keyframe_times[i]));
				max_time = std::max(max_time, anim_node.scales[i].time);
			}
			break;
		}

		case cgltf_animation_path_type_weights:
			throw ModelLoadError {"morph weights not supported"};

		case cgltf_animation_path_type_invalid:
		case cgltf_animation_path_type_max_enum:
			throw ModelLoadError {"invalid animation path type"};
		}
	}

	std::vector<AnimationNode> flat_animation_nodes;
	for (auto& [_, anim_node] : anim_nodes) {
		flat_animation_nodes.emplace_back(std::move(anim_node));
	}

	return Animation {std::move(anim_name), max_time, 1.0f, std::move(flat_animation_nodes)};
}

static std::vector<Tree<uint32_t>> makeBoneIndiceTrees(
	const std::vector<const cgltf_node*>& roots,
	const std::unordered_map<const cgltf_node*, Bone*>& bone_map
) {
	std::vector<Tree<uint32_t>> result;

	std::function<void(Tree<uint32_t>&, const cgltf_node&, int)> dfs;

	dfs = [&](Tree<uint32_t>& tree, const cgltf_node& node, int depth) {
		for (size_t i = 0; i < node.children_count; ++i) {
			const auto& child_node = *node.children[i];
			const auto& child_bone = *bone_map.at(&child_node);

			auto& child_tree = tree.add(child_bone.index);
			dfs(child_tree, child_node, depth + 1);
		}
	};

	for (const auto* root : roots) {
		const auto& bone = bone_map.at(root);
		result.emplace_back(bone->index);
		dfs(result.back(), *root, 0);
	}

	return result;
}

static std::vector<const cgltf_node*> findRootNodes(const cgltf_data& src) {
	std::vector<const cgltf_node*> result;
	for (size_t i = 0; i < src.nodes_count; ++i) {
		const cgltf_node& node = src.nodes[i];
		if (node.parent == nullptr) {
			result.emplace_back(&node);
		}
	}

	return result;
}

// Generate a unique material name if it's missing.
static std::string generateMaterialName(const std::string& model_name, size_t material_index) {
	return model_name + "_material" + std::to_string(material_index);
}

static std::shared_ptr<ms::Material> loadMaterial(
	Assets& assets,
	const InstanceTypes& instance_types,
	const fs::path& base_path,
	const cgltf_material& material,
	const std::string& model_name,
	size_t material_index,
    const ModelLoaderFlags& model_flags
) {
	ms::Material::Builder builder = ms::Material::builder();
	const auto material_name = model_name + (material.name
		? std::string(material.name)
		: generateMaterialName(model_name, material_index));

	builder
		.name(material_name)
		.shading(material.unlit ? ms::Shading::Unlit : ms::Shading::Lit)
		// Force two-sided for glTF imports by default.
		// This matches typical vegetation expectations (leaf cards, thin geometry) and
		// avoids confusing "disappearing" surfaces when authoring/export pipelines omit
		// doubleSided flags.
		.two_sided(true);

	switch (material.alpha_mode) {
	case cgltf_alpha_mode_opaque:
		builder.blending(ms::Blending::Opaque);
		break;
	case cgltf_alpha_mode_blend:
		builder.blending(ms::Blending::Translucent);
		break;
    case cgltf_alpha_mode_mask:
        builder.blending(ms::Blending::Opaque);
        //TODO: add to material built-in
        builder.custom("alpha_cutoff", material.alpha_cutoff);
        builder.fragment(
            "#if defined (ENGINE_MATERIAL_DIFFUSE_TEXTURE)\n"
            "    if (mctx.diffuse.a <= alpha_cutoff) discard;\n"
            "#else\n"
            "    if (computeMaterialColor(mctx).a <= alpha_cutoff) discard;\n"
            "#endif\n"
        );
        break;
	default:
		throw ModelLoadError {"alpha mode " + std::to_string(material.alpha_mode) + " not supported"};
	}

    // Optional: enable wind on imported materials at construction time.
    // We cannot "add" properties later because material UBO layout is created once at build.
    if (model_flags.isPresent(ModelLoaderOption::Wind)) {
        // Mode 2: SpeedTree8 payload (extra TEXCOORD sets) + SpeedTreeWind preset uniforms.
        builder.wind(true).wind_mode(2u).wind_intensity(1.0f).wind_frequency(1.0f);
    }

//	if (!material.has_pbr_metallic_roughness) {
//		throw ModelLoadError {"missing PBR metallic roughness"};
//	}

	auto bytesFromBase64 = [](const char* cstr) -> std::vector<uint8_t> {    
	    auto decode_base64_char = [](char c) -> uint8_t {
	        if (c >= 'A' && c <= 'Z') return c - 'A';
	        if (c >= 'a' && c <= 'z') return c - 'a' + 26;
	        if (c >= '0' && c <= '9') return c - '0' + 52;
	        if (c == '+') return 62;
	        if (c == '/') return 63;
	        if (c == '=') return 64;
	        throw ModelLoadError(std::string("Invalid base64 character ") + c + " aka " + std::to_string(static_cast<int>(c)));
	    };
    
	    std::vector<uint8_t> output;
    
	    while (*cstr) {
	        uint8_t sextet_a = *cstr ? decode_base64_char(*cstr++) : 0;
	        uint8_t sextet_b = *cstr ? decode_base64_char(*cstr++) : 0;
	        uint8_t sextet_c = *cstr ? decode_base64_char(*cstr++) : 0;
	        uint8_t sextet_d = *cstr ? decode_base64_char(*cstr++) : 0;

	        uint32_t triple = (static_cast<uint32_t>(sextet_a) << 18) |
	                          (static_cast<uint32_t>(sextet_b) << 12) |
	                          (static_cast<uint32_t>(sextet_c) << 6) |
	                          static_cast<uint32_t>(sextet_d);
	        
	        if (sextet_c == 64) {
	            output.push_back(static_cast<uint8_t>((triple >> 16) & 0xFF));
	        } else if (sextet_d == 64) {
	            output.push_back(static_cast<uint8_t>((triple >> 16) & 0xFF));
	            output.push_back(static_cast<uint8_t>((triple >> 8) & 0xFF));
	        } else {
	            output.push_back(static_cast<uint8_t>((triple >> 16) & 0xFF));
	            output.push_back(static_cast<uint8_t>((triple >> 8) & 0xFF));
	            output.push_back(static_cast<uint8_t>(triple & 0xFF));
	        }
	    }
	    
	    return output;
	};

	auto loadTextureFrom = [&](cgltf_texture& tex, std::string name, TextureLoaderFlags flags) -> std::optional<std::shared_ptr<Texture>> {
		if (!tex.image) {
			return std::nullopt;
		}
		auto& img = *tex.image;

		if (!tex.sampler) {
			return std::nullopt;
		}
		auto& sampler = *tex.sampler;

		constexpr int GLTF_SAMPLER_FILTER_NEAREST = 9728;
		constexpr int GLTF_SAMPLER_FILTER_LINEAR = 9279;
		constexpr int GLTF_SAMPLER_FILTER_NEAREST_MIPMAP_NEAREST = 9984;
		constexpr int GLTF_SAMPLER_FILTER_LINEAR_MIPMAP_NEAREST = 9985;
		constexpr int GLTF_SAMPLER_FILTER_NEAREST_MIPMAP_LINEAR = 9986;
		constexpr int GLTF_SAMPLER_FILTER_LINEAR_MIPMAP_LINEAR = 9987;

		auto limitlessTexLoaderFilterModeFrom = [](cgltf_int filter_mode) -> std::optional<TextureLoaderFlags::Filter> {
			switch (filter_mode) {
				case GLTF_SAMPLER_FILTER_NEAREST:
				case GLTF_SAMPLER_FILTER_NEAREST_MIPMAP_NEAREST:
				case GLTF_SAMPLER_FILTER_NEAREST_MIPMAP_LINEAR:
					return TextureLoaderFlags::Filter::Nearest;

				case GLTF_SAMPLER_FILTER_LINEAR:
				case GLTF_SAMPLER_FILTER_LINEAR_MIPMAP_NEAREST:
				case GLTF_SAMPLER_FILTER_LINEAR_MIPMAP_LINEAR:
					return TextureLoaderFlags::Filter::Linear;

				default:
					return std::nullopt;
			}
		};

		auto mag_filter_mode = limitlessTexLoaderFilterModeFrom(sampler.mag_filter);
		if (mag_filter_mode) {
			flags.filter = *mag_filter_mode;
		}

		// TODO: support separate magnification/minification filtering in the engine.
		auto min_filter_mode = limitlessTexLoaderFilterModeFrom(sampler.min_filter);
		if (min_filter_mode) {
			flags.filter = *min_filter_mode;
		}

		auto hasMipmap = [](cgltf_int filter_mode) -> bool {
			switch (filter_mode) {
				case GLTF_SAMPLER_FILTER_NEAREST_MIPMAP_NEAREST:
				case GLTF_SAMPLER_FILTER_LINEAR_MIPMAP_NEAREST:
				case GLTF_SAMPLER_FILTER_NEAREST_MIPMAP_LINEAR:
				case GLTF_SAMPLER_FILTER_LINEAR_MIPMAP_LINEAR:
					return true;

				default:
					return false;
			}
		};

		flags.mipmap = hasMipmap(sampler.min_filter);

		auto limitlessTexWrapModeFrom = [](cgltf_int wrapping_mode_enum) -> std::optional<Texture::Wrap> {
			constexpr int GLTF_SAMPLER_WRAP_CLAMP_TO_EDGE = 33071;
			constexpr int GLTF_SAMPLER_WRAP_MIRRORED_REPEAT = 33648;
			constexpr int GLTF_SAMPLER_WRAP_REPEAT = 10497;

			switch (wrapping_mode_enum) {
				case GLTF_SAMPLER_WRAP_CLAMP_TO_EDGE:
					return Texture::Wrap::ClampToEdge;

				case GLTF_SAMPLER_WRAP_MIRRORED_REPEAT:
					return Texture::Wrap::MirroredRepeat;

				case GLTF_SAMPLER_WRAP_REPEAT:
					return Texture::Wrap::Repeat;

				default:
					return std::nullopt;
			}
		};

		auto wrap_s_mode = limitlessTexWrapModeFrom(sampler.wrap_s);
		if (wrap_s_mode) {
			flags.wrapping = *wrap_s_mode;
		}

		// TODO: support separate S/T wrapping mode in the engine.
		auto wrap_t_mode = limitlessTexWrapModeFrom(sampler.wrap_t);
		if (wrap_t_mode) {
			flags.wrapping = *wrap_t_mode;
		}

		if (img.uri == nullptr) {
			if (!img.buffer_view) {
				throw ModelLoadError {"texture has no uri and no buffer view"};
			}

			return TextureLoader::load(
				assets,
				name,
				cgltf_buffer_view_data(img.buffer_view),
				img.buffer_view->size,
				flags
			);

		} else {
			if (strncmp(img.uri, "data:", 5) == 0) {
				const char* comma = strchr(img.uri, ',');

				if (comma && comma - img.uri >= 7 && strncmp(comma - 7, ";base64", 7) == 0) {
					auto buffer = bytesFromBase64(comma + 1);

					return TextureLoader::load(
						assets,
						name,
						buffer.data(),
						buffer.size(),
						flags
					);
				} else {
					throw ModelLoadError {"unknown data uri"};
				}

			} else {
				const auto path = base_path / fs::path(img.uri);
				return TextureLoader::load(assets, path, flags);
			}
		}
	};

	const auto& pbr_mr   = material.pbr_metallic_roughness;
	auto* base_color_tex = pbr_mr.base_color_texture.texture;

    // -----------------------------------------------------------------------------------------
    // Fallback: external textures next to the asset (common SpeedTree->Blender->glTF workflow)
    //
    // Your current Blender export produced a glTF with materials but WITHOUT images/textures.
    // In that case, the engine has no way to render leaf alpha and everything becomes quads.
    //
    // To make iteration painless, if the glTF material has no texture bindings, we try to load
    // textures by convention from the parent directory of the glTF folder.
    //
    // Example layout (your repo):
    //   assets/tree/gltf/Untitled.gltf
    //   assets/tree/Conifer_Color.png
    //   assets/tree/Conifer_Normal.png
    //   assets/tree/Bark_Color.png
    //   assets/tree/Bark_Normal.png
    //
    // This is a best-effort heuristic, not a generic glTF feature.
    // -----------------------------------------------------------------------------------------
    auto toLower = [](std::string s) {
        for (auto& c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        return s;
    };

    const std::string mat_name = material.name ? std::string(material.name) : "";
    const std::string mat_lower = toLower(mat_name);
    const bool looks_like_billboard = mat_lower.find("billboard") != std::string::npos;
    const bool looks_like_conifer = mat_lower.find("conifer") != std::string::npos || mat_lower.find("leaf") != std::string::npos;
    const bool looks_like_bark = mat_lower.find("bark") != std::string::npos || mat_lower.find("trunk") != std::string::npos;

    const bool has_any_gltf_textures =
        (base_color_tex != nullptr) ||
        (material.normal_texture.texture && material.normal_texture.texture->image) ||
        (material.emissive_texture.texture && material.emissive_texture.texture->image) ||
        (pbr_mr.metallic_roughness_texture.texture && pbr_mr.metallic_roughness_texture.texture->image) ||
        (material.occlusion_texture.texture && material.occlusion_texture.texture->image);

    auto tryLoadExternal = [&](const fs::path& path, const TextureLoaderFlags& flags) -> std::optional<std::shared_ptr<Texture>> {
        try {
            if (!fs::exists(path)) {
                return std::nullopt;
            }
            return TextureLoader::load(assets, path, flags);
        } catch (...) {
            return std::nullopt;
        }
    };

    if (!has_any_gltf_textures) {
        const auto parent_dir = base_path.parent_path();

        // Choose naming prefix by material name.
        std::string prefix;
        if (looks_like_billboard) {
            // Prefer exact material-name prefix (SpeedTree exports often use:
            //   Material: <Prefix>_Mat
            //   Textures: <Prefix>_{Color,Normal,SS}.png
            // Example in this repo:
            //   Conifer_billboard_Billboard_Mat
            //   Conifer_billboard_Billboard_Color.png
            prefix = mat_name;
            auto lower = toLower(prefix);
            // Strip common suffixes.
            if (lower.size() >= 4 && lower.rfind("_mat") == lower.size() - 4) {
                prefix.resize(prefix.size() - 4);
            } else if (lower.size() >= 8 && lower.rfind("_material") == lower.size() - 9) {
                prefix.resize(prefix.size() - 9);
            }
        } else if (looks_like_conifer) {
            prefix = "Conifer";
        } else if (looks_like_bark) {
            prefix = "Bark";
        } else {
            prefix = "";
        }

        if (!prefix.empty()) {
            // For foliage alpha-cutout textures:
            // - Mipmaps help reduce shimmering, but hard discard causes coverage loss.
            // We'll keep mipmaps enabled and use a dithered discard in the shader snippet instead.
            auto flags_srgb = TextureLoaderFlags(model_flags.base_tex_flags).withSpace(TextureLoaderFlags::Space::sRGB);
            const auto flags_lin = TextureLoaderFlags(model_flags.base_tex_flags).withSpace(TextureLoaderFlags::Space::Linear);

            const float conifer_cutoff = 0.33f;
            const float billboard_cutoff = 0.33f;

            // If we are going to use alpha-clip for foliage/billboards, enable coverage-preserving mipmaps
            // on the base color texture so distant leaves don't disappear.
            const bool conifer_forced_cutout = looks_like_conifer && material.alpha_mode == cgltf_alpha_mode_opaque;
            const bool billboard_forced_cutout = looks_like_billboard; // billboards are always cutout in practice
            if (conifer_forced_cutout) flags_srgb = flags_srgb.withPreserveAlphaCoverage(conifer_cutoff);
            if (billboard_forced_cutout) flags_srgb = flags_srgb.withPreserveAlphaCoverage(billboard_cutoff);

            const auto billboard_color_path = parent_dir / (prefix + "_Color.png");
            const auto billboard_normal_path = parent_dir / (prefix + "_Normal.png");
            const auto billboard_ss_path = parent_dir / (prefix + "_SS.png");

            if (looks_like_billboard) {
                std::cerr << "[billboard] material='" << mat_name << "' prefix='" << prefix << "' parent_dir='" << parent_dir.string() << "'\n";
            }

            if (auto tex = tryLoadExternal(billboard_color_path, flags_srgb)) {
                builder.diffuse(*tex);
                // Preserve authored baseColorFactor (usually 1,1,1,1). If it's unset, default white.
                builder.color(toVec4(pbr_mr.base_color_factor));
                if (looks_like_billboard) {
                    std::cerr << "[billboard] loaded baseColor: " << billboard_color_path.string() << "\n";
                }
            } else {
                // No external texture; fall back to glTF baseColorFactor.
                builder.color(toVec4(pbr_mr.base_color_factor));
                if (looks_like_billboard) {
                    std::cerr << "[billboard] MISSING baseColor: " << billboard_color_path.string() << "\n";
                }
            }

            if (auto tex = tryLoadExternal(billboard_normal_path, flags_lin)) {
                builder.normal(*tex);
                if (looks_like_billboard) {
                    std::cerr << "[billboard] loaded normal: " << billboard_normal_path.string() << "\n";
                }
            } else if (looks_like_billboard) {
                std::cerr << "[billboard] (optional) missing normal: " << billboard_normal_path.string() << "\n";
            }

            // Optional SpeedTree-ish "SS" mask/color (not currently used by our default shader model).
            // Keep it here for future hookup without breaking content.
            if (looks_like_billboard) {
                if (fs::exists(billboard_ss_path)) {
                    std::cerr << "[billboard] found SS map: " << billboard_ss_path.string() << " (not yet wired)\n";
                } else {
                    std::cerr << "[billboard] (optional) missing SS map: " << billboard_ss_path.string() << "\n";
                }
            }
            (void)tryLoadExternal(billboard_ss_path, flags_lin);

            // Leaf alpha cutout: if glTF didn't specify alphaMode, force a reasonable mask for foliage.
            // This makes leaf quads readable for wind tuning.
            if ((looks_like_conifer && material.alpha_mode == cgltf_alpha_mode_opaque) || looks_like_billboard) {
                // Cutout works best as opaque + discard, and leaves are typically two-sided.
                builder.two_sided(true);
                builder.custom("alpha_cutoff", looks_like_billboard ? billboard_cutoff : conifer_cutoff);
                // Normal alpha-clip. For better edges without dithering, enable alpha-to-coverage (A2C)
                // via render state when MSAA is available.
                builder.custom("alpha_to_coverage", 1u);
                // For billboards we also apply a facing-fade to smoothly select the best card.
                if (looks_like_billboard) {
                    builder.custom("st_bb_fade_power", 2.0f);
                    builder.fragment(
                        "#if defined(ENGINE_VERTEX_NORMAL)\n"
                        "    vec3 _stbbV = normalize(getCameraPosition() - getVertexPosition());\n"
                        "    vec3 _stbbN = normalize(mctx.vertex_normal);\n"
                        "#  if defined(ENGINE_MATERIAL_TWO_SIDED)\n"
                        "    _stbbN = gl_FrontFacing ? _stbbN : -_stbbN;\n"
                        "#  endif\n"
                        "    float _stbbF = clamp(dot(_stbbN, _stbbV), 0.0, 1.0);\n"
                        "    _stbbF = pow(_stbbF, st_bb_fade_power);\n"
                        "#  if defined (ENGINE_MATERIAL_DIFFUSE_TEXTURE)\n"
                        "    mctx.diffuse.a *= _stbbF;\n"
                        "    if (mctx.diffuse.a <= alpha_cutoff) discard;\n"
                        "#  else\n"
                        "    if (computeMaterialColor(mctx).a * _stbbF <= alpha_cutoff) discard;\n"
                        "#  endif\n"
                        "#endif\n"
                    );
                    std::cerr << "[billboard] enabled facing-fade + cutout (cutoff=" << (looks_like_billboard ? billboard_cutoff : conifer_cutoff) << ")\n";
                } else {
                    builder.fragment(
                        "#if defined (ENGINE_MATERIAL_DIFFUSE_TEXTURE)\n"
                        "    if (mctx.diffuse.a <= alpha_cutoff) discard;\n"
                        "#else\n"
                        "    if (computeMaterialColor(mctx).a <= alpha_cutoff) discard;\n"
                        "#endif\n"
                    );
                }
            }
        } else {
            // Unknown material name; at least keep the base color factor.
            builder.color(toVec4(pbr_mr.base_color_factor));
        }

        // Skip the normal glTF texture loading block below (since there are no textures anyway).
        goto finish_material;
    }

	if (base_color_tex == nullptr) {
		// no texture, this means that base color factor is the color.
		builder.color(toVec4(pbr_mr.base_color_factor));

	} else {
		if (base_color_tex->image == nullptr) {
			throw ModelLoadError {"material has no base color texture image despite having PBR"};
		}

		// The base color texture MUST contain 8-bit values encoded with the
		// sRGB opto-electronic transfer function.
		const auto flags = TextureLoaderFlags(model_flags.base_tex_flags)
			.withSpace(TextureLoaderFlags::Space::sRGB);

		builder.diffuse(*loadTextureFrom(*base_color_tex, material_name + "_base_color", flags));
		builder.color(toVec4(pbr_mr.base_color_factor));
	}

	// TODO: load as metallic-roughness texture.
	// auto* mr_tex = pbr_mr.metallic_roughness_texture.texture;
	// if (mr_tex && mr_tex->image) {
	// 	// These values MUST be encoded with a linear transfer function.
	// 	const auto flags = TextureLoaderFlags(TextureLoaderFlags::Space::Linear);

	// 	builder.orm(*loadTextureFrom(*mr_tex, material_name + "_orm", flags));
	// 	builder.metallic(pbr_mr.metallic_factor);
	// 	builder.roughness(pbr_mr.roughness_factor);
	// }

	auto* normal_tex = material.normal_texture.texture;
	if (normal_tex && normal_tex->image) {
		// These values MUST be encoded with a linear transfer function.
		const auto flags = TextureLoaderFlags(model_flags.base_tex_flags)
			.withSpace(TextureLoaderFlags::Space::Linear);

		builder.normal(*loadTextureFrom(*normal_tex, material_name + "_normal", flags));
	}

    // Billboard material behavior (even if textures are embedded in glTF):
    // apply alpha cutout + facing fade (SpeedTree-style card selection).
    if (looks_like_billboard) {
        builder.two_sided(true);
        builder.custom("alpha_cutoff", 0.33f);
        builder.custom("alpha_to_coverage", 1u);
        builder.custom("st_bb_fade_power", 2.0f);
        builder.fragment(
            "#if defined(ENGINE_VERTEX_NORMAL)\n"
            "    vec3 _stbbV = normalize(getCameraPosition() - getVertexPosition());\n"
            "    vec3 _stbbN = normalize(mctx.vertex_normal);\n"
            "#  if defined(ENGINE_MATERIAL_TWO_SIDED)\n"
            "    _stbbN = gl_FrontFacing ? _stbbN : -_stbbN;\n"
            "#  endif\n"
            "    float _stbbF = clamp(dot(_stbbN, _stbbV), 0.0, 1.0);\n"
            "    _stbbF = pow(_stbbF, st_bb_fade_power);\n"
            "#  if defined (ENGINE_MATERIAL_DIFFUSE_TEXTURE)\n"
            "    mctx.diffuse.a *= _stbbF;\n"
            "    if (mctx.diffuse.a <= alpha_cutoff) discard;\n"
            "#  else\n"
            "    if (computeMaterialColor(mctx).a * _stbbF <= alpha_cutoff) discard;\n"
            "#  endif\n"
            "#endif\n"
        );
        std::cerr << "[billboard] (embedded textures) enabled facing-fade + cutout for material='" << mat_name << "'\n";
    }

	if (material.has_ior) {
        builder.refraction(true);
		builder.ior(material.ior.ior);
	}

	auto* emissive_tex = material.emissive_texture.texture;
	if (emissive_tex && emissive_tex->image) {
		// This texture contains RGB components encoded with the sRGB transfer
		// function
		const auto flags = TextureLoaderFlags(model_flags.base_tex_flags)
			.withSpace(TextureLoaderFlags::Space::sRGB);

		builder.emissive_mask(*loadTextureFrom(*emissive_tex, material_name + "_emissive_mask", flags));
	}

	glm::vec3 emissive_rgb = toVec3(material.emissive_factor);
	if (emissive_rgb != glm::vec3(0.f)) {
		glm::vec3 emissive_color(toVec3(material.emissive_factor));
		if (material.has_emissive_strength) {
            emissive_color *= material.emissive_strength.emissive_strength;
		}

		builder.emissive_color(emissive_color);
	}

finish_material:
	return builder.models(instance_types).build(assets);
}

static std::vector<std::shared_ptr<ms::Material>> loadMaterials(
	const std::string& model_name,
	Assets& assets,
	const InstanceTypes& instance_types,
	const fs::path& path,
	const cgltf_data& src,
    const ModelLoaderFlags& flags
) {
	std::vector<std::shared_ptr<ms::Material>> materials;

	for (size_t i = 0; i < src.materials_count; ++i) {
		materials.emplace_back(loadMaterial(
			assets, instance_types, path.parent_path(), src.materials[i], model_name, i, flags
		));
	}

	return materials;
}

// Generate material for meshes that do not have one.
static std::shared_ptr<ms::Material> makeDummyMaterial(
	Assets& assets, const std::string& model_name, const InstanceTypes& instance_types
) {
	return ms::Material::builder()
	    .name(generateMaterialName(model_name, 0))
	    .two_sided(true)
	    .shading(ms::Shading::Unlit)
	    .color({1.f, 0.f, 1.f, 1.f})
	    .models(instance_types)
	    .build(assets);
}

// Fill in missing mesh materials with a dummy one if required.
static void fixMissingMaterials(
	std::vector<std::shared_ptr<ms::Material>>& mesh_materials,
	Assets& assets,
	const std::string& model_name,
	const InstanceTypes& instance_types
) {
	std::shared_ptr<ms::Material> dummy_material;

	// Lazily create dummy material if required.
	auto use_dummy_material = [&]() {
		if (!dummy_material) {
			dummy_material = makeDummyMaterial(assets, model_name, instance_types);
		}
		return dummy_material;
	};

	for (auto& maybe_material : mesh_materials) {
		if (!maybe_material) {
			maybe_material = use_dummy_material();
		}
	}
}

static std::shared_ptr<Model> loadSkeletalModel(
	Assets& assets,
	const fs::path& path,
	const cgltf_data& src,
	const std::string& model_name,
	const ModelLoaderFlags& flags
) {
	std::vector<Bone> bones;
	std::unordered_map<const cgltf_node*, size_t> bone_indice_map;

	auto root_nodes = findRootNodes(src);

	for (size_t i = 0; i < src.nodes_count; ++i) {
		const cgltf_node& node = src.nodes[i];
		auto bone_name =
			node.name ? std::string(node.name) : model_name + "_bone" + std::to_string(i);
		bones.emplace_back(Bone(i, bone_name, getNodeMatrix(node), glm::mat4(1.f)));
		bone_indice_map.emplace(&node, i);

		auto& bone = bones.back();
		if (node.has_translation) {
			bone.position = toVec3(node.translation);
		}
		if (node.has_rotation) {
			bone.rotation = toQuat(node.rotation);
		}
		if (node.has_scale) {
			bone.scale = toVec3(node.scale);
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
			copyFromAccessor<std::array<float, 16>>(*skin.inverse_bind_matrices);

		if (inverse_bind_matrices.size() != skin.joints_count) {
			throw ModelLoadError {"mismatched number of skin joints and inverse bind matrices"};
		}

		for (size_t j = 0; j < skin.joints_count; ++j) {
			const cgltf_node& joint_node = *skin.joints[j];
			auto* bone                   = bone_map.at(&joint_node);
			bone->offset_matrix          = toMat4(inverse_bind_matrices[j]);
			bone->joint_index            = j;
		}
	}

	std::vector<Animation> animations;
	for (size_t i = 0; i < src.animations_count; ++i) {
		auto anim_name = src.animations[i].name ? std::string(src.animations[i].name)
		                                        : "anim" + std::to_string(i);
		animations.emplace_back(loadAnimation(std::move(anim_name), src.animations[i], bone_map));
	}

	auto bone_indices_tree = makeBoneIndiceTrees(root_nodes, bone_map);

	InstanceTypes instance_types = flags.additional_instance_types;
	instance_types.emplace(InstanceType::Skeletal);
	auto loaded_materials = loadMaterials(model_name, assets, instance_types, path, src, flags);

	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<std::shared_ptr<ms::Material>> mesh_materials;

	for (size_t i = 0; i < src.nodes_count; ++i) {
		const cgltf_node& node = src.nodes[i];

		if (node.mesh) {
			auto [more_meshes, more_mesh_materials] = loadMeshes(
				node, *node.mesh, node.skin, model_name, meshes.size(), loaded_materials, src, flags
			);
			meshes.insert(meshes.end(), more_meshes.begin(), more_meshes.end());
			mesh_materials.insert(
				mesh_materials.end(), more_mesh_materials.begin(), more_mesh_materials.end()
			);
		}
	}

	fixMissingMaterials(mesh_materials, assets, model_name, instance_types);

	std::unordered_map<std::string, uint32_t> bone_indices_map;
	for (size_t i = 0; i < bones.size(); ++i) {
		bone_indices_map.emplace(bones[i].name, i);
	}

	return Model::builder()
		.name(model_name)
		.meshes(meshes)
		.materials(mesh_materials)
		.bones(std::move(bones))
		.bone_map(std::move(bone_indices_map))
		.skeletons(std::move(bone_indices_tree))
		.animations(std::move(animations))
		.build(assets);
}

static std::shared_ptr<Model> loadPlainModel(
	Assets& assets, const fs::path& path, const cgltf_data& src, const std::string& model_name, const ModelLoaderFlags& flags
) {
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<std::shared_ptr<ms::Material>> mesh_materials;
	InstanceTypes instance_types = flags.additional_instance_types;
	instance_types.emplace(InstanceType::Model);

	auto loaded_materials = loadMaterials(model_name, assets, instance_types, path, src, flags);

	for (size_t i = 0; i < src.nodes_count; ++i) {
		const cgltf_node& node = src.nodes[i];

		if (node.mesh) {
			auto [more_meshes, more_mesh_materials] = loadMeshes(
				node, *node.mesh, nullptr, model_name, meshes.size(), loaded_materials, src, flags
			);
			meshes.insert(meshes.end(), more_meshes.begin(), more_meshes.end());
			mesh_materials.insert(
				mesh_materials.end(), more_mesh_materials.begin(), more_mesh_materials.end()
			);
		}
	}

	fixMissingMaterials(mesh_materials, assets, model_name, instance_types);

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

		return Model::builder()
			.name(model_name)
			.selection(LodSelection::CameraDistance)
			.transition(LodTransition::None)
			.add_lods(lod_models, distances)
			.build(assets);
	}

	return Model::builder()
		.name(model_name)
		.meshes(std::move(meshes))
		.materials(std::move(mesh_materials))
		.build(assets);
	// return new Model(std::move(meshes), std::move(mesh_materials), model_name);
}

static std::shared_ptr<Model>
loadModel(Assets& assets, const fs::path& path, const cgltf_data& src, const ModelLoaderFlags& flags) {
	auto model_name = path.stem().string();

	if (src.skins_count > 0) {
		return std::shared_ptr<Model>(loadSkeletalModel(assets, path, src, model_name, flags));
	} else {
		return std::shared_ptr<Model>(loadPlainModel(assets, path, src, model_name, flags));
	}
}

std::shared_ptr<Model>
GltfModelLoader::loadModel(Assets& assets, const fs::path& path, const ModelLoaderFlags& flags) {
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
		throw ModelLoadError {
			"failed to parse GLTF model file " + path.string() + ": "
			+ std::to_string(static_cast<int>(gltf))};
	}

	auto result = cgltf_load_buffers(&opts, out_data, path_str.c_str());
	if (result != cgltf_result_success) {
		throw ModelLoadError {
			"failed to load buffers: " + std::to_string(static_cast<int>(result))};
	}

	if (out_data->scenes == nullptr) {
		throw ModelLoadError {"no scene"};
	}

	// Ensure node.parent pointers are set so we can compute full node world transforms.
	buildNodeParents(*out_data);

	auto base_model = ::loadModel(assets, path, *out_data, flags);

    // -----------------------------------------------------------------------------------------
    // SpeedTree billboard LOD auto-attachment (Variant A)
    //
    // If a sibling "*_billboard.gltf" (or any single "*billboard*.gltf") exists next to the main glTF,
    // we load it as an additional distant LOD.
    //
    // This lets us keep "LOD0 wind model" in one glTF and the billboard atlas/card geometry in another,
    // while still treating it as a single Model in-engine.
    // -----------------------------------------------------------------------------------------
    auto toLower = [](std::string s) {
        for (auto& c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        return s;
    };
    const std::string stem_lower = toLower(path.stem().string());
    if (stem_lower.find("billboard") != std::string::npos) {
        return base_model;
    }

    // Only for plain models (billboard LOD for skeletal isn't supported).
    if (out_data->skins_count > 0) {
        return base_model;
    }

    const fs::path dir = path.parent_path();
    const std::string stem = path.stem().string();

    std::optional<fs::path> billboard_path;
    // Preferred: <stem>_billboard.gltf
    {
        fs::path p = dir / (stem + "_billboard.gltf");
        if (fs::exists(p)) billboard_path = p;
    }
    // Fallback: any single "*billboard*.gltf" in the same directory
    if (!billboard_path) {
        std::vector<fs::path> candidates;
        try {
            for (const auto& e : fs::directory_iterator(dir)) {
                if (!e.is_regular_file()) continue;
                auto p = e.path();
                const auto ext = toLower(p.extension().string());
                if (ext != ".gltf") continue;
                if (p == path) continue;
                const auto name = toLower(p.filename().string());
                if (name.find("billboard") != std::string::npos) {
                    candidates.emplace_back(p);
                }
            }
        } catch (...) {
            // ignore filesystem errors
        }
        if (candidates.size() == 1) {
            billboard_path = candidates.front();
        }
    }

    if (!billboard_path) {
        // Debug aid: helps confirm why billboard isn't showing up.
        // We only print for tree-ish assets (wind option enabled or common names) to avoid spam.
        if (flags.isPresent(ModelLoaderOption::Wind) || stem_lower.find("tree") != std::string::npos || stem_lower.find("conifer") != std::string::npos) {
            std::cerr << "[billboard] no billboard LOD found next to '" << path.string() << "'\n";
        }
        return base_model;
    }

    // Load billboard LOD without wind-payload processing.
    ModelLoaderFlags bb_flags = flags;
    bb_flags.options.erase(ModelLoaderOption::Wind);
    std::cerr << "[billboard] attaching billboard LOD: '" << billboard_path->string() << "' -> '" << path.string() << "'\n";
    auto billboard_model = GltfModelLoader::loadModel(assets, *billboard_path, bb_flags);

    // Build distance thresholds based on LOD0 bounding box size.
    const auto bb = base_model->getBoundingBox();
    const float radius = std::max(0.001f, 0.5f * glm::length(bb.size));
    std::vector<float> distances { radius * 10.0f, std::numeric_limits<float>::infinity() };

    std::vector<std::shared_ptr<Model>> lods { base_model, billboard_model };
    return Model::builder()
        .name(path.stem().string())
        .selection(LodSelection::CameraDistance)
        .transition(LodTransition::None)
        .add_lods(lods, distances)
        .build(assets);
}
