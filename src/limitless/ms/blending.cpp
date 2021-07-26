#include <limitless/ms/blending.hpp>

#include <limitless/core/context_state.hpp>

using namespace Limitless::ms;

void Limitless::ms::setBlendingMode(ContextState& context, Blending blending, [[maybe_unused]] uint32_t opaque_count) noexcept {
    switch (blending) {
        case Blending::Opaque:
//            context.enable(Capabilities::DepthTest);
//            context.setDepthFunc(DepthFunc::Less);
//            context.setDepthMask(DepthMask::True);
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
        case Blending::MultipleOpaque:
            context.enable(Capabilities::DepthTest);
            context.setDepthFunc(DepthFunc::Lequal);
            context.setDepthMask(DepthMask::True);
            context.enable(Capabilities::Blending);
            //TODO: refactor; does not work quite well w/ RTO normals write? only unlit?
            // shading models for layering unreal? multi-pass rendering material then for lit?
            // additive for now
//            context.setBlendColor(glm::vec4(glm::vec3(1.0f / static_cast<float>(opaque_count)), 1.0f));
//            context.setBlendFunc(BlendFactor::BlendColor, BlendFactor::One);
            context.setBlendFunc(BlendFactor::One, BlendFactor::One);
            break;
        case Blending::Text:
            context.disable(Capabilities::DepthTest);
            context.enable(Capabilities::Blending);
            context.setBlendFunc(BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha);
            break;
    }
}

void Limitless::ms::setBlendingMode(Blending blending) noexcept {
    if (auto* state = ContextState::getState(glfwGetCurrentContext()); state) {
        setBlendingMode(*state, blending);
    }
}