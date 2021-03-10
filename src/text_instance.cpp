#include <text_instance.hpp>
#include <shader_storage.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <material_system/material.hpp>

using namespace LimitlessEngine;

void TextInstance::updateVertices() {
    mesh.updateVertices(font->generate(string));
}

void TextInstance::draw([[maybe_unused]] MaterialShader shader_type, Blending blending, [[maybe_unused]] const UniformSetter& uniform_set) {
    if (hidden || blending != Blending::Translucent) return;

    calculateModelMatrix();

    ctx.disable(Enable::DepthTest);
    ctx.disable(Enable::Blending);

    // draw selection
    if (selection_mesh) {
        auto& shader = *shader_storage.get("text_selection");

        shader << UniformValue{"model", model_matrix}
               << UniformValue{"proj", glm::ortho(0.0f, static_cast<float>(ctx.getSize().x), 0.0f, static_cast<float>(ctx.getSize().y))}
               << UniformValue{"color", selection_color};

        shader.use();

        selection_mesh->draw();
    }

    // draw text
    {
        auto& shader = *shader_storage.get("text");

        setBlendingMode(Blending::Text);

        shader << UniformSampler{"bitmap", font->getTexture()}
               << UniformValue{"model", model_matrix}
               << UniformValue{"proj", glm::ortho(0.0f, static_cast<float>(ctx.getSize().x), 0.0f, static_cast<float>(ctx.getSize().y))}
               << UniformValue{"color", color};

        shader.use();

        mesh.draw();
    }
}

TextInstance* TextInstance::clone() noexcept {
    return new TextInstance(string, position, scale, color, font, ctx);
}