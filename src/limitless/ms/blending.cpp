#include <limitless/ms/blending.hpp>

#include <limitless/core/context_state.hpp>

using namespace Limitless::ms;

void Limitless::ms::setBlendingMode(Blending blending) noexcept {
    if (auto* state = ContextState::getState(glfwGetCurrentContext()); state) {
        switch (blending) {
            case Blending::Opaque:
//            state->enable(Capabilities::DepthTest);
//            state->setDepthFunc(DepthFunc::Less);
//            state->setDepthMask(DepthMask::True);
                state->disable(Capabilities::Blending);
                break;
            case Blending::Additive:
                state->enable(Capabilities::DepthTest);
                state->setDepthFunc(DepthFunc::Less);
                state->setDepthMask(DepthMask::False);
                state->enable(Capabilities::Blending);
                state->setBlendFunc(BlendFactor::One, BlendFactor::One);
                break;
            case Blending::Modulate:
                state->enable(Capabilities::DepthTest);
                state->setDepthFunc(DepthFunc::Less);
                state->setDepthMask(DepthMask::False);
                state->enable(Capabilities::Blending);
                state->setBlendFunc(BlendFactor::DstColor, BlendFactor::Zero);
                break;
            case Blending::Translucent:
                state->enable(Capabilities::DepthTest);
                state->setDepthFunc(DepthFunc::Less);
                state->setDepthMask(DepthMask::False);
                state->enable(Capabilities::Blending);
                state->setBlendFunc(BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha);
                break;
            case Blending::Text:
                state->disable(Capabilities::DepthTest);
                state->enable(Capabilities::Blending);
                state->setBlendFunc(BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha);
                break;
        }
    }
}