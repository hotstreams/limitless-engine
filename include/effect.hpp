#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include <glm/glm.hpp>

namespace GraphicsEngine {
    class Emitter;
    class SpriteEmitter;
    class MeshEmitter;

    class Effect {
    private:
        std::unordered_map<std::string, std::unique_ptr<Emitter>> emitters;
    public:
        ~Effect() = default;

        Effect(const Effect& e) noexcept;
        Effect& operator=(const Effect& e) noexcept;

        Effect(Effect&&) noexcept = default;
        Effect& operator=(Effect&&) noexcept = default;

        void setPosition(const glm::vec3& position) noexcept;
        void setRotation(const glm::vec3& rotation) noexcept;

        bool isDone() const noexcept;

        [[nodiscard]] const auto& getEmitters() const noexcept { return emitters; }

        SpriteEmitter& getSpriteEmitter(const std::string& emitter);
        MeshEmitter& getMeshEmitter(const std::string& emitter);

        void update();
    };
}