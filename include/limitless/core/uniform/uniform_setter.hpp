#pragma once

#include <vector>
#include <functional>

namespace Limitless {
    class ShaderProgram;

    class UniformSetter {
    private:
        std::vector<std::function<void(ShaderProgram&)>> setters;
    public:
        explicit UniformSetter(std::function<void(ShaderProgram&)>&& f);
        UniformSetter() = default;

        void add(std::function<void(ShaderProgram&)>&& f);

        void operator()(ShaderProgram& shader) const;
    };
}