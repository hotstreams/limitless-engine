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
    : selection_model(0)
    , formatted_text_parts {std::move(_formatted_text_parts)}
    , position {_position}
{
    auto type_set_result = TypeSetter::typeSet(formatted_text_parts);
    bounding_box = type_set_result.bounding_box;
    link_rectangles = std::move(type_set_result.link_rectangles);
    for (auto& fv : type_set_result.vertices_of_fonts) {
        font_text_models.push_back(FontTextModel{
            TextModel(std::move(fv.vertices)), fv.font, fv.effects
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
        link_rectangles = std::move(type_set_result.link_rectangles);
        for (auto& fv : type_set_result.vertices_of_fonts) {
            font_text_models.push_back(FontTextModel{
                TextModel(std::move(fv.vertices)), fv.font, fv.effects
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
    auto vertices = TypeSetter::typeSetSelection(formatted_text_parts, begin, end);
    selection_model.update(std::move(vertices));

    return *this;
}

TextInstance& TextInstance::removeSelection() noexcept {
    selection_model.update({});
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
    if (!selection_model.empty()) {
        ctx.disable(Capabilities::Blending);

        auto& shader = assets.shaders.get("text_selection");

        shader.setUniform("model", model_matrix)
                .setUniform("proj", ortho_projection)
                .setUniform("color", selection_color);

        shader.use();

        selection_model.draw();
    }

    setBlendingMode(ms::Blending::Text);

    auto drawSdf = [&](FontTextModel& ftm, int32_t effect_pass) {
        const auto& shadow = ftm.effects.shadow;
        const auto& outline = ftm.effects.outline;
        auto& sdf_shader = assets.shaders.get("sdf_text");

        sdf_shader
            .setUniform("bitmap", ftm.font_atlas->getTexture())
            .setUniform("model", model_matrix)
            .setUniform("proj", ortho_projection)
            .setUniform("spread", static_cast<float>(ftm.font_atlas->getSdfSpread()))
            .setUniform("effect_pass", effect_pass)
            .setUniform("shadow_color", shadow ? shadow->color : glm::vec4(0.f))
            .setUniform("shadow_offset", shadow ? shadow->offset : glm::vec2(0.f))
            .setUniform("shadow_softness", shadow ? shadow->softness : 0.f)
            .setUniform("outline_color", outline ? outline->color : glm::vec4(0.f))
            .setUniform("outline_width", outline ? outline->width : 0.f);

        sdf_shader.use();

        ftm.text_model.draw();
    };

    // Shadows and outlines of all glyphs go first, so that none covers a neighbouring glyph.
    for (auto& ftm : font_text_models) {
        if (ftm.font_atlas->isSdfAtlas() && !ftm.effects.empty()) {
            drawSdf(ftm, 0);
        }
    }

    // draw text
    for (auto& ftm : font_text_models) {
        if (ftm.font_atlas->isSdfAtlas()) {
            drawSdf(ftm, 1);
            continue;
        }

        auto& text_shader = ftm.font_atlas->isIconAtlas()
            ? assets.shaders.get("icon_text")
            : assets.shaders.get("text");

        text_shader
              .setUniform("bitmap", ftm.font_atlas->getTexture())
              .setUniform("model", model_matrix)
              .setUniform("proj", ortho_projection);

        text_shader.use();

        ftm.text_model.draw();
    }
}
