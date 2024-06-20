#pragma once

#include <filesystem>
#include <limitless/models/model.hpp>
#include <memory>
#include <stdexcept>
#include <string>

namespace Limitless {
	class AbstractModel;

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

	class ModelLoaderFlags {
	public:
		std::set<ModelLoaderOption> options;
		float scale_factor {1.0f};
		InstanceTypes additional_instance_types;

		auto isPresent(ModelLoaderOption option) const { return options.count(option) != 0; }

		ModelLoaderFlags& additionalInstanceTypes(InstanceTypes _additional_instance_types) {
			additional_instance_types = _additional_instance_types;
			return *this;
		}

		ModelLoaderFlags& instanced() {
			additional_instance_types.emplace(InstanceType::Instanced);
			return *this;
		}
	};

	class GltfModelLoader {
	public:
		// Load a 3D model from given file.
		// Will also attempt to load materials referenced in model definition.
		// Returns a shared pointer to resulting model on success.
		// Provided assets are modified.
		// On failure, a ModelLoadError exception is thrown.
		static std::shared_ptr<AbstractModel> loadModel(
			Assets& assets, const fs::path& path, const ModelLoaderFlags& flags
		);
	};
}
