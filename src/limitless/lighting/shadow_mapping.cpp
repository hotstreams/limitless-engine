#include <limitless/lighting/shadow_mapping.hpp>

#include <limitless/scene.hpp>

using namespace LimitlessEngine;

ShadowMapping::ShadowMapping() {

}

void ShadowMapping::castShadows(Renderer& render, const Assets& assets, Scene& scene, Context& ctx, Camera& camera) {
    if (scene.lighting.directional_light.direction != glm::vec4{0.0f}) {
        directional_shadow.castShadows(render, assets, scene, ctx, camera);
        directional_shadow.mapData();
    }
}

void ShadowMapping::setUniform(ShaderProgram& shader) const {
    directional_shadow.setUniform(shader);
}

