#pragma once

#include <limitless/core/shader/shader.hpp>
#include <string>
#include <unordered_map>

namespace Limitless {
    inline std::unordered_map<Shader::Type, std::string> shader_file_extensions = {
            { Shader::Type::Vertex, ".vert" },
            { Shader::Type::TessControl, ".tesc" },
            { Shader::Type::TessEval, ".tese" },
            { Shader::Type::Geometry, ".geom" },
            { Shader::Type::Fragment, ".frag" },
            { Shader::Type::Compute, ".comp" }
    };
}