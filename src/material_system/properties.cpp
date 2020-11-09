#include <material_system/properties.hpp>
#include <core/context_state.hpp>

using namespace GraphicsEngine;

void GraphicsEngine::setBlendingMode(ContextState& context, Blending blending) noexcept {
    switch (blending) {
        case Blending::Opaque:
            context.enable(Enable::DepthTest);
            context.setDepthFunc(DepthFunc::Less);
            context.setDepthMask(DepthMask::True);
            context.disable(Enable::Blending);
            break;
        case Blending::Additive:
            context.enable(Enable::DepthTest);
            context.setDepthFunc(DepthFunc::Less);
            context.setDepthMask(DepthMask::False);
            context.enable(Enable::Blending);
            context.setBlendFunc(BlendFactor::One, BlendFactor::One);
            break;
        case Blending::Modulate:
            context.enable(Enable::DepthTest);
            context.setDepthFunc(DepthFunc::Less);
            context.setDepthMask(DepthMask::False);
            context.enable(Enable::Blending);
            context.setBlendFunc(BlendFactor::DstColor, BlendFactor::Zero);
            break;
        case Blending::Translucent:
            context.enable(Enable::DepthTest);
            context.setDepthFunc(DepthFunc::Less);
            context.setDepthMask(DepthMask::False);
            context.enable(Enable::Blending);
            context.setBlendFunc(BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha);
            break;
        case Blending::OpaqueHalf:
            context.enable(Enable::DepthTest);
            context.setDepthFunc(DepthFunc::Lequal);
            context.setDepthMask(DepthMask::True);
            context.enable(Enable::Blending);
            context.setBlendColor({0.5f, 0.5f, 0.5f, 1.0f});
            context.setBlendFunc(BlendFactor::BlendColor, BlendFactor::BlendColor);
            break;
    }
}

void GraphicsEngine::setBlendingMode(Blending blending) noexcept {
    if (auto* state = ContextState::getState(glfwGetCurrentContext()); state) {
        setBlendingMode(*state, blending);
    }
}
