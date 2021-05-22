#include <limitless/fx/emitters/unique_emitter.hpp>
#include <tuple>

#include <limitless/ms/material.hpp>

using namespace Limitless::fx;

bool Limitless::fx::operator<(const UniqueEmitterShader& lhs, const UniqueEmitterShader& rhs) noexcept {
    return std::tie(lhs.emitter_type, lhs.module_type, lhs.material_type) < std::tie(rhs.emitter_type, rhs.module_type, rhs.material_type);
}

bool Limitless::fx::operator==(const UniqueEmitterShader &lhs, const UniqueEmitterShader &rhs) noexcept {
    return !(lhs < rhs) && !(rhs < lhs);
}

bool Limitless::fx::operator<(const UniqueEmitterShaderKey& lhs, const UniqueEmitterShaderKey& rhs) noexcept {
    return std::tie(lhs.emitter_type, lhs.shader) < std::tie(rhs.emitter_type, rhs.shader);
}

bool Limitless::fx::operator==(const UniqueEmitterShaderKey& lhs, const UniqueEmitterShaderKey& rhs) noexcept {
    return !(lhs < rhs) && !(rhs < lhs);
}

bool Limitless::fx::operator<(const UniqueEmitterRenderer& lhs, const UniqueEmitterRenderer& rhs) noexcept {
    return std::tie(lhs.emitter_type, lhs.mesh, *lhs.material) < std::tie(rhs.emitter_type, rhs.mesh, *rhs.material);
}

bool Limitless::fx::operator==(const UniqueEmitterRenderer& lhs, const UniqueEmitterRenderer& rhs) noexcept {
    return !(lhs < rhs) && !(rhs < lhs);
}
