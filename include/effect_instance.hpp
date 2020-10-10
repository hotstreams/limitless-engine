#pragma once

#include <memory>
#include <string>
#include <glm/glm.hpp>
#include <abstract_instance.hpp>
#include <unordered_map>

namespace GraphicsEngine {
    class Emitter;
    class SpriteEmitter;
    class MeshEmitter;

    class EffectInstance : public AbstractInstance {
    private:
        std::unordered_map<std::string, std::unique_ptr<Emitter>> emitters;

        friend void swap(EffectInstance&, EffectInstance&) noexcept;
        bool isDone() const noexcept;
    public:
        ~EffectInstance() override;

        EffectInstance(const EffectInstance&) noexcept;
        EffectInstance& operator=(const EffectInstance&) noexcept;

        EffectInstance(EffectInstance&&) = default;
        EffectInstance& operator=(EffectInstance&&) = default;

        EffectInstance* clone() noexcept override;

        EffectInstance& setPosition(const glm::vec3& position) noexcept override;
        EffectInstance& setRotation(const glm::vec3& rotation) noexcept override;

        SpriteEmitter& getSpriteEmitter(const std::string& emitter);
        MeshEmitter& getMeshEmitter(const std::string& emitter);

        auto begin() noexcept { return emitters.begin(); }
        auto end() noexcept { return emitters.end(); }

        auto begin() const noexcept { return emitters.cbegin(); }
        auto end() const noexcept { return emitters.cend(); }

        Emitter& operator[](const std::string& emitter);

        void update() override;

        void draw(MaterialShader shader_type, Blending blending, const UniformSetter& uniform_set) override;
    };

    void swap(EffectInstance&, EffectInstance&) noexcept;
}