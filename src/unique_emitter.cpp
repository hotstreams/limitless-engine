#include <unique_emitter.hpp>

using namespace GraphicsEngine;

bool GraphicsEngine::operator<(const UniqueSpriteEmitter& lhs, const UniqueSpriteEmitter& rhs) noexcept {
    return std::tie(lhs.modules, lhs.material) < std::tie(rhs.modules, rhs.material);
}

bool GraphicsEngine::operator<(const UniqueMeshEmitter& lhs, const UniqueMeshEmitter& rhs) noexcept {
    return std::tie(lhs.modules, lhs.material, lhs.mesh) < std::tie(rhs.modules, rhs.material, rhs.mesh);
}

bool GraphicsEngine::operator==(const UniqueSpriteEmitter &lhs, const UniqueSpriteEmitter &rhs) noexcept {
    return !(lhs < rhs) && !(rhs < lhs);
}

bool GraphicsEngine::operator==(const UniqueMeshEmitter &lhs, const UniqueMeshEmitter &rhs) noexcept {
    return !(lhs < rhs) && !(rhs < lhs);
}
