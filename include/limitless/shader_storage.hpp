#pragma once

#include <limitless/fx/emitters/unique_emitter.hpp>
#include <limitless/renderer/shader_type.hpp>

#include <unordered_map>
#include <memory>
#include <mutex>
#include <map>

namespace Limitless {
    class RendererSettings;
    class ShaderProgram;
    class Context;

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
        void initialize(Context& ctx, const RendererSettings& settings, const fs::path& shader_dir);

        ShaderProgram& get(const std::string& name) const;
        ShaderProgram& get(ShaderType material_type, InstanceType model_type, uint64_t material_index) const;
        ShaderProgram& get(const fx::UniqueEmitterShaderKey& emitter_type) const;

        void add(std::string name, std::shared_ptr<ShaderProgram> program);
        void add(ShaderType material_type, InstanceType model_type, uint64_t material_index, std::shared_ptr<ShaderProgram> program);
        void add(const fx::UniqueEmitterShaderKey& emitter_type, std::shared_ptr<ShaderProgram> program);

        void remove(ShaderType material_type, InstanceType model_type, uint64_t material_index);

        bool contains(const std::string& name) noexcept;
        bool contains(ShaderType material_type, InstanceType model_type, uint64_t material_index) noexcept;
        bool contains(const fx::UniqueEmitterShaderKey& emitter_type) noexcept;

        bool reserveIfNotContains(ShaderType material_type, InstanceType model_type, uint64_t material_index) noexcept;
        bool reserveIfNotContains(const fx::UniqueEmitterShaderKey& emitter_type) noexcept;

        const auto& getCommonShaders() const noexcept { return shaders; }
        const auto& getMaterialShaders() const noexcept { return materials; }
        const auto& getEmitterShaders() const noexcept { return emitters; }

        void add(const ShaderStorage& other);

        void clear();
    };
}
