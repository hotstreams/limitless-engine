#include <limitless/fx/emitters/sprite_emitter.hpp>

#include <limitless/ms/material.hpp>
#include <limitless/fx/emitters/emitter_visitor.hpp>

using namespace Limitless::fx;

SpriteEmitter::SpriteEmitter() noexcept
    : Emitter<>(Type::Sprite) {
}

SpriteEmitter::SpriteEmitter(const SpriteEmitter& emitter)
    : Emitter<>(emitter)
    , material {std::make_shared<ms::Material>(*emitter.material)} {
}

SpriteEmitter* SpriteEmitter::clone() const {
    return new SpriteEmitter(*this);
}

void SpriteEmitter::accept(EmitterVisitor& visitor) noexcept {
    visitor.visit(*this);
}

UniqueEmitterRenderer SpriteEmitter::getUniqueRendererType() const noexcept {
    return { type, std::nullopt, material.get() };
}
