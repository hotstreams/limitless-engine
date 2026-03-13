#pragma once

#include <filesystem>
#include <limitless/models/model.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace Limitless {
	class AbstractModel;
	class TextureLoaderFlags;

	enum class ModelLoaderOption {
		FlipUV,
		GenerateUniqueMeshNames,
		FlipWindingOrder,
		NoMaterials,
		GlobalScale
	};

	struct ModelLoadError : public std::runtime_error {
		explicit ModelLoadError(const std::string& msg) noexcept
			: std::runtime_error(msg) {}
	};

	struct LodOptions {
		float target_error {0.01f};
		float simplification_factor {0.5f};
		bool forced {false};
	};

	class ModelLoaderFlags {
	public:
		std::set<ModelLoaderOption> options;
		float scale_factor {1.0f};
		InstanceTypes additional_instance_types;
		TextureLoaderFlags base_tex_flags;
		std::unordered_map<std::string, std::string> texture_uri_replacements;
		LodOptions lod_options;

		auto isPresent(ModelLoaderOption option) const { return options.count(option) != 0; }

		ModelLoaderFlags& additionalInstanceTypes(InstanceTypes _additional_instance_types) {
			additional_instance_types = _additional_instance_types;
			return *this;
		}

		ModelLoaderFlags& instanced() {
			additional_instance_types.emplace(InstanceType::Instanced);
			return *this;
		}

		ModelLoaderFlags& baseTextureLoaderFlags(TextureLoaderFlags tex_flags) {
			base_tex_flags = std::move(tex_flags);
			return *this;
		}

		ModelLoaderFlags& textureUriReplacements(std::unordered_map<std::string, std::string> uri_replacements) {
			texture_uri_replacements = std::move(uri_replacements);
			return *this;
		}

		ModelLoaderFlags& lodOptions(LodOptions options) {
			lod_options = std::move(options);
			return *this;
		}
	};

	class GltfModelLoader {
	public:
		// Load a GLTF 3D model from given file.
		// Will also attempt to load materials referenced in model definition.
		// Returns a shared pointer to resulting model on success.
		// Provided assets are modified.
		// On failure, a ModelLoadError exception is thrown.
		static std::shared_ptr<AbstractModel> loadModel(
			Assets& assets,
			const fs::path& path,
			const ModelLoaderFlags& flags
		);

		// Load a GLTF 3D model variant of given file with variant texture URI replacements in flags.
		// This is more efficient as it reuses base model meshes.
		// Requires loading base model first via loadModel.
		static std::vector<std::shared_ptr<Limitless::ms::Material>> loadModelVariant(
			Assets& assets,
			const fs::path& path,
			std::string variant_name,
			const ModelLoaderFlags& flags
		);

		static std::shared_ptr<AbstractMesh> simplifyMesh(
			const AbstractMesh& mesh,
			const LodOptions& options,
			const std::vector<unsigned char>& vertex_locks = {}
		);
	};
}
