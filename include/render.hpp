#pragma once

#include <scene_data.hpp>
#include <scene.hpp>
#include <render_settings.hpp>
#include <assets.hpp>

namespace GraphicsEngine {
    class Render {
    private:
        SceneDataStorage scene_data;

        void dispatchInstances(Context& context, Scene& scene, MaterialShaderType shader_type, Blending blending) const {
            for (const auto& [id, instance] : scene.instances) {
                instance->isWireFrame() ? context.setPolygonMode(GL_LINE) : context.setPolygonMode(GL_FILL);

                instance->draw(shader_type, blending);
            }
        }

        void renderLightsVolume(Context& context, Scene& scene) const {
            context.setPolygonMode(GL_LINE);
            auto sphere_instance =  ModelInstance(assets.models.get("sphere"), assets.materials.get("sphere"), glm::vec3(0.0f));
            for (const auto& light : scene.lighting.dynamic.points_lights.getLights()) {
                sphere_instance.setPosition(light.position);
                sphere_instance.setScale(glm::vec3(light.radius));
                sphere_instance.draw(MaterialShaderType::Default, Blending::Opaque);
            }
            context.setPolygonMode(GL_FILL);
        }
    public:
        explicit Render(Context& context) noexcept {
            context.clearColor({ 0.5f, 0.5f, 0.5f, 1.0f});

            context.enable(GL_VERTEX_PROGRAM_POINT_SIZE);
            context.enable(GL_DEPTH_TEST);
        }

        void draw(Context& context, Scene& scene, Camera& camera) {
            scene.update();
            scene_data.update(context, scene, camera);

            context.setViewPort(context.getSize());

            context.enable(GL_DEPTH_TEST);

            context.clear(Clear::ColorDepth);

            dispatchInstances(context, scene, MaterialShaderType::Default, Blending::Opaque);

            dispatchInstances(context, scene, MaterialShaderType::Default, Blending::Additive);

            if (render_settings.light_radius) {
                renderLightsVolume(context, scene);
            }
        }
    };
}