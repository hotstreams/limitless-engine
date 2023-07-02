#pragma once

#include <memory>
#include <string>
#include <glm/glm.hpp>
#include <limitless/instances/abstract_instance.hpp>
#include <unordered_map>
#include <stdexcept>
#include <iostream>

namespace Limitless {
    namespace fx {
        class AbstractEmitter;
        class EffectBuilder;
        class SpriteEmitter;
        class MeshEmitter;
    }

    class no_such_emitter : std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };
}

namespace Limitless {
    /**
     * EffectInstance is a class that contains emitters describing some effect
     */
    class EffectInstance : public AbstractInstance {
    private:
        /**
         * Contains [emitter_name, emitter]
         */
        std::unordered_map<std::string, std::unique_ptr<fx::AbstractEmitter>> emitters;

        /**
         * Effect name
         */
        std::string name;

        /**
         * Checks if effect is finished
         */
        bool isDone() const noexcept;

        void updateBoundingBox() noexcept override;
        void updateEmitters(Context& context, const Camera& camera) const noexcept;

        friend class fx::EffectBuilder;
        friend class EffectSerializer;
        EffectInstance() noexcept;
    public:
        /**
         * Creates specified effect at position
         */
        EffectInstance(const std::shared_ptr<EffectInstance>& effect, const glm::vec3& position) noexcept;
        ~EffectInstance() override = default;

        /**
         * Makes a copy
         */
        EffectInstance(const EffectInstance&) noexcept;
        EffectInstance(EffectInstance&&) noexcept = default;

        /**
         * Makes a copy
         */
        std::unique_ptr<AbstractInstance> clone() noexcept override;

        /**
         * Updates instance and then emitters
         */
        void update(Context& context, const Camera& camera) override;

        /**
         * Does NOT draw effect
         *
         * use EffectRenderer instead
         */
        void draw(Context& ctx, const Assets& assets, ShaderPass shader_type, ms::Blending blending, const UniformSetter& uniform_set) override;

        const auto& getEmitters() const noexcept { return emitters; }
        auto& getEmitters() noexcept { return emitters; }
        const auto& getName() const noexcept { return name; }


        /**
         * Gets emitter with specified name and type
         */
        template<typename EmitterType>
        const EmitterType& get(const std::string& emitter_name) const {
            try {
                return static_cast<EmitterType&>(*emitters.at(emitter_name));
            } catch (...) {
                throw no_such_emitter("with name " + emitter_name);
            }
        }

        /**
         * Gets emitter with specified name and type
         */
        template<typename EmitterType>
        EmitterType& get(const std::string& emitter_name) {
            try {
                return static_cast<EmitterType&>(*emitters.at(emitter_name));
            } catch (...) {
                throw no_such_emitter("with name " + emitter_name);
            }
        }
    };
}