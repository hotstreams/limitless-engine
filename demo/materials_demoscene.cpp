#include "materials_demoscene.hpp"

#include <limitless/scene.hpp>
#include <limitless/instances/instanced_instance.hpp>
#include <limitless/ms/material.hpp>

using namespace LimitlessDemo;

MaterialsScene::MaterialsScene(Limitless::Context& ctx, Limitless::Assets& assets)
    : Limitless::Scene(ctx) {
    addModels(assets);

    setSkybox(assets.skyboxes.at("skybox"));
    lighting.ambient_color *= 0.5;
    lighting.directional_light = {glm::vec4(2.0, -2.0, 1.5, 1.0f), glm::vec4{1.0f}};
}

void MaterialsScene::addModels(const Limitless::Assets& assets) {
    using namespace Limitless;

    add<ModelInstance>(assets.models.at("sphere"), assets.materials.at("color"), glm::vec3{29.0f, 1.0f, 1.0f});
    add<ModelInstance>(assets.models.at("sphere"), assets.materials.at("albedo"), glm::vec3{29.0f, 1.0f, 4.0f });
    add<ModelInstance>(assets.models.at("sphere"), assets.materials.at("emissive_color"), glm::vec3{29.0f, 1.0f, 7.0f });
    open = &add<ModelInstance>(assets.models.at("plane"), assets.materials.at("emissive_mask"), glm::vec3{29.0f, 1.0f, 10.0f});
    open->setRotation(glm::vec3(M_PI_2, M_PI_2 * 3, 0.0f));
    add<ModelInstance>(assets.models.at("plane"), assets.materials.at("blend_mask"), glm::vec3{29.0f, 1.0f, 13.0f })
    .setRotation(glm::vec3(M_PI_2, M_PI_2 * 3, 0.0f));
    add<ModelInstance>(assets.models.at("sphere"), assets.materials.at("refraction"), glm::vec3{29.0f, 1.0f, 16.0f });

    add<ModelInstance>(assets.models.at("sphere"), assets.materials.at("basic1"), glm::vec3{24.0f, 1.0f, 1.0f });
    add<ModelInstance>(assets.models.at("sphere"), assets.materials.at("basic2"), glm::vec3{24.0f, 1.0f, 4.0f });
    add<ModelInstance>(assets.models.at("sphere"), assets.materials.at("basic3"), glm::vec3{24.0f, 1.0f, 7.0f });
    add<ModelInstance>(assets.models.at("plane"), assets.materials.at("basic5"), glm::vec3{24.0f, 1.0f, 10.0f })
            .setRotation(glm::vec3(M_PI_2, M_PI_2 * 3, 0.0f));
    add<ModelInstance>(assets.models.at("sphere"), assets.materials.at("basic6"), glm::vec3{24.0f, 1.0f, 13.0f });
    add<ModelInstance>(assets.models.at("sphere"), assets.materials.at("basic7"), glm::vec3{24.0f, 1.0f, 16.0f });
    add<ModelInstance>(assets.models.at("sphere"), assets.materials.at("basic8"), glm::vec3{24.0f, 1.0f, 19.0f });

    auto& floor = add<InstancedInstance<ModelInstance>>(glm::vec3{0.0f});
    for (int i = 0; i < 30; ++i) {
        for (int j = 0; j < 30; ++j) {
            floor.addInstance(
                    std::make_unique<ModelInstance>(
                            assets.models.at("plane"),
                            assets.materials.at("basic4"),
                            glm::vec3{i, 0.0f, j}
                    )
            );
        }
    }

    add<ModelInstance>(assets.models.at("plane"), assets.materials.at("blending1"), glm::vec3{19.0f, 1.0f, 1.0f })
            .setRotation(glm::vec3(M_PI_2, M_PI_2 * 2, 0.0f));
    add<ModelInstance>(assets.models.at("plane"), assets.materials.at("blending2"), glm::vec3{19.0f, 1.0f, 4.0f })
            .setRotation(glm::vec3(M_PI_2, M_PI_2 * 2, 0.0f));
    add<ModelInstance>(assets.models.at("plane"), assets.materials.at("blending3"), glm::vec3{19.0f, 1.0f, 7.0f })
            .setRotation(glm::vec3(M_PI_2, M_PI_2 * 2, 0.0f));

    add<ModelInstance>(assets.models.at("cube"), assets.materials.at("blending4"), glm::vec3{19.0f, 1.0f, 10.0f });
    add<ModelInstance>(assets.models.at("cube"), assets.materials.at("blending5"), glm::vec3{19.0f, 1.0f, 13.0f });

    add<ModelInstance>(assets.models.at("sphere"), assets.materials.at("lava"), glm::vec3{14.0f, 1.0f, 1.0f });
    add<ModelInstance>(assets.models.at("sphere"), assets.materials.at("ice"), glm::vec3{14.0f, 1.0f, 4.0f });
    add<ModelInstance>(assets.models.at("sphere"), assets.materials.at("poison"), glm::vec3{14.0f, 1.0f, 7.0f });
    auto& m = add<ModelInstance>(assets.models.at("sphere"), assets.materials.at("ice"), glm::vec3{14.0f, 1.0f, 10.0f });
    m["sphere"].getMaterial().apply(assets.materials.at("poison"));

    add<ModelInstance>(assets.models.at("sphere"), assets.materials.at("bump_mapping"), glm::vec3{14.0f, 1.0f, 13.0f });
    add<ModelInstance>(assets.models.at("sphere"), assets.materials.at("fresnel"), glm::vec3{14.0f, 1.0f, 17.0f });

    add<ModelInstance>(assets.models.at("sphere"), assets.materials.at("hue_shift"), glm::vec3{14.0f, 1.0f, 20.0f });
    add<ModelInstance>(assets.models.at("sphere"), assets.materials.at("fireball"), glm::vec3{14.0f, 1.0f, 23.0f });
}

void MaterialsScene::update(Limitless::Context& context, const Limitless::Camera& camera) {
    Limitless::Scene::update(context, camera);

    (*open)["plane_mesh"].getMaterial()[0].getEmissiveColor().setValue({
        glm::abs(glm::cos(glfwGetTime() * 2.5)) * 2.5,
        0.0f,
        glm::abs(glm::cos(glfwGetTime() * 2.5)) * 5.0,
        1.0f
    });
}