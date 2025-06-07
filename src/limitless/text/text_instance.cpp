#include <limitless/text/text_instance.hpp>

#include <limitless/ms/blending.hpp>
#include "limitless/core/shader/shader_program.hpp"
#include "limitless/core/uniform/uniform.hpp"
#include <limitless/core/context.hpp>
#include <limitless/text/font_atlas.hpp>
#include <limitless/text/type_setter.hpp>
#include <limitless/assets.hpp>

#include <glm/gtx/transform.hpp>

using namespace Limitless;

TextInstance::TextInstance(
    std::vector<FormattedText> _formatted_text_parts,
    const glm::vec2& _position
)
    : formatted_text_parts {std::move(_formatted_text_parts)}
    , position {_position}
{
    auto type_set_result = TypeSetter::typeSet(formatted_text_parts);
    bounding_box = type_set_result.bounding_box;
    for (auto& fv : type_set_result.vertices_of_fonts) {
        font_text_models.push_back(FontTextModel{
            TextModel(std::move(fv.vertices)), fv.font
        });
    }
    calculateModelMatrix();
}

void TextInstance::calculateModelMatrix() noexcept {
    const auto translation_matrix = glm::translate(glm::mat4{1.0f}, glm::vec3{position, 0.0f});
    const auto scaling_matrix = glm::scale(glm::mat4{1.0f}, glm::vec3{size, 0.0f});

    model_matrix = translation_matrix * scaling_matrix;
}

TextInstance& TextInstance::setText(std::string _text, TextFormat _text_format) {
    return setText({FormattedText(std::move(_text), std::move(_text_format))});
}

TextInstance& TextInstance::setText(std::vector<FormattedText> _formatted_text_parts) {
    if (formatted_text_parts != _formatted_text_parts) {
        formatted_text_parts = std::move(_formatted_text_parts);

        auto type_set_result = TypeSetter::typeSet(formatted_text_parts);
        font_text_models.clear();

        bounding_box = type_set_result.bounding_box;
        for (auto& fv : type_set_result.vertices_of_fonts) {
            font_text_models.push_back(FontTextModel{
                TextModel(std::move(fv.vertices)), fv.font
            });
        }
    }
    return *this;
}

TextInstance& TextInstance::setSelectionColor(const glm::vec4& _color) noexcept {
    selection_color = _color;
    return *this;
}

TextInstance& TextInstance::setSelection(size_t begin, size_t end) {
    selection_model = TextSelectionModel{TypeSetter::typeSetSelection(formatted_text_parts, begin, end)};
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

glm::vec2 TextInstance::getBoundingBoxDimensions() const noexcept {
    const auto& [min, max] = bounding_box;
    return (max - min) * size;
}

void TextInstance::draw(Context& ctx, const Assets& assets) {
    ctx.disable(Capabilities::DepthTest);

    const auto ortho_projection = glm::ortho(
        0.0f,
        static_cast<float>(ctx.getSize().x),
        0.0f,
        static_cast<float>(ctx.getSize().y)
    );

    // draw selection
    if (selection_model) {
        ctx.disable(Capabilities::Blending);

        auto& shader = assets.shaders.get("text_selection");

        shader.setUniform("model", model_matrix)
              .setUniform("proj", ortho_projection)
              .setUniform("color", selection_color);

        shader.use();

        selection_model->draw();
    }

    // draw text
    for (auto& ftm : font_text_models) {
        auto& text_shader = ftm.font_atlas->isIconAtlas()
            ? assets.shaders.get("icon_text")
            : assets.shaders.get("text");
        setBlendingMode(ms::Blending::Text);

        text_shader
              .setUniform("bitmap", ftm.font_atlas->getTexture())
              .setUniform("model", model_matrix)
              .setUniform("proj", ortho_projection);

        text_shader.use();

        ftm.text_model.draw();
    }
}
