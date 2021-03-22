#include <limitless/particle_system/unique_emitter.hpp>

using namespace LimitlessEngine;

bool LimitlessEngine::operator<(const UniqueSpriteEmitter& lhs, const UniqueSpriteEmitter& rhs) noexcept {
    return std::tie(lhs.modules, lhs.material) < std::tie(rhs.modules, rhs.material);
}

bool LimitlessEngine::operator<(const UniqueMeshEmitter& lhs, const UniqueMeshEmitter& rhs) noexcept {
    return std::tie(lhs.modules, lhs.material, lhs.mesh) < std::tie(rhs.modules, rhs.material, rhs.mesh);
}

bool LimitlessEngine::operator==(const UniqueSpriteEmitter &lhs, const UniqueSpriteEmitter &rhs) noexcept {
    return !(lhs < rhs) && !(rhs < lhs);
}

bool LimitlessEngine::operator==(const UniqueMeshEmitter &lhs, const UniqueMeshEmitter &rhs) noexcept {
    return !(lhs < rhs) && !(rhs < lhs);
}
