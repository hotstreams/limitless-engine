#pragma once

#include <shader_program.hpp>

namespace GraphicsEngine {
    enum class MaterialShaderType { Default, Deferred, ForwardPlus, Shadows, ColorPicker };
    using RequiredShaders = std::vector<MaterialShaderType>;
    inline const std::map<MaterialShaderType, std::string> material_shader_path = {
        { MaterialShaderType::Default, "pipeline/forward" },
        { MaterialShaderType::Deferred, "pipeline/deferred" },
        { MaterialShaderType::ForwardPlus, "pipeline/forward+" },
        { MaterialShaderType::Shadows, "pipeline/shadows" },
        { MaterialShaderType::ColorPicker, "other/color_picker" }
    };

    class ShaderStorage {
    private:
        std::unordered_map<std::string, std::shared_ptr<ShaderProgram>> shaders;

        using MaterialShaderMap = std::unordered_map<uint64_t, std::shared_ptr<ShaderProgram>>;
        std::unordered_map<MaterialShaderType, MaterialShaderMap> material_shaders;
    public:
        const auto& get(const std::string& name) const {
            return shaders.at(name);
        }

        const auto& get(MaterialShaderType type, uint64_t material_index) const {
            return material_shaders.at(type).at(material_index);
        }

        void add(std::string name, std::shared_ptr<ShaderProgram> program) noexcept {
            shaders.emplace(std::move(name), std::move(program));
        }

        void add(MaterialShaderType type, uint64_t material_index, std::shared_ptr<ShaderProgram> program) noexcept {
            material_shaders[type].emplace(material_index, std::move(program));
        }

        void initialize() {

        }
    };

    inline ShaderStorage shader_storage;
}