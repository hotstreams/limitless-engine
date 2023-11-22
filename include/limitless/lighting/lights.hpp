#pragma once

#include <glm/glm.hpp>
#include <stdexcept>

namespace Limitless {
    class light_builder_exception final : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    class Light final {
    public:
        static constexpr auto SHADER_STORAGE_NAME = "LIGHTS_BUFFER";

        enum class Type : uint32_t {
            DirectionalLight,
            Point,
            Spot
        };
    private:
        glm::vec4 color {};
        glm::vec4 position {};
        glm::vec4 direction {};
        glm::vec2 scale_offset {};
        float falloff {};
        Type type {1u};

        static float radiusToFalloff(float r) {
            return 1.0f / (r * r);
        }

        static float falloffToRadius(float f) {
            return glm::sqrt(1.0f / f);
        }

        static glm::vec2 anglesToScaleOffset(float inner_angle, float outer_angle) {
            const auto inner = glm::radians(inner_angle);
            const auto outer = glm::radians(outer_angle);

            const auto inner_cos = glm::cos(inner);
            const auto outer_cos = glm::cos(outer);
            const auto scale = 1.0f / glm::max(1.0f / 1024.0f, inner_cos - outer_cos);
            const auto offset = -outer_cos * scale;

            return { scale, offset };
        }

        static glm::vec2 scaleOffsetToAngles(const glm::vec2& scale_offset) {
            const auto outer_cos = -scale_offset.y / scale_offset.x;
            const auto inner_cos = 1.0f / (scale_offset.x + 1.0f / 1024.0f);

            const auto outer_angle = glm::degrees(glm::acos(outer_cos));
            const auto inner_angle = glm::degrees(glm::acos(inner_cos));

            return { inner_angle, outer_angle };
        }

        Light(const glm::vec4& color, const glm::vec3& position, float radius) noexcept
            : color {color}
            , position {position, 0.0f}
            , direction {}
            , scale_offset {}
            , falloff {radiusToFalloff(radius)}
            , type {Type::Point} {
        }

        Light(const glm::vec4& color, const glm::vec3& position, const glm::vec3& direction, float inner_angle, float outer_angle, float radius) noexcept
                : color {color}
                , position {position, 0.0f}
                , direction {direction, 0.0f}
                , scale_offset {anglesToScaleOffset(inner_angle, outer_angle)}
                , falloff {radiusToFalloff(radius)}
                , type {Type::Spot} {
        }

        Light(const glm::vec4& color, const glm::vec3& direction) noexcept
                : color {color}
                , position {}
                , direction {direction, 0.0f}
                , scale_offset {}
                , falloff {}
                , type {Type::DirectionalLight} {
        }
    public:
        [[nodiscard]] const glm::vec4& getColor() const noexcept { return color; }
        void setColor(const glm::vec4& _color) noexcept { color = _color; }

        [[nodiscard]] glm::vec3 getPosition() const noexcept { return position; }
        void setPosition(const glm::vec3& _position) noexcept { position = glm::vec4(_position, 0.0f); }

        [[nodiscard]] glm::vec3 getDirection() const noexcept { return direction; }
        void setDirection(const glm::vec3& _direction) noexcept { direction = glm::vec4(_direction, 0.0f); }

        [[nodiscard]] glm::vec2 getCone() const noexcept { return scaleOffsetToAngles(scale_offset); }
        void setCone(float inner_angle, float outer_angle) noexcept { scale_offset = anglesToScaleOffset(inner_angle, outer_angle); }

        [[nodiscard]] float getRadius() const noexcept { return glm::sqrt(1.0f / falloff); }
        void setRadius(float _radius) noexcept { falloff = 1.0f / (_radius * _radius); }

        [[nodiscard]] Type getType() const noexcept { return type; }

        class Builder {
        private:
            glm::vec4 color_ {0.0f};
            glm::vec3 position_ {0.0f};
            float outer_angle_ {0.0f};
            float inner_angle_ {0.0f};
            glm::vec3 direction_ {0.0f};
            float radius_ {0.0f};

            bool isSpot() {
                return color_ != glm::vec4(0.0f) && position_ != glm::vec3(0.0) && inner_angle_ != 0.0f && outer_angle_ != 0.0f && direction_ != glm::vec3(0.0f) && radius_ != 0.0f;
            }

            bool isPoint() {
                return color_ != glm::vec4(0.0f) && position_ != glm::vec3(0.0) && radius_ != 0.0f;
            }

            bool isDirectional() {
                return color_ != glm::vec4(0.0f) && direction_ != glm::vec3(0.0f);
            }
        public:
            Builder& color(const glm::vec4& _color) { color_ = _color; return *this; }
            Builder& position(const glm::vec3& _position) { position_ = _position; return *this; }
            Builder& direction(const glm::vec3& _direction) { direction_ = _direction; return *this; }
            Builder& cone(float outer_angle, float inner_angle) { outer_angle_ = outer_angle; inner_angle_ = inner_angle; return *this; }
            Builder& radius(float _radius) { radius_ = _radius; return *this; }

            Light build() {
                if (isSpot()) {
                    if (inner_angle_ < 0.00872665f || inner_angle_ > 90.0f || outer_angle_ < inner_angle_ || outer_angle_ > 90.0f) {
                        throw light_builder_exception {"Inner/Outer angles are out of range!"};
                    }
                    return {color_, position_, direction_, inner_angle_, outer_angle_, radius_};
                }

                if (isPoint()) {
                    return {color_, position_, radius_};
                }

                if (isDirectional()) {
                    return {color_, direction_};
                }

                throw light_builder_exception {"Required parameters are not set!"};
            }

            static Builder builder() {
                return {};
            }
        };
    };
}
