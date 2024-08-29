#include "limitless/core/uniform/uniform_setter.hpp"

#include "limitless/core/shader/shader_program.hpp"

using namespace Limitless;

UniformSetter::UniformSetter(std::function<void(ShaderProgram&)>&& f)
    : setters {std::move(f)} {
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

UniformInstanceSetter::UniformInstanceSetter(std::function<void(ShaderProgram &, const Instance &)>&& f)
    : setters {std::move(f)} {

}

void UniformInstanceSetter::add(std::function<void(ShaderProgram &, const Instance &)> &&f) {
    setters.emplace_back(std::move(f));
}

void UniformInstanceSetter::operator()(ShaderProgram &shader, const Instance &instance) const {
    for (auto& setter : setters) {
        if (setter) {
            setter(shader, instance);
        }
    }
}
