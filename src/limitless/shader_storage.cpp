#include <limitless/shader_storage.hpp>
#include <limitless/core/shader_compiler.hpp>

using namespace LimitlessEngine;

bool LimitlessEngine::operator<(const ShaderKey& lhs, const ShaderKey& rhs) noexcept {
    return std::tie(lhs.material_type, lhs.model_type, lhs.material_index) < std::tie(rhs.material_type, rhs.model_type, rhs.material_index);
}

bool LimitlessEngine::operator<(const SpriteEmitterKey& lhs, const SpriteEmitterKey& rhs) noexcept {
    return std::tie(lhs.material_type, lhs.emitter_type) < std::tie(rhs.material_type, rhs.emitter_type);
}

bool LimitlessEngine::operator<(const MeshEmitterKey& lhs, const MeshEmitterKey& rhs) noexcept {
    return std::tie(lhs.material_type, lhs.emitter_type) < std::tie(rhs.material_type, rhs.emitter_type);
}

ShaderProgram& ShaderStorage::get(const std::string& name) const {
    try {
        return *shaders.at(name);
    } catch (const std::out_of_range& e) {
        throw shader_storage_error("No such shader " + name);
    }
}

ShaderProgram& ShaderStorage::get(MaterialShader material_type, ModelShader model_type, uint64_t material_index) const {
    try {
        return *material_shaders.at({material_type, model_type, material_index});
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

void ShaderStorage::add(MaterialShader material_type, ModelShader model_type, uint64_t material_index, std::shared_ptr<ShaderProgram> program) {
    std::unique_lock lock(mutex);
    const auto result = material_shaders.emplace(ShaderKey{material_type, model_type, material_index}, std::move(program));
    if (!result.second) {
        throw shader_storage_error{"Shader already exists"};
    }
}

bool ShaderStorage::contains(MaterialShader material_type, ModelShader model_type, uint64_t material_index) noexcept {
    std::unique_lock lock(mutex);
    return material_shaders.find({material_type, model_type, material_index}) != material_shaders.end();
}

bool ShaderStorage::contains(MaterialShader material_type, const UniqueSpriteEmitter& emitter_type) noexcept {
    std::unique_lock lock(mutex);
    return sprite_emitter_shaders.find({material_type, emitter_type}) != sprite_emitter_shaders.end();
}

bool ShaderStorage::contains(MaterialShader material_type, const UniqueMeshEmitter& emitter_type) noexcept {
    std::unique_lock lock(mutex);
    return mesh_emitter_shaders.find({material_type, emitter_type}) != mesh_emitter_shaders.end();
}


ShaderProgram& ShaderStorage::get(MaterialShader material_type, const UniqueSpriteEmitter &emitter_type) const {
    try {
        return *sprite_emitter_shaders.at({material_type, emitter_type});
    } catch (const std::out_of_range& e) {
        throw shader_storage_error("No such sprite emitter shader");
    }
}

ShaderProgram& ShaderStorage::get(MaterialShader material_type, const UniqueMeshEmitter &emitter_type) const {
    try {
        return *mesh_emitter_shaders.at({material_type, emitter_type});
    } catch (const std::out_of_range& e) {
        throw shader_storage_error("No such mesh emitter shader");
    }
}

void ShaderStorage::add(MaterialShader material_type, const UniqueSpriteEmitter& emitter_type, std::shared_ptr<ShaderProgram> program) {
    std::unique_lock lock(mutex);
    const auto result = sprite_emitter_shaders.emplace(SpriteEmitterKey{material_type, emitter_type}, std::move(program));
    if (!result.second) {
        throw shader_storage_error{"Shader already exists"};
    }
}

void ShaderStorage::add(MaterialShader material_type, const UniqueMeshEmitter& emitter_type, std::shared_ptr<ShaderProgram> program) {
    std::unique_lock lock(mutex);
    const auto result = mesh_emitter_shaders.emplace(MeshEmitterKey{material_type, emitter_type}, std::move(program));
    if (!result.second) {
        throw shader_storage_error{"Shader already exists"};
    }
}

void ShaderStorage::initialize(Context& ctx) {
    ShaderCompiler compiler{ctx};

    add("blur", compiler.compile(SHADER_DIR "postprocessing/blur"));
    add("brightness", compiler.compile(SHADER_DIR "postprocessing/brightness"));
    add("postprocess", compiler.compile(SHADER_DIR "postprocessing/postprocess"));
    add("skybox", compiler.compile(SHADER_DIR "pipeline/skybox"));
    add("text", compiler.compile(SHADER_DIR "pipeline/text"));
    add("text_selection", compiler.compile(SHADER_DIR "pipeline/text_selection"));
}
