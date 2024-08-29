#pragma once

#include <vector>
#include <functional>

namespace Limitless {
    class ShaderProgram;
    class Instance;

    class UniformSetter {
    private:
        std::vector<std::function<void(ShaderProgram&)>> setters;
    public:
        UniformSetter(std::function<void(ShaderProgram&)>&& f);
        UniformSetter() = default;

        void add(std::function<void(ShaderProgram&)>&& f);

        void operator()(ShaderProgram& shader) const;
    };

    class UniformInstanceSetter {
    private:
        std::vector<std::function<void(ShaderProgram&, const Instance& instance)>> setters;
    public:
        UniformInstanceSetter(std::function<void(ShaderProgram&, const Instance& instance)>&& f);
        UniformInstanceSetter() = default;

        void add(std::function<void(ShaderProgram&, const Instance& instance)>&& f);

        void operator()(ShaderProgram& shader, const Instance& instance) const;
    };
}