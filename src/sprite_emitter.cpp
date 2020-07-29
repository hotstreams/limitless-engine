#include <sprite_emitter.hpp>

using namespace GraphicsEngine;

bool GraphicsEngine::operator==(const UniqueSpriteEmitter& lhs, const UniqueSpriteEmitter& rhs) noexcept {
    return lhs.modules == rhs.modules && lhs.material == rhs.material;
}

bool GraphicsEngine::operator<(const UniqueSpriteEmitter& lhs, const UniqueSpriteEmitter& rhs) noexcept {
    return std::tie(lhs.modules, lhs.material) < std::tie(rhs.modules, rhs.material);
}

SpriteEmitter::SpriteEmitter() noexcept
    : Emitter(EmitterType::Sprite) {

}

void GraphicsEngine::swap(SpriteEmitter &lhs, SpriteEmitter &rhs) noexcept {
    swap(static_cast<Emitter&>(lhs), static_cast<Emitter&>(rhs));

    using std::swap;

    swap(lhs.material, rhs.material);
}

SpriteEmitter::SpriteEmitter(SpriteEmitter&& emitter) noexcept
    : Emitter(std::move(emitter)) {
    swap(*this, emitter);
}

SpriteEmitter& SpriteEmitter::operator=(SpriteEmitter&& emitter) noexcept {
    Emitter::operator=(std::move(emitter));

    swap(*this, emitter);
    return *this;
}

SpriteEmitter* SpriteEmitter::clone() const noexcept {
    return new SpriteEmitter(*this);
}

UniqueSpriteEmitter SpriteEmitter::getEmitterType() const noexcept {
    std::vector<EmitterModuleType> mod(modules.size());
    for (const auto& [type, module] : modules) {
        mod.emplace_back(type);
    }
    return { mod, material };
}
