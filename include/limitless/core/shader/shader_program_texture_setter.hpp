#pragma once

#include <string>
#include <memory>
#include <map>

namespace Limitless {
    class Uniform;

    class ShaderProgramTextureSetter {
    private:

    public:
        static void bindTextures(const std::map<std::string, std::unique_ptr<Uniform>>& uniforms);
    };
}