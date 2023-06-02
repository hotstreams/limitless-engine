#pragma once

#include <limitless/core/shader.hpp>
#include <string_view>

namespace Limitless {
    constexpr struct { std::string_view ext; Shader::Type type; } shader_file_extensions[] = {
            { ".vs",  Shader::Type::Vertex },
            { ".tcs", Shader::Type::TessControl },
            { ".tes", Shader::Type::TessEval },
            { ".gs",  Shader::Type::Geometry },
            { ".fs",  Shader::Type::Fragment },
            { ".cs",  Shader::Type::Compute }
    };
}