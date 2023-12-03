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
        enum class Type {
            DirectionalLight,
            Point,
            Spot
        };
    private:
        static inline uint64_t next_id = 0u;
        glm::vec4 color;
        glm::vec3 position;
        glm::vec3 direction;
        glm::vec2 inner_outer_angles;
        uint64_t id;
        float radius;
        Type type;
        bool changed;
        bool removed;
        bool hidden;

        Light(const glm::vec4& color, const glm::vec3& position, float radius) noexcept;
        Light(const glm::vec4& color, const glm::vec3& position, const glm::vec3& direction, const glm::vec2& inner_outer_angles, float radius) noexcept;
        Light(const glm::vec4& color, const glm::vec3& direction) noexcept;
    public:
        Light(const Light& light) noexcept;
        Light(Light&& light) noexcept;

        Light& operator=(const Light& light) noexcept;
        Light& operator=(Light&& light) noexcept;

        [[nodiscard]] const glm::vec4& getColor() const noexcept;
        void setColor(const glm::vec4& _color) noexcept;

        [[nodiscard]] glm::vec3 getPosition() const noexcept;
        [[nodiscard]] glm::vec3& getPosition() noexcept;
        void setPosition(const glm::vec3& _position) noexcept;

        [[nodiscard]] glm::vec3 getDirection() const noexcept;
        void setDirection(const glm::vec3& _direction) noexcept;

        [[nodiscard]] glm::vec2 getCone() const noexcept;
        void setCone(const glm::vec2& _inner_outer_angles) noexcept;

        [[nodiscard]] float getRadius() const noexcept;
        void setRadius(float _radius) noexcept;

        [[nodiscard]] Type getType() const noexcept;
        [[nodiscard]] bool isDirectional() const noexcept;
        [[nodiscard]] bool isPoint() const noexcept;
        [[nodiscard]] bool isSpot() const noexcept;

        [[nodiscard]] uint64_t getId() const noexcept;

        [[nodiscard]] bool isChanged() const noexcept;
        void resetChanged() noexcept;
        void change() noexcept;

        [[nodiscard]] bool isRemoved() const noexcept;
        void remove() noexcept;

        [[nodiscard]] bool isHidden() const noexcept;
        void hide() noexcept;
        void reveal() noexcept;

        class Builder {
        private:
            glm::vec4 color_ {0.0f};
            glm::vec3 position_ {0.0f};
            float outer_angle_ {0.0f};
            float inner_angle_ {0.0f};
            glm::vec3 direction_ {0.0f};
            float radius_ {0.0f};

            bool isSpot();
            bool isPoint();
            bool isDirectional();
        public:
            Builder& color(const glm::vec4& color) noexcept;
            Builder& position(const glm::vec3& position) noexcept;
            Builder& direction(const glm::vec3& direction) noexcept;
            Builder& cone(float outer_angle, float inner_angle) noexcept;
            Builder& radius(float radius) noexcept;

            Light build();
            Light buildDefaultDirectional();
        };

        static Builder builder();
    };
}
