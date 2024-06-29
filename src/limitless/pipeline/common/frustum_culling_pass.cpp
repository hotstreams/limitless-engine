#include <limitless/pipeline/common/frustum_culling_pass.hpp>
#include <limitless/util/frustum.hpp>
#include <iostream>

using namespace Limitless;

FrustumCullingPass::FrustumCullingPass(Pipeline &pipeline)
    : PipelinePass(pipeline) {
}

void FrustumCullingPass::update(Scene& scene, Instances& instances, Context &ctx, const Camera &camera) {
    Instances visible;

    auto frustum = Frustum::fromCamera(camera);

    for (const auto& instance: instances) {
        if (frustum.intersects(instance.get().getBoundingBox())) {
            visible.emplace_back(instance);
        }
    }

    instances = visible;
}
