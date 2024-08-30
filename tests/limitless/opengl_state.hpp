#pragma once

#include <limitless/core/context.hpp>
#include <limitless/core/state_query.hpp>
#include <limitless/core/texture/state_texture.hpp>

namespace LimitlessTest {
    using namespace Limitless;

    //TODO: move framebuffer draw buffers state to ContextState
    // and add check here
    inline void check_opengl_state() {
        SECTION("checking opengl debug errors") {
            REQUIRE(gl_error_count == 0);
        }

        SECTION("checking state") {
            if (auto* state = Context::getCurrentContext(); state) {
                StateQuery query;

                REQUIRE(state->getCapabilities().at(Capabilities::Blending) == query.getb(QueryState::Blend));
                REQUIRE(state->getCapabilities().at(Capabilities::CullFace) == query.getb(QueryState::CullFace));
                REQUIRE(state->getCapabilities().at(Capabilities::DepthTest) == query.getb(QueryState::DepthTest));
                REQUIRE(state->getCapabilities().at(Capabilities::ProgramPointSize) == query.getb(QueryState::ProgramPointSize));
                REQUIRE(state->getCapabilities().at(Capabilities::ScissorTest) == query.getb(QueryState::ScissorTest));
                REQUIRE(state->getCapabilities().at(Capabilities::StencilTest) == query.getb(QueryState::StencilTest));

                REQUIRE((int)state->getShaderId() == query.geti(QueryState::CurrentProgram));
                REQUIRE((int)state->getVertexArrayId() == query.geti(QueryState::VertexArrayObject));
                REQUIRE(state->getBlendColor() == query.get4f(QueryState::BlendColor));
                REQUIRE(state->getClearColor() == query.get4f(QueryState::ClearColor));
                REQUIRE((int)state->getCullFace() == query.geti(QueryState::CullFaceMode));
                REQUIRE((int)state->getDepthFunc() == query.geti(QueryState::DepthFunc));
                REQUIRE((int)state->getDepthMask() == query.geti(QueryState::DepthMask));
                REQUIRE((int)state->getDepthMask() == query.geti(QueryState::DepthMask));

                REQUIRE((int)state->getBufferTargets().at(Buffer::Type::Element) == query.geti(QueryState::ElementArrayBufferBinding));
                REQUIRE((int)state->getBufferTargets().at(Buffer::Type::Array) == query.geti(QueryState::ArrayBufferBinding));
                REQUIRE((int)state->getBufferTargets().at(Buffer::Type::ShaderStorage) == query.geti(QueryState::ShaderStorageBufferBinding));
                REQUIRE((int)state->getBufferTargets().at(Buffer::Type::Uniform) == query.geti(QueryState::UniformBufferBinding));

                REQUIRE(state->getLineWidth() == query.getf(QueryState::LineWidth));

                REQUIRE((int)state->getActiveTexture() == (query.geti(QueryState::ActiveTexture) - GL_TEXTURE0));
                const auto last_active_texture = state->getActiveTexture();
                for (const auto& [unit, id] : state->getTextureBound()) {
                    StateTexture::activate(unit);

                    bool a = (int)id == query.geti(QueryState::TextureBinding2D)
                             || (int)id == query.geti(QueryState::TextureBinding3D)
                             || (int)id == query.geti(QueryState::TextureBinding2DArray)
                             || (int)id == query.geti(QueryState::TextureBindingCubeMap);
                    REQUIRE(a);
                }
                StateTexture::activate(last_active_texture);

                for (const auto& [point, id] : state->getBufferPoints()) {
                    if (point.target == Buffer::Type::ShaderStorage) {
                        REQUIRE((int)id == query.getiForIndex(QueryState::ShaderStorageBufferBinding, point.point));
                    }
                    if (point.target == Buffer::Type::Uniform) {
                        REQUIRE((int)id == query.getiForIndex(QueryState::UniformBufferBinding, point.point));
                    }
                }
            }
        }

        SECTION("clean up state") {
            gl_error_count = 0;
        }
    }
}