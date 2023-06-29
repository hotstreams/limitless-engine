#include <limitless/core/uniform/uniform_time.hpp>

#include <limitless/core/shader_program.hpp>
#include <limitless/core/bindless_texture.hpp>
#include <limitless/core/texture.hpp>
#include <limitless/core/context_initializer.hpp>

using namespace Limitless;

UniformTime::UniformTime(const std::string& name) noexcept
        : UniformValue{name, UniformType::Time, 0.0f} {

}

void UniformTime::update() noexcept {
    if (start == std::chrono::time_point<std::chrono::steady_clock>{}) {
        start = std::chrono::steady_clock::now();
    }

    setValue(std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::steady_clock::now() - start).count());
}

void UniformTime::set(const ShaderProgram& shader) {
    update();
    UniformValue::set(shader);
}

UniformTime* UniformTime::clone() noexcept {
    return new UniformTime(*this);
}
