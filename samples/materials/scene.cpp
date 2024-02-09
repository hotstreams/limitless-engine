#include "scene.hpp"

#include <limitless/scene.hpp>
#include <limitless/instances/instanced_instance.hpp>
#include <limitless/ms/material.hpp>
#include <limitless/lighting/light.hpp>

using namespace LimitlessMaterials;
using namespace Limitless;

LimitlessMaterials::Scene::Scene(Limitless::Context& ctx, Limitless::Assets& assets)
    : scene(ctx)
    , assets {assets} {
    setUpLighting();
    setUpModels();
}

void LimitlessMaterials::Scene::update(Limitless::Context& context, const Limitless::Camera& camera) {
    scene.update(context, camera);

    open_model->getMaterial(0)->getEmissiveColor() = {
        glm::abs(glm::cos(glfwGetTime() * 2.5)) * 2.5,
        0.0f,
        glm::abs(glm::cos(glfwGetTime() * 2.5)) * 5.0
    };
}

void LimitlessMaterials::Scene::setUpModels() {
    using namespace Limitless;

    scene.add(Instance::builder()
                      .model(assets.models.at("sphere"))
                      .material(assets.materials.at("color"))
                      .position({29.0f, 1.0f, 1.0f})
                      .build()
    );

    scene.add(Instance::builder()
                      .model(assets.models.at("sphere"))
                      .material(assets.materials.at("albedo"))
                      .position({29.0f, 1.0f, 4.0f })
                      .build()
    );

    scene.add(Instance::builder()
                      .model(assets.models.at("sphere"))
                      .material(assets.materials.at("emissive_color"))
                      .position({29.0f, 1.0f, 7.0f })
                      .build()
    );

    open_model = Instance::builder()
            .model(assets.models.at("plane"))
            .material(assets.materials.at("emissive_mask"))
            .position({29.0f, 1.0f, 10.0f})
            .rotation(glm::vec3{M_PI_2, M_PI_2 * 3, 0.0f})
            .asModel();
    scene.add(open_model);

    scene.add(Instance::builder()
                      .model(assets.models.at("plane"))
                      .material(assets.materials.at("blend_mask"))
                      .position({29.0f, 1.0f, 13.0f })
                      .rotation(glm::vec3(M_PI_2, M_PI_2 * 3, 0.0f))
                      .build()
    );

    scene.add(Instance::builder()
                      .model(assets.models.at("sphere"))
                      .material(assets.materials.at("refraction"))
                      .position({29.0f, 1.0f, 16.0f })
                      .build()
    );

    scene.add(Instance::builder()
                      .model(assets.models.at("sphere"))
                      .material(assets.materials.at("basic1"))
                      .position({24.0f, 1.0f, 1.0f })
                      .build()
    );

    scene.add(Instance::builder()
                      .model(assets.models.at("sphere"))
                      .material(assets.materials.at("basic2"))
                      .position({24.0f, 1.0f, 4.0f })
                      .build()
    );

    scene.add(Instance::builder()
                      .model(assets.models.at("sphere"))
                      .material(assets.materials.at("basic3"))
                      .position({24.0f, 1.0f, 7.0f })
                      .build()
    );

    scene.add(Instance::builder()
                      .model(assets.models.at("sphere"))
                      .material(assets.materials.at("basic5"))
                      .position({24.0f, 1.0f, 10.0f })
                      .rotation(glm::vec3(M_PI_2, M_PI_2 * 3, 0.0f))
                      .build()
    );

    scene.add(Instance::builder()
                      .model(assets.models.at("sphere"))
                      .material(assets.materials.at("basic6"))
                      .position({24.0f, 1.0f, 13.0f })
                      .build()
    );

    scene.add(Instance::builder()
                      .model(assets.models.at("sphere"))
                      .material(assets.materials.at("basic7"))
                      .position({24.0f, 1.0f, 16.0f })
                      .build()
    );

    scene.add(Instance::builder()
                      .model(assets.models.at("sphere"))
                      .material(assets.materials.at("basic8"))
                      .position({24.0f, 1.0f, 19.0f })
                      .build()
    );

    auto floor = std::make_shared<InstancedInstance>();
    for (int i = 0; i < 30; ++i) {
        for (int j = 0; j < 30; ++j) {
            floor->add(Instance::builder()
                .model(assets.models.at("plane"))
                .material(assets.materials.at("basic1"))
                .position(glm::vec3{i, 0.0f, j})
                .asModel()
            );
        }
    }
    scene.add(floor);

    scene.add(Instance::builder()
                      .model(assets.models.at("plane"))
                      .material(assets.materials.at("blending1"))
                      .position({19.0f, 1.0f, 1.0f })
                      .rotation(glm::vec3(M_PI_2, M_PI_2 * 2, 0.0f))
                      .build()
    );

    scene.add(Instance::builder()
                      .model(assets.models.at("plane"))
                      .material(assets.materials.at("blending2"))
                      .position({19.0f, 1.0f, 4.0f })
                      .rotation(glm::vec3(M_PI_2, M_PI_2 * 2, 0.0f))
                      .build()
    );

    scene.add(Instance::builder()
                      .model(assets.models.at("plane"))
                      .material(assets.materials.at("blending3"))
                      .position({19.0f, 1.0f, 7.0f })
                      .rotation(glm::vec3(M_PI_2, M_PI_2 * 2, 0.0f))
                      .build()
    );

    scene.add(Instance::builder()
                      .model(assets.models.at("cube"))
                      .material(assets.materials.at("blending4"))
                      .position({19.0f, 1.0f, 10.0f })
                      .rotation(glm::vec3(M_PI_2, M_PI_2 * 2, 0.0f))
                      .build()
    );

    scene.add(Instance::builder()
                      .model(assets.models.at("cube"))
                      .material(assets.materials.at("blending5"))
                      .position({19.0f, 1.0f, 13.0f })
                      .rotation(glm::vec3(M_PI_2, M_PI_2 * 2, 0.0f))
                      .build()
    );

    scene.add(Instance::builder()
                      .model(assets.models.at("sphere"))
                      .material(assets.materials.at("lava"))
                      .position({14.0f, 1.0f, 1.0f })
                      .build()
    );

    scene.add(Instance::builder()
                      .model(assets.models.at("sphere"))
                      .material(assets.materials.at("ice"))
                      .position({14.0f, 1.0f, 4.0f })
                      .build()
    );

    scene.add(Instance::builder()
                      .model(assets.models.at("sphere"))
                      .material(assets.materials.at("poison"))
                      .position({14.0f, 1.0f, 7.0f })
                      .build()
    );

    scene.add(Instance::builder()
                      .model(assets.models.at("sphere"))
                      .material(assets.materials.at("bump_mapping"))
                      .position({14.0f, 1.0f, 13.0f })
                      .build()
    );

    scene.add(Instance::builder()
                      .model(assets.models.at("sphere"))
                      .material(assets.materials.at("fresnel"))
                      .position({14.0f, 1.0f, 17.0f })
                      .build()
    );

    scene.add(Instance::builder()
                      .model(assets.models.at("sphere"))
                      .material(assets.materials.at("hue_shift"))
                      .position({14.0f, 1.0f, 20.0f })
                      .build()
    );

    scene.add(Instance::builder()
                      .model(assets.models.at("sphere"))
                      .material(assets.materials.at("fireball"))
                      .position({14.0f, 1.0f, 23.0f })
                      .build()
    );
}

void LimitlessMaterials::Scene::setUpLighting() {
    scene.getLighting().setAmbientColor(glm::vec4(1.0f));

    scene.setSkybox(assets.skyboxes.at("skybox"));

    scene.add(Light::builder()
          .color(glm::vec4(1.0, 1.0, 1.0, 1.0f))
          .direction(glm::vec3{0.3f, -0.99f, 0.0f})
          .build()
    );

    scene.add(Light::builder()
          .position({0.0f, 0.5f, 0.0f})
          .color({1.0f, 0.0f, 0.0f, 2.0f})
          .radius(1.0f)
          .build()
    );

    scene.add(Light::builder()
          .position({5.0f, 0.5f, 0.0f})
          .color({0.0f, 1.0f, 0.0f, 2.0f})
          .radius(2.0f)
          .build()
    );

    scene.add(Light::builder()
          .position({0.0f, 0.5f, 5.0f})
          .color({0.0f, 0.0f, 1.0f, 2.0f})
          .radius(2.0f)
          .build()
    );

    scene.add(Light::builder()
          .position({5.0f, 0.5f, 5.0f})
          .color({0.0f, 0.0f, 1.0f, 1.0f})
          .cone(30.0f, 15.0f)
          .direction({0.0f, -1.0f, 0.0f})
          .radius(1.0f)
          .build()
    );
}
