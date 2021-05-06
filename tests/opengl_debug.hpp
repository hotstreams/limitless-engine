#pragma once

#include <limitless/core/context_state.hpp>
#include <limitless/core/state_query.hpp>
#include <limitless/core/state_texture.hpp>

namespace Limitless {
    void check_opengl_state() {
        REQUIRE(gl_error_count == 0);
        gl_error_count = 0;

        if (auto* state = ContextState::getState(glfwGetCurrentContext()); state) {
            StateQuery query;

            REQUIRE(state->getShaderId() == query.geti(QueryState::CurrentProgram));
            REQUIRE(state->getVertexArrayId() == query.geti(QueryState::ArrayBufferBinding));
            REQUIRE(state->getActiveTexture() == (query.geti(QueryState::ActiveTexture) - GL_TEXTURE0));

            const auto last_active_texture = state->getActiveTexture();
            for (const auto& [unit, id] : state->getTextureBound()) {
                StateTexture::activate(unit);
                REQUIRE(id == query.geti(QueryState::TextureBinding2D));
            }
            StateTexture::activate(last_active_texture);
        }
    }
}