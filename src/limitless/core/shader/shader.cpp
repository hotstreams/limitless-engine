#include <limitless/core/shader/shader.hpp>
#include <limitless/core/context_initializer.hpp>
#include <limitless/core/keyline_extensions.hpp>
#include <string>
#include <sstream>
#include <limitless/core/shader/shader_extensions.hpp>

using namespace Limitless;

Shader::Shader(fs::path _path, Type _type, const ShaderAction& action)
    : path {std::move(_path)}
    , type {_type} {
    source = getSource(path);

    replaceIncludes(path.parent_path());

    if (action) {
        action(*this);
    }

    // includes can appear in other includes that were added by ShaderActions
    // so replace one more time
    replaceIncludes(path.parent_path());

    id = glCreateShader(static_cast<GLenum>(type));
}

Shader::~Shader() {
    if (id != 0) {
        glDeleteShader(id);
    }
}

void Shader::checkStatus() const {
    GLint success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);

    GLint log_size = 0;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &log_size);

    if (log_size == 0) {

#ifdef LIMITLESS_OPENGL_SHADER_OUTPUT
        static int i = 0;

        for (const auto& [ext, t]: shader_file_extensions) {
            if (t == type) {
                std::ofstream f {"./glslang/" + std::to_string(i++) + ext.data()};
                f << getSource();
                f.close();
                break;
            }
        }
#endif

//        return;
    }

    std::string log;
    log.resize(log_size);
    glGetShaderInfoLog(id, log_size, &log_size, log.data());

    {
        static int counter = 0;
        std::ofstream file("shader_compilation_error_" + path.stem().string() + std::to_string(counter++));
        file << path << std::endl;
        file << log << std::endl;
        file << "-------------" << std::endl;
        file << "Shader source:" << source << std::endl;
    }

    return;

    throw shader_compilation_error("failed to compile " + path.string() + ": " + log);
}

void Shader::replaceKey(const std::string& key, const std::string& value) noexcept {
    size_t found = 0;
    for (;;) {
        found = source.find(key, found);

        if (found == std::string::npos) {
            break;
        }

        source.replace(found, key.length(), value);
    }
}

std::string Shader::getSource(const fs::path& filepath) {
    try {
        std::ifstream file(filepath);
        std::string file_source;

        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        if (file.good()) {
            std::stringstream stream;
            stream << file.rdbuf();

            file_source = stream.str();
        }

        return file_source;
    } catch (...) {
        throw shader_file_not_found(filepath.string());
    }
}

void Shader::resolveIncludes(const fs::path& base_dir, std::string& src) {
    static constexpr std::string_view include = "#include";

    std::size_t found {};
    for (;;) {
        found = src.find(include, found);

        if (found == std::string::npos) {
            return;
        }

        size_t beg = found + include.length() + 2;
        size_t end = src.find('"', beg);
        size_t name_length = end - beg;

        fs::path file_name = src.substr(beg, name_length);

        if (!include_entries.count(file_name.stem().string())) {
            include_entries.emplace(file_name.stem().string());

            auto include_src = getSource(base_dir / file_name);
            resolveIncludes(base_dir / file_name.parent_path(), include_src);
            src.replace(found, include.length() + 3 + name_length, include_src);
        } else {
            src.replace(found, include.length() + 3 + name_length, "");
        }
    }
}

void Shader::replaceIncludes(const fs::path& base_dir) {
    try {
        resolveIncludes(base_dir, source);
    } catch (const shader_file_not_found& not_found) {
        throw shader_include_not_found("Failed to resolve include for " + path.string() + ": " + not_found.what());
    }
}

void Shader::compile() const {
    const auto* src = source.data();

    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    checkStatus();
}

void Limitless::swap(Shader &lhs, Shader &rhs) noexcept {
    using std::swap;

    swap(lhs.source, rhs.source);
    swap(lhs.path, rhs.path);
    swap(lhs.type, rhs.type);
    swap(lhs.id, rhs.id);
}

Shader::Shader(Shader&& rhs) noexcept : Shader() {
    swap(*this, rhs);
}

Shader &Shader::operator=(Shader&& rhs) noexcept {
    swap(*this, rhs);
    return *this;
}
