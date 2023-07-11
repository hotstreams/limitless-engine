#pragma once

#include <limitless/core/shader/shader.hpp>
#include <string_view>

namespace Limitless {
    constexpr struct { std::string_view ext; Shader::Type type; } shader_file_extensions[] = {
            { ".vert",  Shader::Type::Vertex },
            { ".tesc", Shader::Type::TessControl },
            { ".tese", Shader::Type::TessEval },
            { ".geom",  Shader::Type::Geometry },
            { ".frag",  Shader::Type::Fragment },
            { ".comp",  Shader::Type::Compute }
    };
}