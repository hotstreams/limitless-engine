#pragma once

#include <filesystem>
#include <limitless/assets.hpp>
#include <limitless/models/model.hpp>

namespace Limitless {
    struct ModelSaveError : public std::runtime_error { 
        explicit ModelSaveError(const std::string& msg) noexcept
            : std::runtime_error(msg) {}
    };

	class GltfModelSaver {
	public:
		static void saveModel(const std::filesystem::path& output_path, const Model& model);
	};
}
