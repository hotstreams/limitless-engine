#pragma once

#include <limitless/instances/abstract_instance.hpp>
#include <limitless/shader_types.hpp>
#include <limitless/font_atlas.hpp>
#include <utility>
#include <limitless/models/mesh.hpp>
#include <limitless/core/context.hpp>

namespace LimitlessEngine {
    class TextInstance : public AbstractInstance {
    private:
        std::shared_ptr<FontAtlas> font;
        std::string string;
        glm::vec4 color;
        glm::vec4 selection_color;

        Mesh<TextVertex> mesh;
        std::optional<Mesh<TextVertex>> selection_mesh;
        Context& ctx;

        void updateVertices();
    public:
        template<typename str>
        TextInstance(str&& text, glm::vec2 _position, glm::vec2 _scale, const glm::vec4& _color, std::shared_ptr<FontAtlas> _font, Context& _ctx)
            : AbstractInstance{ModelShader::Text, glm::vec3{_position, 0.0f}, glm::vec3{0.f}, glm::vec3{_scale, 0.0f}}, font{std::move(_font)}, string{std::forward<str>(text)}, color{_color}, mesh{"text", MeshDataType::Dynamic, DrawMode::Triangles}, ctx{_ctx} {
                updateVertices();
            }

        template<typename str>
        TextInstance& setText(str&& text) noexcept {
            if (string != text) {
                string = std::forward<str>(text);
                updateVertices();
            }

            return *this;
        }

        TextInstance& setColor(const glm::vec4& _color) noexcept {
            color = _color;
            return *this;
        }

        [[nodiscard]] auto& getColor() const noexcept { return color; }

        TextInstance* clone() noexcept override;

        void setSelection(size_t begin, size_t end) {
            selection_mesh = {"text_selection", MeshDataType::Dynamic, DrawMode::Triangles};

            selection_mesh->updateVertices(font->getSelectionGeometry(string, begin, end));
        }

        void removeSelection() {
            selection_mesh = std::nullopt;
        }

        void setSelectionColor(const glm::vec4& sel_color) {
            selection_color = sel_color;
        }

        using AbstractInstance::draw;
        void draw(const Assets& assets, MaterialShader shader_type, Blending blending, const UniformSetter& uniform_set) override;
    };
}