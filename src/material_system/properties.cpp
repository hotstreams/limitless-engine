#include <limitless/material_system/properties.hpp>
#include <limitless/core/context_state.hpp>

using namespace LimitlessEngine;

void LimitlessEngine::setBlendingMode(ContextState& context, Blending blending) noexcept {
    switch (blending) {
        case Blending::Opaque:
            context.enable(Capabilities::DepthTest);
            context.setDepthFunc(DepthFunc::Less);
            context.setDepthMask(DepthMask::True);
            context.disable(Capabilities::Blending);
            break;
        case Blending::Additive:
            context.enable(Capabilities::DepthTest);
            context.setDepthFunc(DepthFunc::Less);
            context.setDepthMask(DepthMask::False);
            context.enable(Capabilities::Blending);
            context.setBlendFunc(BlendFactor::One, BlendFactor::One);
            break;
        case Blending::Modulate:
            context.enable(Capabilities::DepthTest);
            context.setDepthFunc(DepthFunc::Less);
            context.setDepthMask(DepthMask::False);
            context.enable(Capabilities::Blending);
            context.setBlendFunc(BlendFactor::DstColor, BlendFactor::Zero);
            break;
        case Blending::Translucent:
            context.enable(Capabilities::DepthTest);
            context.setDepthFunc(DepthFunc::Less);
            context.setDepthMask(DepthMask::False);
            context.enable(Capabilities::Blending);
            context.setBlendFunc(BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha);
            break;
        case Blending::OpaqueHalf:
            context.enable(Capabilities::DepthTest);
            context.setDepthFunc(DepthFunc::Lequal);
            context.setDepthMask(DepthMask::True);
            context.enable(Capabilities::Blending);
            context.setBlendColor({0.5f, 0.5f, 0.5f, 1.0f});
            context.setBlendFunc(BlendFactor::BlendColor, BlendFactor::BlendColor);
            break;
	    case Blending::Text:
		    context.disable(Capabilities::DepthTest);
		    context.setDepthFunc(DepthFunc::Less);
		    context.setDepthMask(DepthMask::False);
		    context.enable(Capabilities::Blending);
		    context.setBlendFunc(BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha);
		    break;
    }
}

void LimitlessEngine::setBlendingMode(Blending blending) noexcept {
    if (auto* state = ContextState::getState(glfwGetCurrentContext()); state) {
        setBlendingMode(*state, blending);
    }
}
