#include <limitless/core/uniform_setter.hpp>

#include <limitless/core/shader_program.hpp>

using namespace LimitlessEngine;

UniformSetter::UniformSetter(std::function<void(ShaderProgram&)>&& f)
    : setters {f} {
}

void UniformSetter::operator()(ShaderProgram& shader) const {
    for (auto& setter : setters) {
        if (setter) {
            setter(shader);
        }
    }
}

void UniformSetter::add(std::function<void(ShaderProgram&)>&& f) {
    setters.emplace_back(std::move(f));
}