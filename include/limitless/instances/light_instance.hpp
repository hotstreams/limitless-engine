#pragma once

#include <limitless/instances/instance.hpp>
#include <limitless/pipeline/shader_type.hpp>
#include <limitless/lighting/lighting.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace Limitless {
    template<typename Light>
    class LightInstance : public AbstractInstance {
    private:
		Lighting& lighting;
		uint64_t id;

		auto& getContainer() {
			return static_cast<LightContainer<Light>&>(lighting);
		}

		void synchronize() {
			glm::vec3 translation {0.0f};
			glm::quat rotation {1.0f, 0.0f, 0.0f, 0.0f};
			glm::vec3 scale {1.0f};
			glm::vec3 skew {0.0f};
			glm::vec4 perspective {1.0f};

			glm::decompose(final_matrix, scale, rotation, translation, skew, perspective);
			rotation = glm::conjugate(rotation);

			getLight().position = { translation, 1.0f };
			// getLight().radius = scale;

			// TODO:
			// refactor light container
			// we need to check whether setTranslation/Rotation/Radius changed or not
			// to optimize mapping to gpu
		}

	    void updateBoundingBox() noexcept override {}
    public:
    	LightInstance(Lighting& _lighting)
    	    : AbstractInstance {ModelShader::Model, glm::vec3{0.0f}}
    	    , lighting {_lighting}
    	    , id {getContainer().emplace_back()} {
    	}

    	template<typename... Args>
    	LightInstance(Lighting& _lighting, Args&& ...args)
    	    : AbstractInstance {ModelShader::Model, glm::vec3{0.0f}}
    	    , lighting {_lighting}
    	    , id {getContainer().emplace_back(std::forward<Args>(args)...)} {
		    AbstractInstance::setPosition(getLight().position);
		}

	    LightInstance(Lighting& _lighting, uint64_t _id)
		    : AbstractInstance {ModelShader::Model, static_cast<LightContainer<Light>&>(lighting).at(_id).position}
		    , lighting {_lighting}
		    , id {_id} {
	    }

	    LightInstance(const LightInstance& instance)
		    : AbstractInstance {instance.getShaderType(), instance.getPosition()}
	        , lighting {instance.lighting}
	        , id {getContainer().emplace_back(getContainer().at(instance.id))} {
		}

	    LightInstance(LightInstance&&) = default;

	    ~LightInstance() override {
			getContainer().erase(id);
		}

	    LightInstance* clone() noexcept override {
		    return new LightInstance(*this);
	    }

	    void draw(  [[maybe_unused]] Context& ctx,
			        [[maybe_unused]] const Assets& assets,
			        [[maybe_unused]] ShaderPass shader_type,
			        [[maybe_unused]] ms::Blending blending,
			        [[maybe_unused]] const UniformSetter& uniform_set) override {
		}

	    void update(Context& context, const Camera& camera) override {
		    AbstractInstance::update(context, camera);
		    synchronize();
	    }

	    const auto& getLight() const {
    		return getContainer().at(id);
    	}

	    auto& getLight() {
		    return getContainer().at(id);
	    }
    };
}
