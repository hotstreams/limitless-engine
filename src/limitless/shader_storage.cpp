#include <limitless/shader_storage.hpp>
#include <limitless/core/shader/shader_compiler.hpp>
#include <limitless/pipeline/render_settings.hpp>

using namespace Limitless;

bool Limitless::operator<(const ShaderKey& lhs, const ShaderKey& rhs) noexcept {
    return std::tie(lhs.material_type, lhs.model_type, lhs.material_index) < std::tie(rhs.material_type, rhs.model_type, rhs.material_index);
}

ShaderProgram& ShaderStorage::get(const std::string& name) const {
    try {
        return *shaders.at(name);
    } catch (const std::out_of_range& e) {
        throw shader_storage_error("No such shader " + name);
    }
}

ShaderProgram& ShaderStorage::get(ShaderPass material_type, ModelShader model_type, uint64_t material_index) const {
    try {
        return *materials.at({material_type, model_type, material_index});
    } catch (const std::out_of_range& e) {
        throw shader_storage_error("No such material shader");
    }
}

void ShaderStorage::add(std::string name, std::shared_ptr<ShaderProgram> program) {
    std::unique_lock lock(mutex);

    const auto result = shaders.emplace(std::move(name), std::move(program));
    if (!result.second) {
        throw shader_storage_error{"Shader already exists"};
    }
}

void ShaderStorage::add(ShaderPass material_type, ModelShader model_type, uint64_t material_index, std::shared_ptr<ShaderProgram> program) {
    std::unique_lock lock(mutex);
    const auto key = ShaderKey{material_type, model_type, material_index};

    const auto result = materials.emplace(key, program);
    if (!result.second) {
        if (!materials[key]) {
            materials[key] = std::move(program);
            return;
        }

        throw shader_storage_error{"Shader already exists"};
    }
}

bool ShaderStorage::contains(ShaderPass material_type, ModelShader model_type, uint64_t material_index) noexcept {
    std::unique_lock lock(mutex);
    return materials.find({material_type, model_type, material_index}) != materials.end();
}

bool ShaderStorage::contains(const fx::UniqueEmitterShaderKey& emitter_type) noexcept {
    std::unique_lock lock(mutex);
    return emitters.find(emitter_type) != emitters.end();
}

bool ShaderStorage::reserveIfNotContains(ShaderPass material_type, ModelShader model_type, uint64_t material_index) noexcept {
    std::unique_lock lock(mutex);
    bool contains = materials.find({material_type, model_type, material_index}) != materials.end();

    if (!contains) {
        materials.emplace(ShaderKey{material_type, model_type, material_index}, nullptr);
    }

    return contains;
}

bool ShaderStorage::reserveIfNotContains(const fx::UniqueEmitterShaderKey& emitter_type) noexcept {
    std::unique_lock lock(mutex);
    bool contains = emitters.find(emitter_type) != emitters.end();

    if (!contains) {
        emitters.emplace(emitter_type, nullptr);
    }

    return contains;
}

ShaderProgram& ShaderStorage::get(const fx::UniqueEmitterShaderKey& emitter_type) const {
    try {
        return *emitters.at(emitter_type);
    } catch (const std::out_of_range& e) {
        throw shader_storage_error("No such sprite emitter shader");
    }
}

void ShaderStorage::add(const fx::UniqueEmitterShaderKey& emitter_type, std::shared_ptr<ShaderProgram> program) {
    std::unique_lock lock(mutex);
    const auto result = emitters.emplace(emitter_type, program);
    if (!result.second) {
        if (!emitters[emitter_type]) {
            emitters[emitter_type] = std::move(program);
            return;
        }

        throw shader_storage_error{"Shader already contains emitter"};
    }
}

void ShaderStorage::initialize(Context& ctx, const RenderSettings& settings, const fs::path& shader_dir) {
    ShaderCompiler compiler {ctx, settings};

    if (settings.pipeline == RenderPipeline::Forward) {
        add("blur", compiler.compile(shader_dir / "postprocessing/blur"));
        add("brightness", compiler.compile(shader_dir / "postprocessing/bloom/brightness"));
        add("postprocess", compiler.compile(shader_dir / "postprocessing/postprocess"));
    }

    if (settings.pipeline == RenderPipeline::Deferred) {
        add("deferred", compiler.compile(shader_dir / "pipeline/deferred/deferred"));
        add("composite", compiler.compile(shader_dir / "pipeline/deferred/composite"));
        add("ssao", compiler.compile(shader_dir / "postprocessing/ssao"));
        add("ssao_blur", compiler.compile(shader_dir / "postprocessing/ssao_blur"));

        add("blur_downsample", compiler.compile(shader_dir / "postprocessing/bloom/blur_downsample"));
        add("blur_upsample", compiler.compile(shader_dir / "postprocessing/bloom/blur_upsample"));
        add("brightness", compiler.compile(shader_dir / "postprocessing/bloom/brightness"));
    }

    if (settings.fast_approximate_antialiasing) {
        add("fxaa", compiler.compile(shader_dir / "postprocessing/fxaa"));
    }

    if (settings.depth_of_field) {
	    add("dof", compiler.compile(shader_dir / "postprocessing/dof"));
    }

    add("quad", compiler.compile(shader_dir / "pipeline/quad"));
    add("text", compiler.compile(shader_dir / "text/text"));
    add("text_selection", compiler.compile(shader_dir / "text/text_selection"));
}

void ShaderStorage::clear() {
    materials.clear();
    emitters.clear();
    shaders.clear();
}

void ShaderStorage::add(const ShaderStorage& other) {
    for (auto&& [key, value] : other.shaders) {
        shaders.emplace(key, value);
    }

    for (auto&& [key, value] : other.materials) {
        materials.emplace(key, value);
    }

    for (auto&& [key, value] : other.emitters) {
        emitters.emplace(key, value);
    }
}

bool ShaderStorage::contains(const std::string& name) noexcept {
    return shaders.find(name) != shaders.end();
}

void ShaderStorage::remove(ShaderPass material_type, ModelShader model_type, uint64_t material_index) {
    std::unique_lock lock(mutex);

    const auto key = ShaderKey{material_type, model_type, material_index};

    materials.erase(key);
}
