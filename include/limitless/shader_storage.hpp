#pragma once

#include <limitless/fx/emitters/unique_emitter.hpp>
#include <limitless/pipeline/shader_pass_types.hpp>
#include <limitless/util/filesystem.hpp>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <map>

namespace Limitless {
    class RenderSettings;
    class ShaderProgram;
    class Context;

    struct ShaderKey {
        ShaderPass material_type;
        ModelShader model_type;
        uint64_t material_index;
    };
    bool operator<(const ShaderKey& lhs, const ShaderKey& rhs) noexcept;

    inline const std::map<ShaderPass, std::string> SHADER_PASS_PATH = {
        { ShaderPass::Forward,              "pipeline"  PATH_SEPARATOR "forward" PATH_SEPARATOR "forward" },

        { ShaderPass::Depth,                "pipeline"  PATH_SEPARATOR "deferred" PATH_SEPARATOR "depth" },
        { ShaderPass::GBuffer,              "pipeline"  PATH_SEPARATOR "deferred" PATH_SEPARATOR "gbuffer" },
        { ShaderPass::Skybox,               "pipeline"  PATH_SEPARATOR "deferred" PATH_SEPARATOR "skybox"},
        { ShaderPass::Transparent,          "pipeline"  PATH_SEPARATOR "deferred" PATH_SEPARATOR "transparent"},

        { ShaderPass::DirectionalShadow,    "pipeline"  PATH_SEPARATOR "lighting" PATH_SEPARATOR "directional_shadows" },

        { ShaderPass::ColorPicker,          "pipeline"  PATH_SEPARATOR "util"     PATH_SEPARATOR "color_picker" },
    };

    class shader_storage_error : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    class ShaderStorage final {
    private:
        std::unordered_map<std::string, std::shared_ptr<ShaderProgram>> shaders;

        std::map<ShaderKey, std::shared_ptr<ShaderProgram>> materials;
        std::map<fx::UniqueEmitterShaderKey, std::shared_ptr<ShaderProgram>> emitters;

        std::mutex mutex;
    public:
        ShaderStorage() = default;
        ~ShaderStorage() = default;

        void initialize(Context& ctx, const RenderSettings& settings, const fs::path& shader_dir);

        ShaderProgram& get(const std::string& name) const;
        ShaderProgram& get(ShaderPass material_type, ModelShader model_type, uint64_t material_index) const;
        ShaderProgram& get(const fx::UniqueEmitterShaderKey& emitter_type) const;

        void add(std::string name, std::shared_ptr<ShaderProgram> program);
        void add(ShaderPass material_type, ModelShader model_type, uint64_t material_index, std::shared_ptr<ShaderProgram> program);
        void add(const fx::UniqueEmitterShaderKey& emitter_type, std::shared_ptr<ShaderProgram> program);

        bool contains(const std::string& name) noexcept;
        bool contains(ShaderPass material_type, ModelShader model_type, uint64_t material_index) noexcept;
        bool contains(const fx::UniqueEmitterShaderKey& emitter_type) noexcept;

        bool reserveIfNotContains(ShaderPass material_type, ModelShader model_type, uint64_t material_index) noexcept;
        bool reserveIfNotContains(const fx::UniqueEmitterShaderKey& emitter_type) noexcept;

        const auto& getCommonShaders() const noexcept { return shaders; }
        const auto& getMaterialShaders() const noexcept { return materials; }
        const auto& getEmitterShaders() const noexcept { return emitters; }

        void add(const ShaderStorage& other);

        void clear();
    };
}
