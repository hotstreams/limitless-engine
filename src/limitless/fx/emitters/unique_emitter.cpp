#include <limitless/fx/emitters/unique_emitter.hpp>
#include <tuple>

using namespace Limitless::fx;

bool Limitless::fx::operator<(const UniqueEmitter& lhs, const UniqueEmitter& rhs) noexcept {
    return std::tie(lhs.emitter_type, lhs.module_type, lhs.material_type) < std::tie(rhs.emitter_type, rhs.module_type, rhs.material_type);
}

bool Limitless::fx::operator==(const UniqueEmitter &lhs, const UniqueEmitter &rhs) noexcept {
    return !(lhs < rhs) && !(rhs < lhs);
}

bool Limitless::fx::operator<(const UniqueEmitterShaderKey& lhs, const UniqueEmitterShaderKey& rhs) noexcept {
    return std::tie(lhs.emitter_type, lhs.shader) < std::tie(rhs.emitter_type, rhs.shader);
}

bool Limitless::fx::operator==(const UniqueEmitterShaderKey& lhs, const UniqueEmitterShaderKey& rhs) noexcept {
    return !(lhs < rhs) && !(rhs < lhs);
}
