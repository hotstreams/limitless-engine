#include <limitless/renderer/instance_renderer.hpp>

using namespace Limitless;

void InstanceRenderer::renderScene(const DrawParameters& drawp) {
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
            render(static_cast<DecalInstance&>(*instance), drawp);
        }
    }
}

void InstanceRenderer::render(ModelInstance& instance, const DrawParameters& drawp) {
    if (instance.isHidden()) {
        return;
    }

    for (const auto& [_, mesh]: instance.getMeshes()) {
        // skip mesh if blending is different
        if (mesh.getMaterial()->getBlending() != drawp.blending) {
            return;
        }

        // set render state: shaders, material, blending, etc
        setRenderState(mesh, drawp, {InstanceType::Model, instance.getFinalMatrix()});

        // draw vertices
        mesh.getMesh()->draw();
    }
}

void InstanceRenderer::render(SkeletalInstance& instance, const DrawParameters& drawp) {
    if (instance.isHidden()) {
        return;
    }

    for (const auto& [_, mesh]: instance.getMeshes()) {
        // skip mesh if blending is different
        if (mesh.getMaterial()->getBlending() != drawp.blending) {
            return;
        }

        // set render state: shaders, material, blending, etc
        setRenderState(mesh, drawp, {InstanceType::Model, instance.getFinalMatrix()});

        // draw vertices
        mesh.getMesh()->draw();
    }

    instance.getBoneBuffer()->bindBase(drawp.ctx.getIndexedBuffers().getBindingPoint(IndexedBuffer::Type::ShaderStorage, "bone_buffer"));

    for (const auto& [_, mesh]: instance.getMeshes()) {
        // skip mesh if blending is different
        if (mesh.getMaterial()->getBlending() != drawp.blending) {
            return;
        }

        // set render state: shaders, material, blending, etc
        setRenderState(mesh, drawp, {InstanceType::Skeletal, instance.getFinalMatrix()});

        // draw vertices
        mesh.getMesh()->draw();
    }

    instance.getBoneBuffer()->fence();
}

void InstanceRenderer::render(DecalInstance& instance, const DrawParameters& drawp) {
    if (instance.isHidden() || instance.getMaterial()->getBlending() != drawp.blending) {
        return;
    }

    setBlendingMode(instance.getMaterial()->getBlending());

    drawp.ctx.disable(Capabilities::DepthTest);
    drawp.ctx.setDepthFunc(DepthFunc::Lequal);
    drawp.ctx.setDepthMask(DepthMask::False);

    auto& shader = drawp.assets.shaders.get(drawp.type, InstanceType::Decal, instance.getMaterial()->getShaderIndex());

    // updates model/material uniforms
    shader  .setUniform("_model_transform", instance.getFinalMatrix())
            .setUniform("decal_VP", glm::inverse(instance.getFinalMatrix()))
            .setMaterial(*instance.getMaterial());

    // sets custom pass-dependent uniforms
    drawp.setter(shader);

    shader.use();

    instance.getModel()->getMeshes()[0]->draw();
}

void InstanceRenderer::render(Instance& instance, const DrawParameters& drawp) {
    switch (instance.getInstanceType()) {
        case InstanceType::Model: render(static_cast<ModelInstance&>(instance), drawp); break;
        case InstanceType::Skeletal: render(static_cast<SkeletalInstance&>(instance), drawp); break;
        case InstanceType::Instanced: render(static_cast<InstancedInstance&>(instance), drawp); break;
        case InstanceType::SkeletalInstanced: break;
        case InstanceType::Effect: break;
        case InstanceType::Decal: render(static_cast<DecalInstance&>(instance), drawp); break;
        case InstanceType::Terrain: render(static_cast<TerrainInstance&>(instance), drawp); break;
    }
}

void InstanceRenderer::renderVisibleInstancedInstance(InstancedInstance& instance, const DrawParameters& drawp) {
    if (instance.isHidden()) {
        return;
    }

    // we should take shadow influencers from shadowmap too
    // if drawp.type != Shadows
    // set instanced subset (visible for current frame path)
    instance.setVisible(frustum_culling.getVisibleModelInstanced(instance));

    render(instance, drawp);
}

void InstanceRenderer::render(InstancedInstance &instance, const DrawParameters &drawp) {
    if (instance.isHidden()) {
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
        setRenderState(mesh, drawp, {InstanceType::Instanced, instance.getFinalMatrix()});

        // draw vertices
        mesh.getMesh()->draw_instanced(instance.getVisibleInstances().size());
    }
}

void InstanceRenderer::render(TerrainInstance &instance, const DrawParameters &drawp) {
    render(static_cast<ModelInstance&>(instance), drawp);
}

void InstanceRenderer::renderVisible(Instance &instance, const DrawParameters &drawp) {
    switch (instance.getInstanceType()) {
        case InstanceType::Model: render(static_cast<ModelInstance&>(instance), drawp); break;
        case InstanceType::Skeletal: render(static_cast<SkeletalInstance&>(instance), drawp); break;
        case InstanceType::Instanced: renderVisibleInstancedInstance(static_cast<InstancedInstance&>(instance), drawp); break;
        case InstanceType::SkeletalInstanced: break;
        case InstanceType::Effect: break;
        case InstanceType::Decal: break;
        case InstanceType::Terrain: render(static_cast<TerrainInstance&>(instance), drawp); break;
    }
}

void InstanceRenderer::setRenderState(const MeshInstance& mesh, const DrawParameters& drawp, const InstanceParameters& instp) {
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
    auto& shader = drawp.assets.shaders.get(drawp.type, instp.type, mesh.getMaterial()->getShaderIndex());

    // updates model/material uniforms
    shader  .setUniform("_model_transform", instp.model_matrix)
            .setMaterial(*mesh.getMaterial());

    // sets custom pass-dependent uniforms
    drawp.setter(shader);

    shader.use();
}

void InstanceRenderer::update(Scene& scene, Camera& camera) {
    frustum_culling.update(scene, camera);
    effect_renderer.update(frustum_culling.getVisibleInstances());
}