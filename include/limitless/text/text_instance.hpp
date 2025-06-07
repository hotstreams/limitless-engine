#pragma once

#include <limitless/models/text_model.hpp>
#include <limitless/models/text_selection_model.hpp>
#include <limitless/text/type_setter.hpp>
#include <string>
#include <optional>

namespace Limitless {
    class FontAtlas;
    class Context;
    class Assets;

    class TextInstance {
    private:
        struct FontTextModel {
            TextModel text_model;
            std::shared_ptr<FontAtlas> font_atlas;
        };

        std::vector<FontTextModel> font_text_models;
        std::optional<TextSelectionModel> selection_model {std::nullopt};

        std::vector<FormattedText> formatted_text_parts;
        glm::vec2 position {0.0f};
        glm::vec4 color {1.0f};
        glm::vec2 size {1.0f};
        std::pair<glm::vec2, glm::vec2> bounding_box {glm::vec2(0.f),glm::vec2(0.f)};

        glm::vec4 selection_color {};
        bool hidden {false};

        glm::mat4 model_matrix {1.0f};
        void calculateModelMatrix() noexcept;
    public:
        TextInstance(std::vector<FormattedText> formatted_text_parts, const glm::vec2& position);

        void hide() noexcept { hidden = true; }
        void reveal() noexcept { hidden = false; }

        TextInstance& setText(std::string text, TextFormat text_format);
        TextInstance& setText(std::vector<FormattedText> new_formatted_text_parts);
        TextInstance& setPosition(const glm::vec2& position) noexcept;
        TextInstance& setSize(const glm::vec2& size) noexcept;
        TextInstance& setSelectionColor(const glm::vec4& color) noexcept;

        TextInstance& setSelection(size_t begin, size_t end);
        TextInstance& removeSelection() noexcept;

        [[nodiscard]] const auto& getColor() const noexcept { return color; }
        [[nodiscard]] const auto& getPosition() const noexcept { return position; }
        [[nodiscard]] auto isHidden() const noexcept { return hidden; }
        [[nodiscard]] auto isVisible() const noexcept { return !hidden; }

        /**
         * In model space.
         */
        [[nodiscard]] const auto getBoundingBox() const noexcept {
            return std::make_pair(
                bounding_box.first * size,
                bounding_box.second * size
            );
        }

        [[nodiscard]] glm::vec2 getBoundingBoxDimensions() const noexcept;

        void draw(Context& context, const Assets& assets);
    };
}
