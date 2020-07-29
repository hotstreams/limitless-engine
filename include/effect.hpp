#pragma once

#include <sprite_emitter.hpp>
#include <mesh_emitter.hpp>

namespace GraphicsEngine {
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

        const auto& getEmitters() const noexcept { return emitters; }

        SpriteEmitter& getSpriteEmitter(const std::string& emitter);
        MeshEmitter& getMeshEmitter(const std::string& emitter);

        void update();
    };
}