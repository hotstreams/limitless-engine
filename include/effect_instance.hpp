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

        friend class EffectBuilder;
        EffectInstance() noexcept;
    public:
        EffectInstance(Lighting* lighting, const std::shared_ptr<EffectInstance>& effect, const glm::vec3& position, const glm::vec3& rotation = glm::vec3{0.f}) noexcept;
        EffectInstance(const std::shared_ptr<EffectInstance>& effect, const glm::vec3& position, const glm::vec3& rotation = glm::vec3{0.f}) noexcept;
        ~EffectInstance() override = default;

        EffectInstance(const EffectInstance&) noexcept;
        EffectInstance& operator=(const EffectInstance&) noexcept;

        EffectInstance(EffectInstance&&) noexcept = default;
        EffectInstance& operator=(EffectInstance&&) noexcept = default;

        EffectInstance* clone() noexcept override;

        EffectInstance& setPosition(const glm::vec3& position) noexcept override;
        EffectInstance& setRotation(const glm::vec3& rotation) noexcept override;

        template<typename T>
        const T& get(const std::string& name) const {
            try {
                return static_cast<T&>(*emitters.at(name));
            } catch(...) {
                throw std::runtime_error("No such emitter " + name);
            }
        }

        template<typename T>
        T& get(const std::string& name) {
            try {
                return static_cast<T&>(*emitters.at(name));
            } catch (...) {
                throw std::runtime_error("No such emitter " + name);
            }
        }

        auto begin() noexcept { return emitters.begin(); }
        auto end() noexcept { return emitters.end(); }

        auto begin() const noexcept { return emitters.cbegin(); }
        auto end() const noexcept { return emitters.cend(); }

        void update() override;

        void draw(MaterialShader shader_type, Blending blending, const UniformSetter& uniform_set) override;
    };

    void swap(EffectInstance&, EffectInstance&) noexcept;
}