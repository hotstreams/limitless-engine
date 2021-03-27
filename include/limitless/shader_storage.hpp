#pragma once

#include <limitless/particle_system/unique_emitter.hpp>
#include <limitless/shader_types.hpp>

#include <unordered_map>
#include <memory>
#include <mutex>
#include <map>

namespace LimitlessEngine {
    class ShaderProgram;

    struct ShaderKey {
        MaterialShader material_type;
        ModelShader model_type;
        uint64_t material_index;
    };
    bool operator<(const ShaderKey& lhs, const ShaderKey& rhs) noexcept;

    inline const std::map<MaterialShader, std::string> material_shader_path = {
        {MaterialShader::Forward,           "pipeline/forward" },
        {MaterialShader::Deferred,          "pipeline/deferred" },
        {MaterialShader::ForwardPlus,       "pipeline/forward+" },
        {MaterialShader::DirectionalShadow, "pipeline/directional_shadows" },
        {MaterialShader::PointShadow,       "shadows/point_shadows" },
        {MaterialShader::ColorPicker,       "other/color_picker" }
    };

    struct shader_storage_error : public std::runtime_error {
        explicit shader_storage_error(const char* msg) : runtime_error{msg} {}
        explicit shader_storage_error(const std::string& msg) : runtime_error{msg} {}
    };

    class ShaderStorage final {
    private:
        std::unordered_map<std::string, std::shared_ptr<ShaderProgram>> shaders;
        std::map<ShaderKey, std::shared_ptr<ShaderProgram>> material_shaders;

        std::map<UniqueSpriteEmitter, std::shared_ptr<ShaderProgram>> sprite_emitter_shaders;
        std::map<UniqueMeshEmitter, std::shared_ptr<ShaderProgram>> mesh_emitter_shaders;
        std::mutex mutex;
    public:
        ShaderStorage() = default;
        ~ShaderStorage() = default;

        void initialize();

        ShaderProgram& get(const std::string& name) const;
        ShaderProgram& get(MaterialShader material_type, ModelShader model_type, uint64_t material_index) const;
        ShaderProgram& get(const UniqueSpriteEmitter& emitter_type) const;
        ShaderProgram& get(const UniqueMeshEmitter& emitter_type) const;

        void add(std::string name, std::shared_ptr<ShaderProgram> program) noexcept;
        void add(MaterialShader material_type, ModelShader model_type, uint64_t material_index, std::shared_ptr<ShaderProgram> program) noexcept;
        void add(const UniqueSpriteEmitter& emitter_type, std::shared_ptr<ShaderProgram> program) noexcept;
        void add(const UniqueMeshEmitter& emitter_type, std::shared_ptr<ShaderProgram> program) noexcept;

        bool exists(MaterialShader material_type, ModelShader model_type, uint64_t material_index) noexcept;
    };
}