#include <limitless/renderer/instance_renderer.hpp>
#include <limitless/core/cpu_profiler.hpp>

using namespace Limitless;

void InstanceRenderer::setRenderState(const Instance& instance, const MeshInstance& mesh, const DrawParameters& drawp) {
    // sets culling based on two-sideness
    if (mesh.getMaterial()->getTwoSided()) {
        drawp.ctx.disable(Capabilities::CullFace);
    } else {
        drawp.ctx.enable(Capabilities::CullFace);
    }

    // front cullfacing for shadows helps prevent peter panning
    if (drawp.type == ShaderType::DirectionalShadow) {
        drawp.ctx.setCullFace(CullFace::Front);
    } else {
        drawp.ctx.setCullFace(CullFace::Back);
    }

    setBlendingMode(mesh.getMaterial()->getBlending());

    // gets required shader from storage
    auto& shader = drawp.assets.shaders.get(drawp.type, instance.getInstanceType(), mesh.getMaterial()->getShaderIndex());

    instance.getInstanceBuffer()->bindBase(drawp.ctx.getIndexedBuffers().getBindingPoint(IndexedBuffer::Type::UniformBuffer, "INSTANCE_BUFFER"));

    shader
            .setMaterial(*mesh.getMaterial());

    // sets custom pass-dependent uniforms
    drawp.setter(shader);

    // sets custom instance-dependent uniforms
    drawp.isetter(shader, instance);

    shader.use();
}

bool InstanceRenderer::shouldBeRendered(const Instance &instance, const DrawParameters& drawp) {
    if (instance.isHidden()) {
        return false;
    }

    if (instance.getInstanceType() == InstanceType::Decal && static_cast<const DecalInstance&>(instance).getMaterial()->getBlending() != drawp.blending) { //NOLINT
        return false;
    }

    if (drawp.type == ShaderType::DirectionalShadow && !instance.doesCastShadow()) {
        return false;
    }

    if (drawp.type == ShaderType::ColorPicker && !instance.isPickable()) {
        return false;
    }

    return true;
}

void InstanceRenderer::renderScene(const DrawParameters& drawp) {
    CpuProfileScope scope(global_profiler, "InstanceRenderer::renderScene");
    // renders common instances except decals
    // because decals rendered projected on everything else
    for (const auto& instance: frustum_culling.getVisibleInstances()) {
        renderVisible(*instance, drawp);
    }

    // renders batched effect instances
    effect_renderer.draw(drawp.ctx, drawp.assets, drawp.type, drawp.blending, drawp.setter);
}

void InstanceRenderer::renderDecals(const DrawParameters& drawp) {
    for (const auto& instance: frustum_culling.getVisibleInstances()) {
        if (instance->getInstanceType() == InstanceType::Decal) {
            render(static_cast<DecalInstance&>(*instance), drawp); //NOLINT
        }
    }
}

void InstanceRenderer::render(ModelInstance& instance, const DrawParameters& drawp) {
    if (!shouldBeRendered(instance, drawp)) {
        return;
    }

    for (const auto& [_, mesh]: instance.getMeshes()) {
        // skip mesh if blending is different
        if (mesh.getMaterial()->getBlending() != drawp.blending) {
            return;
        }

        // set render state: shaders, material, blending, etc
        setRenderState(instance, mesh, drawp);

        // draw vertices
        mesh.getMesh()->draw();
    }
}

void InstanceRenderer::render(SkeletalInstance& instance, const DrawParameters& drawp) {
    if (!shouldBeRendered(instance, drawp)) {
        return;
    }

    instance.getBoneBuffer()->bindBase(drawp.ctx.getIndexedBuffers().getBindingPoint(IndexedBuffer::Type::ShaderStorage, "bone_buffer"));

    for (const auto& [_, mesh]: instance.getMeshes()) {
        // skip mesh if blending is different
        if (mesh.getMaterial()->getBlending() != drawp.blending) {
            return;
        }

        // set render state: shaders, material, blending, etc
        setRenderState(instance, mesh, drawp);

        // draw vertices
        mesh.getMesh()->draw();
    }

    instance.getBoneBuffer()->fence();
}

void InstanceRenderer::render(DecalInstance& instance, const DrawParameters& drawp) {
    if (!shouldBeRendered(instance, drawp)) {
        return;
    }

    setBlendingMode(instance.getMaterial()->getBlending());

    drawp.ctx.disable(Capabilities::DepthTest);
    drawp.ctx.setDepthFunc(DepthFunc::Lequal);
    drawp.ctx.setDepthMask(DepthMask::False);

    auto& shader = drawp.assets.shaders.get(drawp.type, InstanceType::Decal, instance.getMaterial()->getShaderIndex());

    instance.getInstanceBuffer()->bindBase(drawp.ctx.getIndexedBuffers().getBindingPoint(IndexedBuffer::Type::UniformBuffer, "INSTANCE_BUFFER"));

    // updates model/material uniforms
    shader
            .setUniform("decal_VP", glm::inverse(instance.getFinalMatrix()))
            .setUniform<uint32_t>("projection_mask", instance.getProjectionMask())
            .setMaterial(*instance.getMaterial());

    // sets custom pass-dependent uniforms
    drawp.setter(shader);

    shader.use();

    instance.getModel()->getMeshes()[0]->draw();
}

void InstanceRenderer::render(Instance& instance, const DrawParameters& drawp) {
    switch (instance.getInstanceType()) {
        case InstanceType::Model: render(static_cast<ModelInstance&>(instance), drawp); break; //NOLINT
        case InstanceType::Skeletal: render(static_cast<SkeletalInstance&>(instance), drawp); break;//NOLINT
        case InstanceType::Instanced: render(static_cast<InstancedInstance&>(instance), drawp); break;//NOLINT
        case InstanceType::SkeletalInstanced: break; //NOLINT
        case InstanceType::Effect: break; //NOLINT
        case InstanceType::Decal: render(static_cast<DecalInstance&>(instance), drawp); break; //NOLINT
        case InstanceType::Terrain: render(static_cast<TerrainInstance&>(instance), drawp); break; //NOLINT
    }
}

void InstanceRenderer::renderVisibleInstancedInstance(InstancedInstance& instance, const DrawParameters& drawp) {
    if (!shouldBeRendered(instance, drawp)) {
        return;
    }

    // we should take shadow influencers from shadowmap too
    // if drawp.type != Shadows
    // set instanced subset (visible for current frame path)
    instance.setVisible(frustum_culling.getVisibleModelInstanced(instance));

    render(instance, drawp);
}

void InstanceRenderer::renderVisibleTerrain(TerrainInstance &instance, const DrawParameters &drawp) {
    if (!shouldBeRendered(instance, drawp)) {
        return;
    }

    for (const auto& mref: frustum_culling.getVisibleTerrainMeshes(instance)) {
        auto& mesh = mref.get();

        // skip mesh if blending is different
        if (mesh.getMaterial()->getBlending() != drawp.blending) {
            return;
        }

        // set render state: shaders, material, blending, etc
        setRenderState(instance, mesh, drawp);

        // draw vertices
        mesh.getMesh()->draw();
    }
}

void InstanceRenderer::render(InstancedInstance &instance, const DrawParameters &drawp) {
    if (!shouldBeRendered(instance, drawp)) {
        return;
    }

    // bind buffer for instanced data
    instance.getBuffer()->bindBase(drawp.ctx.getIndexedBuffers().getBindingPoint(IndexedBuffer::Type::ShaderStorage, "model_buffer"));

    for (const auto& [_, mesh]: instance.getInstances()[0]->getMeshes()) {
        // skip mesh if blending is different
        if (mesh.getMaterial()->getBlending() != drawp.blending) {
            return;
        }

        // set render state: shaders, material, blending, etc
        setRenderState(instance, mesh, drawp);

        // draw vertices
        mesh.getMesh()->draw_instanced(instance.getVisibleInstances().size());
    }
}

void InstanceRenderer::render(TerrainInstance &instance, const DrawParameters &drawp) {
    if (!shouldBeRendered(instance, drawp)) {
        return;
    }

    for (const auto& [_, mesh]: instance.getMeshes()) {
        // skip mesh if blending is different
        if (mesh.getMaterial()->getBlending() != drawp.blending) {
            return;
        }

        // set render state: shaders, material, blending, etc
        setRenderState(instance, mesh, drawp);

        // draw vertices
        mesh.getMesh()->draw();
    }
}

void InstanceRenderer::renderVisible(Instance &instance, const DrawParameters &drawp) {
    switch (instance.getInstanceType()) {
        case InstanceType::Model: render(static_cast<ModelInstance&>(instance), drawp); break; //NOLINT
        case InstanceType::Skeletal: render(static_cast<SkeletalInstance&>(instance), drawp); break; //NOLINT
        case InstanceType::Instanced: renderVisibleInstancedInstance(static_cast<InstancedInstance&>(instance), drawp); break; //NOLINT
        case InstanceType::SkeletalInstanced: break; //NOLINT
        case InstanceType::Effect: break; //NOLINT
        case InstanceType::Decal: break; //NOLINT
        case InstanceType::Terrain: renderVisibleTerrain(static_cast<TerrainInstance&>(instance), drawp); break; //NOLINT
    }
}

void InstanceRenderer::update(Scene& scene, Camera& camera) {
    CpuProfileScope scope(global_profiler, "InstanceRenderer::update");

    frustum_culling.update(scene, camera);
    effect_renderer.update(frustum_culling.getVisibleInstances());
}
