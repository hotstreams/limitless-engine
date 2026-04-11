#include <limitless/shader_storage.hpp>
#include <limitless/core/shader/shader_compiler.hpp>
#include <limitless/core/shader/shader_program.hpp>
#include <limitless/renderer/renderer_settings.hpp>
#include <limitless/core/context_debug.hpp>
#include <GL/glew.h>
#include <sstream>

using namespace Limitless;

namespace {
	[[nodiscard]] static const char* toString(ShaderType t) noexcept {
		switch (t) {
			case ShaderType::Depth: return "Depth";
			case ShaderType::GBuffer: return "GBuffer";
			case ShaderType::Decal: return "Decal";
			case ShaderType::Skybox: return "Skybox";
			case ShaderType::Forward: return "Forward";
			case ShaderType::DirectionalShadow: return "DirectionalShadow";
			case ShaderType::ColorPicker: return "ColorPicker";
			default: return "ShaderType(?)";
		}
	}

	[[nodiscard]] static const char* toString(InstanceType t) noexcept {
		switch (t) {
			case InstanceType::Model: return "Model";
			case InstanceType::Skeletal: return "Skeletal";
			case InstanceType::BatchedModel: return "BatchedModel";
			case InstanceType::Instanced: return "Instanced";
			case InstanceType::SkeletalInstanced: return "SkeletalInstanced";
			case InstanceType::Effect: return "Effect";
			case InstanceType::Decal: return "Decal";
			case InstanceType::Terrain: return "Terrain";
			case InstanceType::IndirectModel: return "IndirectModel";
			default: return "InstanceType(?)";
		}
	}

	static void labelProgramIfPossible(const std::shared_ptr<ShaderProgram>& program, const std::string& label) {
		if (!program) return;
		if (!(GLEW_KHR_debug || GLEW_VERSION_4_3)) return;
		const auto id = program->getId();
		if (!id) return;
		glObjectLabel(GL_PROGRAM, id, static_cast<GLsizei>(label.size()), label.c_str());
	}
}

bool ShaderKey::operator<(const ShaderKey& rhs) const noexcept {
    return std::tie(material_type, model_type, material_index) <
           std::tie(rhs.material_type, rhs.model_type, rhs.material_index);
}

ShaderProgram& ShaderStorage::get(const std::string& name) const {
    try {
        return *shaders.at(name);
    } catch (const std::out_of_range& e) {
        throw shader_storage_error("No such shader " + name);
    }
}

ShaderProgram& ShaderStorage::get(ShaderType material_type, InstanceType model_type, uint64_t material_index) const {
    try {
        return *materials.at({material_type, model_type, material_index});
    } catch (const std::out_of_range&) {
        std::ostringstream msg;
        msg << "No such material shader (pass=" << toString(material_type) << ", instance=" << toString(model_type)
			<< ", material_index=" << material_index << ')';
        throw shader_storage_error(msg.str());
    }
}

void ShaderStorage::add(std::string name, std::shared_ptr<ShaderProgram> program) {
    std::unique_lock lock(mutex);

    const auto result = shaders.emplace(std::move(name), std::move(program));
    if (!result.second) {
        throw shader_storage_error{"Shader already exists"};
    }

	// Attach a GL object label for better KHR_debug output (optional).
	labelProgramIfPossible(result.first->second, "shader:" + result.first->first);
	#ifdef LIMITLESS_OPENGL_DEBUG
	Limitless::debug_register_program(result.first->second->getId(), "shader:" + result.first->first);
	#endif
}

void ShaderStorage::add(ShaderType material_type, InstanceType model_type, uint64_t material_index, std::shared_ptr<ShaderProgram> program) {
    std::unique_lock lock(mutex);
    const auto key = ShaderKey{material_type, model_type, material_index};

    const auto result = materials.emplace(key, program);
    if (!result.second) {
        if (!materials[key]) {
            materials[key] = std::move(program);
            // fallthrough to labeling / debug registration below
        } else {
            throw shader_storage_error{"Shader already exists"};
        }
    }

	std::ostringstream ss;
	ss << "material:" << toString(material_type) << ":" << toString(model_type) << ":" << material_index;
	// `result.first` points to the map entry; handle both the emplace-success and
	// "filled previously-reserved nullptr" cases.
	labelProgramIfPossible(materials.at(key), ss.str());
	#ifdef LIMITLESS_OPENGL_DEBUG
	Limitless::debug_register_program(materials.at(key)->getId(), ss.str());
	#endif
}

bool ShaderStorage::contains(ShaderType material_type, InstanceType model_type, uint64_t material_index) noexcept {
    std::unique_lock lock(mutex);
    return materials.find({material_type, model_type, material_index}) != materials.end();
}

bool ShaderStorage::contains(const fx::UniqueEmitterShaderKey& emitter_type) noexcept {
    std::unique_lock lock(mutex);
    return emitters.find(emitter_type) != emitters.end();
}

bool ShaderStorage::reserveIfNotContains(ShaderType material_type, InstanceType model_type, uint64_t material_index) noexcept {
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
            // fallthrough to labeling / debug registration below
        } else {
            throw shader_storage_error{"Shader already contains emitter"};
        }
    }

	// Best-effort label: we don't stringify the full key here to avoid pulling more deps.
	labelProgramIfPossible(emitters.at(emitter_type), "emitter:shader");
	#ifdef LIMITLESS_OPENGL_DEBUG
	Limitless::debug_register_program(emitters.at(emitter_type)->getId(), "emitter:shader");
	#endif
}

void ShaderStorage::initialize(Context& ctx, const RendererSettings& settings, const fs::path& shader_dir) {
    ShaderCompiler compiler {ctx, settings};

    if (settings.bloom) {
        add("blur_downsample", compiler.compile(shader_dir / "postprocessing/quad",shader_dir / "postprocessing/bloom/blur_downsample"));
        add("blur_upsample", compiler.compile(shader_dir / "postprocessing/quad",shader_dir / "postprocessing/bloom/blur_upsample"));
        add("brightness", compiler.compile(shader_dir / "postprocessing/quad",shader_dir / "postprocessing/bloom/brightness"));
        add("bloom_prefilter", compiler.compile(shader_dir / "postprocessing/quad", shader_dir / "postprocessing/bloom/bloom_prefilter"));
        add("bloom_downsample9", compiler.compile(shader_dir / "postprocessing/quad", shader_dir / "postprocessing/bloom/bloom_downsample9"));
    }

    add("deferred", compiler.compile(shader_dir / "pipeline/quad", shader_dir / "pipeline/deferred"));
    add("composite", compiler.compile(shader_dir / "pipeline/quad", shader_dir / "pipeline/composite"));
    add("outline", compiler.compile(shader_dir / "pipeline/quad", shader_dir / "pipeline/outline"));
    add("debug_cascade_overlay", compiler.compile(shader_dir / "pipeline/quad", shader_dir / "pipeline/debug_cascade_overlay"));


    if (settings.ambient_occlusion_mode == AmbientOcclusionMode::SAO) {
        add("ssao", compiler.compile(shader_dir / "postprocessing/quad",shader_dir / "postprocessing/ssao/ssao"));
        add("ssao_blur", compiler.compile(shader_dir / "postprocessing/quad",shader_dir / "postprocessing/ssao/ssao_blur"));
    }
    if (settings.ambient_occlusion_mode == AmbientOcclusionMode::HBAO) {
        add("hbao_depthlinearize", compiler.compile(shader_dir / "postprocessing/quad", shader_dir / "postprocessing/hbao/hbao_depthlinearize"));
        add("hbao_calc", compiler.compile(shader_dir / "postprocessing/quad", shader_dir / "postprocessing/hbao/hbao_calc"));
        add("hbao_blur_pass1", compiler.compile(shader_dir / "postprocessing/quad", shader_dir / "postprocessing/hbao/hbao_blur_pass1"));
        add("hbao_blur_pass2", compiler.compile(shader_dir / "postprocessing/quad", shader_dir / "postprocessing/hbao/hbao_blur_pass2"));
        add("hbao_pack", compiler.compile(shader_dir / "postprocessing/quad", shader_dir / "postprocessing/hbao/hbao_pack"));
    }

    if (settings.screen_space_reflections) {
        add("ssr", compiler.compile(shader_dir / "postprocessing/quad",shader_dir / "postprocessing/ssr/ssr"));
    }

    if (settings.fast_approximate_antialiasing) {
        add("fxaa", compiler.compile(shader_dir / "postprocessing/quad",shader_dir / "postprocessing/fxaa"));
    }

//    if (settings.depth_of_field) {
//	    add("dof", compiler.compile(shader_dir / "postprocessing/dof"));
//    }

    add("quad", compiler.compile(shader_dir / "pipeline/quad"));

    add("text", compiler.compile(shader_dir / "text/text"));
    add("text_selection", compiler.compile(shader_dir / "text/text_selection"));
    add("icon_text", compiler.compile(shader_dir / "text/icon_text"));
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

void ShaderStorage::remove(ShaderType material_type, InstanceType model_type, uint64_t material_index) {
    std::unique_lock lock(mutex);

    const auto key = ShaderKey{material_type, model_type, material_index};

    materials.erase(key);
}
