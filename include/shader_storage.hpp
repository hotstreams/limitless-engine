#pragma once

#include <shader_program.hpp>
#include <emitter_buffer.hpp>

namespace GraphicsEngine {
    enum class MaterialShaderType {
        Default,
        Deferred,
        ForwardPlus,
        DirectionalShadow,
        PointShadow,
        ColorPicker
    };

    enum class ModelShaderType {
        Model,
        Skeletal,
        Instanced,
        SkeletalInstanced,
    };

    struct ShaderKey {
        MaterialShaderType material_type;
        ModelShaderType model_type;
        uint64_t material_index;
    };

    bool operator<(const ShaderKey& lhs, const ShaderKey& rhs) noexcept;

    using RequiredMaterialShaders = std::vector<MaterialShaderType>;
    using RequiredModelShaders = std::vector<ModelShaderType>;

    inline const std::map<MaterialShaderType, std::string> material_shader_path = {
        { MaterialShaderType::Default, "pipeline/forward" },
        { MaterialShaderType::Deferred, "pipeline/deferred" },
        { MaterialShaderType::ForwardPlus, "pipeline/forward+" },
        { MaterialShaderType::DirectionalShadow, "shadows/directional" },
        { MaterialShaderType::PointShadow, "shadows/point" },
        { MaterialShaderType::ColorPicker, "other/color_picker" }
    };

    class ShaderStorage {
    private:
        std::unordered_map<std::string, std::shared_ptr<ShaderProgram>> shaders;
        std::map<ShaderKey, std::shared_ptr<ShaderProgram>> material_shaders;

        std::map<UniqueSpriteEmitter, std::shared_ptr<ShaderProgram>> sprite_emitter_shaders;
        std::map<UniqueMeshEmitter, std::shared_ptr<ShaderProgram>> mesh_emitter_shaders;
    public:
        const std::shared_ptr<ShaderProgram>& get(const std::string& name) const;
        const std::shared_ptr<ShaderProgram>& get(MaterialShaderType material_type, ModelShaderType model_type, uint64_t material_index) const;
        const std::shared_ptr<ShaderProgram>& get(const UniqueSpriteEmitter& emitter_type) const;
        const std::shared_ptr<ShaderProgram>& get(const UniqueMeshEmitter& emitter_type) const;

        void add(std::string name, std::shared_ptr<ShaderProgram> program) noexcept;
        void add(MaterialShaderType material_type, ModelShaderType model_type, uint64_t material_index, std::shared_ptr<ShaderProgram> program) noexcept;
        void add(const UniqueSpriteEmitter& emitter_type, std::shared_ptr<ShaderProgram> program) noexcept;
        void add(const UniqueMeshEmitter& emitter_type, std::shared_ptr<ShaderProgram> program) noexcept;

        bool isExist(MaterialShaderType material_type, ModelShaderType model_type, uint64_t material_index) const noexcept;

        void initialize();
    };

    inline ShaderStorage shader_storage;
}