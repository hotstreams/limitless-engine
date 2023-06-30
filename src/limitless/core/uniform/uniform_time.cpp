#include <limitless/core/uniform/uniform_time.hpp>

using namespace Limitless;

UniformTime::UniformTime(std::string name, GLint location) noexcept
    : UniformValue {std::move(name), location, UniformType::Time, 0.0f} {}

void UniformTime::set() {
    update();
    UniformValue::set();
}

std::unique_ptr<Uniform> UniformTime::clone() noexcept {
    return std::make_unique<UniformTime>(*this);
}

void UniformTime::update() noexcept {
    using namespace std::chrono;

    if (start == time_point<steady_clock>{}) {
        start = steady_clock::now();
    }

    setValue(duration_cast<duration<float>>(steady_clock::now() - start).count());
}

void UniformTime::reset() noexcept {
    start = std::chrono::steady_clock::now();
}
