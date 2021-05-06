#include <limitless/pipeline/pipeline.hpp>

#include <limitless/scene.hpp>
#include <limitless/core/uniform_setter.hpp>
#include <limitless/pipeline/render_pass.hpp>

using namespace Limitless;

void Pipeline::draw(Context& context, const Assets& assets, Scene& scene, Camera& camera) {
    auto instances = scene.getWrappers();

    for (const auto& pass : passes) {
        pass->update(scene, instances, context, camera);
    }

    UniformSetter setter;
    for (const auto& pass : passes) {
        pass->draw(instances, context, assets, camera, setter);
        pass->addSetter(setter);
    }
}