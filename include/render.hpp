#pragma once

#include <scene_data.hpp>
#include <scene.hpp>

namespace GraphicsEngine {
    class Render {
    private:
        SceneDataStorage scene_data;


        void dispatchInstances() {

        }
    public:
        void draw(Context& context, Scene& scene, Camera& camera) {
            scene.update();
            scene_data.update(context, scene, camera);

            context.setViewPort(context.getSize());

            context.enable(GL_DEPTH_TEST);
            //context.enable(GL_CULL_FACE);
            context.setCullFace(GL_BACK);
            context.setFrontFace(GL_CW);

            context.clear(Clear::ColorDepth);

            for (const auto& [id, instance] : scene.instances) {
                instance->draw(MaterialShaderType::Default, Blending::Opaque);
            }
        }
    };
}