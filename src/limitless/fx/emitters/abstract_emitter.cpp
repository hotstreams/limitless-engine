#include <limitless/fx/emitters/abstract_emitter.hpp>

using namespace Limitless::fx;

AbstractEmitter::AbstractEmitter(AbstractEmitter::Type _type) noexcept
    : type {_type} {
}
