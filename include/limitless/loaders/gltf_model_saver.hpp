#pragma once

#include <filesystem>
#include <stdexcept>
#include <limitless/assets.hpp>
#include <limitless/models/model.hpp>

namespace Limitless {
	class SkeletalModel;

    struct ModelSaveError : public std::runtime_error { 
        explicit ModelSaveError(const std::string& msg) noexcept
            : std::runtime_error(msg) {}
    };

	class GltfModelSaver {
	public:
		static void saveModel(const std::filesystem::path& output_path, const Model& model);

		// Reload `source_path`, replace its animation tracks with `model`'s, and write a GLB.
		// Mesh, skin, and material data from the source file are preserved.
		// Returns the path that was written.
		static std::filesystem::path saveSkeletalAnimations(
			const std::filesystem::path& source_path,
			const SkeletalModel& model
		);
	};
}
