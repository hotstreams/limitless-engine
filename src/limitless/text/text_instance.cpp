#include <limitless/text/text_instance.hpp>

#include <limitless/ms/blending.hpp>
#include "limitless/core/shader/shader_program.hpp"
#include "limitless/core/uniform/uniform.hpp"
#include <limitless/core/context.hpp>
#include <limitless/text/font_atlas.hpp>
#include <limitless/assets.hpp>

#include <glm/gtx/transform.hpp>

using namespace Limitless;

TextInstance::TextInstance(std::string _text, const glm::vec2& _position, std::shared_ptr<FontAtlas> _font)
    : text_model {_font->generate(_text)}
    , text {std::move(_text)}
    , position {_position}
    , font {std::move(_font)} {
    calculateModelMatrix();
}

TextInstance::TextInstance(size_t count, const glm::vec2& _position, std::shared_ptr<FontAtlas> _font)
    : text_model {count}
    , text {}
    , position {_position}
    , font {std::move(_font)} {
    calculateModelMatrix();
}

void TextInstance::calculateModelMatrix() noexcept {
    const auto translation_matrix = glm::translate(glm::mat4{1.0f}, glm::vec3{position, 0.0f});
    const auto scaling_matrix = glm::scale(glm::mat4{1.0f}, glm::vec3{size, 0.0f});

    model_matrix = translation_matrix * scaling_matrix;
}

TextInstance& TextInstance::setText(std::string _text) {
    if (text != _text) {
        text = std::move(_text);
        text_model.update(font->generate(text));
    }
    return *this;
}

TextInstance& TextInstance::setColor(const glm::vec4& _color) noexcept {
    color = _color;
    return *this;
}

TextInstance& TextInstance::setSelectionColor(const glm::vec4& _color) noexcept {
    selection_color = _color;
    return *this;
}

TextInstance& TextInstance::setSelection(size_t begin, size_t end) {
    selection_model = TextModel{font->getSelectionGeometry(text, begin, end)};
    return *this;
}

TextInstance& TextInstance::removeSelection() noexcept {
    selection_model = std::nullopt;
    return *this;
}

TextInstance& TextInstance::setPosition(const glm::vec2& _position) noexcept {
    position = _position;
    calculateModelMatrix();
    return *this;
}

TextInstance& TextInstance::setSize(const glm::vec2& _size) noexcept {
    size = _size;
    calculateModelMatrix();
    return *this;
}

void TextInstance::draw(Context& ctx, const Assets& assets) {
    ctx.disable(Capabilities::DepthTest);

    // draw selection
    if (selection_model) {
        ctx.disable(Capabilities::Blending);

        auto& shader = assets.shaders.get("text_selection");

        shader.setUniform("model", model_matrix)
              .setUniform("proj", glm::ortho(0.0f, static_cast<float>(ctx.getSize().x), 0.0f, static_cast<float>(ctx.getSize().y)))
              .setUniform("color", selection_color);

        shader.use();

        selection_model->draw();
    }

    // draw text
    {
        auto& shader = assets.shaders.get("text");

        setBlendingMode(ms::Blending::Text);

        shader.setUniform("bitmap", font->getTexture())
              .setUniform("model", model_matrix)
              .setUniform("proj", glm::ortho(0.0f, static_cast<float>(ctx.getSize().x), 0.0f, static_cast<float>(ctx.getSize().y)))
              .setUniform("color", color);

        shader.use();

        text_model.draw();
    }
}