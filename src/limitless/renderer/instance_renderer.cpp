#include <limitless/renderer/instance_renderer.hpp>
#include <limitless/instances/model_instance.hpp>
#include <limitless/models/mesh.hpp>
#include <limitless/util/lod_transition.h>

#include "limitless/core/profiler.hpp"
#include <limitless/core/uniform/uniform_value.hpp>

namespace {
void mapInstanceLodFade(Limitless::Instance& instance, const glm::vec4& fade) {
    Limitless::Instance::Data d = instance.getCurrentData();
    d.lod_fade = fade;
    instance.getInstanceBuffer()->mapData(&d, sizeof(Limitless::Instance::Data));
}

    /**
     * Check if an instance is fully handled by the indirect renderer.
     * Returns true only if the instance is a Model or Instanced type AND
     * ALL its meshes have draw_info (batched geometry).
     */
    bool isFullyBatchedForIndirect(const Limitless::Instance& instance) {
        using namespace Limitless;
        if (instance.getInstanceType() == InstanceType::Model) {
            const auto* model = dynamic_cast<const ModelInstance*>(&instance);
            if (!model) return false;
            for (const auto& [name, mesh_inst] : model->getMeshes()) {
                const auto& mesh = mesh_inst.getMesh();
                if (!mesh || !mesh->getDrawInfo()) {
                    return false;
                }
            }
            return true;
        }
        if (instance.getInstanceType() == InstanceType::Instanced) {
            const auto* instanced = dynamic_cast<const InstancedInstance*>(&instance);
            if (!instanced) return false;
            const auto& subs = instanced->getInstances();
            if (subs.empty()) return false;
            for (const auto& [name, mesh_inst] : subs[0]->getMeshes()) {
                const auto& mesh = mesh_inst.getMesh();
                if (!mesh || !mesh->getDrawInfo()) {
                    return false;
                }
            }
            return true;
        }
        if (instance.getInstanceType() == InstanceType::SkeletalInstanced) {
            return false;
        }
        return false;
    }
} // namespace

using namespace Limitless;

static bool getMaterialBoolUniform(const Limitless::ms::Material& material, const char* name) noexcept {
    try {
        const auto& uniforms = material.getUniforms();
        auto it = uniforms.find(std::string{name});
        if (it == uniforms.end() || !it->second) {
            return false;
        }

        // We commonly store bool-like switches as uint32_t 0/1 in materials.
        if (auto* u = dynamic_cast<Limitless::UniformValue<uint32_t>*>(it->second.get())) {
            return u->getValue() != 0u;
        }
        if (auto* u = dynamic_cast<Limitless::UniformValue<int32_t>*>(it->second.get())) {
            return u->getValue() != 0;
        }
        if (auto* u = dynamic_cast<Limitless::UniformValue<float>*>(it->second.get())) {
            return u->getValue() != 0.0f;
        }
    } catch (...) {
        // Never let render-state setup throw.
    }
    return false;
}

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

    // Alpha-to-coverage for masked foliage (requires MSAA framebuffer to have an effect).
    // We intentionally disable it for shadow maps to avoid unexpected artifacts.
    drawp.ctx.disable(Capabilities::SampleAlphaToCoverage);
    if (drawp.type != ShaderType::DirectionalShadow) {
        if (getMaterialBoolUniform(*mesh.getMaterial(), "alpha_to_coverage")) {
            drawp.ctx.enable(Capabilities::SampleAlphaToCoverage);
        }
    }

    // gets required shader from storage
    auto& shader = drawp.assets.shaders.get(drawp.type, instance.getInstanceType(), mesh.getMaterial()->getShaderIndex());

    instance.getInstanceBuffer()->bindBase(drawp.ctx.getIndexedBuffers().getBindingPoint(IndexedBuffer::Type::UniformBuffer, "INSTANCE_BUFFER"));

    shader.setMaterial(*mesh.getMaterial());

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
    // renders common instances except decals
    // because decals rendered projected on everything else
    for (const auto& instance: frustum_culling.getVisibleInstances()) {
        renderVisible(*instance, drawp);
    }

    // renders batched effect instances
    effect_renderer.draw(drawp.ctx, drawp.assets, drawp.type, drawp.blending, drawp.setter);
}

void InstanceRenderer::renderSceneFromLists(
        const DrawParameters& drawp,
        const Instances& instances,
        const std::unordered_map<uint64_t, std::vector<std::shared_ptr<ModelInstance>>>& visible_subinstances) {
    // Render instances exactly as specified (caller owns culling).
    for (const auto& instance : instances) {
        if (!instance) continue;

        if (instance->getInstanceType() == InstanceType::Instanced
            || instance->getInstanceType() == InstanceType::SkeletalInstanced) {
            auto& instanced = static_cast<InstancedInstance&>(*instance); // NOLINT
            auto it = visible_subinstances.find(instance->getId());
            if (it != visible_subinstances.end()) {
                instanced.setVisible(it->second);
            } else {
                // No subset provided -> render all.
                instanced.setVisible(instanced.getInstances());
            }
            if (instance->getInstanceType() == InstanceType::SkeletalInstanced) {
                render(static_cast<SkeletalInstancedInstance&>(instanced), drawp); // NOLINT
            } else {
                render(instanced, drawp);
            }
            continue;
        }

        if (instance->getInstanceType() == InstanceType::Terrain) {
            auto& terrain = static_cast<TerrainInstance&>(*instance); // NOLINT
            auto applyVisible = [&](const std::shared_ptr<InstancedInstance>& container) {
                if (!container) return;
                auto it = visible_subinstances.find(container->getId());
                if (it != visible_subinstances.end()) {
                    container->setVisible(it->second);
                } else {
                    container->setVisible(container->getInstances());
                }
            };

            // cross is a standalone instance
            if (terrain.getMesh().cross) {
                render(*terrain.getMesh().cross, drawp);
            }
            applyVisible(terrain.getMesh().seams);
            applyVisible(terrain.getMesh().trims);
            applyVisible(terrain.getMesh().fillers);
            applyVisible(terrain.getMesh().tiles);

            // Render the terrain sub-containers
            if (terrain.getMesh().seams)  render(*terrain.getMesh().seams, drawp);
            if (terrain.getMesh().trims)  render(*terrain.getMesh().trims, drawp);
            if (terrain.getMesh().fillers)render(*terrain.getMesh().fillers, drawp);
            if (terrain.getMesh().tiles)  render(*terrain.getMesh().tiles, drawp);
            continue;
        }

        render(*instance, drawp);
    }

    // Effects are currently still driven by the internal effect_renderer update path.
    // In shadow rendering, effects are usually excluded; keep consistent behavior.
    effect_renderer.draw(drawp.ctx, drawp.assets, drawp.type, drawp.blending, drawp.setter);
}

void InstanceRenderer::renderVisibleTerrainOnly(const DrawParameters& drawp) {
    for (const auto& instance: frustum_culling.getVisibleInstances()) {
        if (instance->getInstanceType() == InstanceType::Terrain) {
            renderVisibleTerrain(static_cast<TerrainInstance&>(*instance), drawp); //NOLINT
        }
    }
}

void InstanceRenderer::renderVisibleNonTerrain(const DrawParameters& drawp) {
    for (const auto& instance: frustum_culling.getVisibleInstances()) {
        if (instance->getInstanceType() != InstanceType::Terrain &&
            instance->getInstanceType() != InstanceType::Decal) {
            renderVisible(*instance, drawp);
        }
    }
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

    const auto& model = *instance.getModel();
    auto& lg = instance.getLodGroup();

    const auto drawMeshesAtLod = [&](uint32_t lod_index, const glm::vec4& fade) {
        mapInstanceLodFade(instance, fade);
        const auto& meshes = lg.getLodMeshesAt(lod_index);
        for (const auto& [_, mesh] : meshes) {
            if (mesh.getMaterial()->getBlending() != drawp.blending) {
                return;
            }
            setRenderState(instance, mesh, drawp);
            mesh.getMesh()->draw();
        }
    };

    if (model.getLods().size() > 1u && model.getTransition() == LodTransition::CrossFadeDither && lg.isLodCrossFadeActive()) {
        drawMeshesAtLod(lg.getCrossFadeFinerLod(), lg.getLodFadePackedForDrawLod(lg.getCrossFadeFinerLod()));
        drawMeshesAtLod(lg.getCrossFadeCoarserLod(), lg.getLodFadePackedForDrawLod(lg.getCrossFadeCoarserLod()));
        mapInstanceLodFade(instance, glm::vec4(0.0f));
        return;
    }

    mapInstanceLodFade(instance, glm::vec4(0.0f));
    for (const auto& [_, mesh]: instance.getMeshes()) {
        // skip mesh if blending is different
        if (mesh.getMaterial()->getBlending() != drawp.blending) {
            return;
        }

        // set render state: shaders, material, blending, etc
        setRenderState(instance, mesh, drawp);

        // draw current lod
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

    instance.getModel()->getLods()[0].meshes[0]->draw();
}

void InstanceRenderer::render(Instance& instance, const DrawParameters& drawp) {
    switch (instance.getInstanceType()) {
        case InstanceType::Model: render(static_cast<ModelInstance&>(instance), drawp); break; //NOLINT
        case InstanceType::Skeletal: render(static_cast<SkeletalInstance&>(instance), drawp); break;//NOLINT
        case InstanceType::Instanced: render(static_cast<InstancedInstance&>(instance), drawp); break;//NOLINT
        case InstanceType::SkeletalInstanced:
            render(static_cast<SkeletalInstancedInstance&>(instance), drawp); // NOLINT
            break;
        case InstanceType::Effect: break; //NOLINT
        case InstanceType::Decal: render(static_cast<DecalInstance&>(instance), drawp); break; //NOLINT
        case InstanceType::Terrain: render(static_cast<TerrainInstance&>(instance), drawp); break; //NOLINT
        case InstanceType::BatchedModel:
            break;
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

void InstanceRenderer::renderVisibleSkeletalInstancedInstance(SkeletalInstancedInstance& instance, const DrawParameters& drawp) {
    if (!shouldBeRendered(instance, drawp)) {
        return;
    }

    instance.setVisible(frustum_culling.getVisibleModelInstanced(instance));
    render(instance, drawp);
}

void InstanceRenderer::renderVisibleTerrain(TerrainInstance &instance, const DrawParameters &drawp)
{
    if (!shouldBeRendered(instance, drawp)) {
        return;
    }

    render(*instance.getMesh().cross, drawp);

    renderVisibleInstancedInstance(*instance.getMesh().tiles, drawp);
    renderVisibleInstancedInstance(*instance.getMesh().fillers, drawp);
    renderVisibleInstancedInstance(*instance.getMesh().trims, drawp);
    renderVisibleInstancedInstance(*instance.getMesh().seams, drawp);
}

void InstanceRenderer::render(InstancedInstance &instance, const DrawParameters &drawp) {
    if (!shouldBeRendered(instance, drawp)) {
        return;
    }

    const auto& lod_groups = instance.getLodVisibleInstances();

    // Render each LOD level separately with its own buffer and mesh
    for (const auto& [lod_level, lod_instances] : lod_groups) {
        if (lod_instances.empty()) {
            continue;
        }

        // Get LOD-specific buffer
        auto lod_buffer = instance.getLodBuffer(lod_level);
        if (!lod_buffer) {
            continue;
        }

        // Bind LOD buffer for instanced data
        lod_buffer->bindBase(drawp.ctx.getIndexedBuffers().getBindingPoint(IndexedBuffer::Type::ShaderStorage, "model_buffer"));

        // Get meshes from the first visible ModelInstance (contains correct materials from builder)
        // Per-LOD mesh/materials from the LOD group (instance may be in two LOD buckets during cross-fade)
        const auto& meshes = lod_instances[0]->getLodGroup().getLodMeshesAt(lod_level);

        for (const auto& [name, mesh_instance] : meshes) {
            const auto& material = mesh_instance.getMaterial();

            // skip mesh if blending is different
            if (material->getBlending() != drawp.blending) {
                continue;
            }

            setRenderState(instance, mesh_instance, drawp);

            mesh_instance.getMesh()->draw_instanced(lod_instances.size());
        }
    }
}

void InstanceRenderer::render(SkeletalInstancedInstance& instance, const DrawParameters& drawp) {
    if (!shouldBeRendered(instance, drawp)) {
        return;
    }

    const auto bp_bones = drawp.ctx.getIndexedBuffers().getBindingPoint(
        IndexedBuffer::Type::ShaderStorage, "bone_buffer");
    const auto bp_model = drawp.ctx.getIndexedBuffers().getBindingPoint(
        IndexedBuffer::Type::ShaderStorage, "model_buffer");

    for (const auto& [lod_level, lod_instances] : instance.getLodVisibleInstances()) {
        if (lod_instances.empty()) {
            continue;
        }

        auto lod_buffer = instance.getLodBuffer(lod_level);
        auto bone_buffer = instance.getLodBoneBuffer(lod_level);
        if (!lod_buffer || !bone_buffer) {
            continue;
        }

        lod_buffer->bindBase(bp_model);
        bone_buffer->bindBase(bp_bones);

        const auto& meshes = lod_instances[0]->getLodGroup().getLodMeshesAt(lod_level);

        for (const auto& [name, mesh_instance] : meshes) {
            const auto& material = mesh_instance.getMaterial();

            if (material->getBlending() != drawp.blending) {
                continue;
            }

            setRenderState(instance, mesh_instance, drawp);

            mesh_instance.getMesh()->draw_instanced(lod_instances.size());
        }

        bone_buffer->fence();
    }
}

void InstanceRenderer::render(TerrainInstance &instance, const DrawParameters &drawp) {
    if (!shouldBeRendered(instance, drawp)) {
        return;
    }

//    render(*instance.getMesh().cross, drawp);

//    for (auto &item: instance.getMesh().seams) {
//        render(*item, drawp);
//    }
//
//    for (auto &item : instance.getMesh().trims_test) {
//        render(*item, drawp);
//    }
//
//    render(*instance.getMesh().cross, drawp);

//    for (auto &item: instance.getMesh().seams) {
    instance.getMesh().seams->setVisible(instance.getMesh().seams->getInstances());
    render(*instance.getMesh().seams, drawp);
//    }

//    for (auto &item: instance.getMesh().trims) {
    instance.getMesh().trims->setVisible(instance.getMesh().trims->getInstances());
    render(*instance.getMesh().trims, drawp);
//    }

//    for (auto &item: instance.getMesh().fillers) {
    instance.getMesh().fillers->setVisible(instance.getMesh().fillers->getInstances());
    render(*instance.getMesh().fillers, drawp);
//    }

//    for (auto &item: instance.getMesh().tiles) {
    instance.getMesh().tiles->setVisible(instance.getMesh().tiles->getInstances());
    render(*instance.getMesh().tiles, drawp);
//    }

    // instance.getMesh().trims->setVisible(instance.getMesh().trims->getInstances());
    // render(*instance.getMesh().trims, drawp);
//
//    for (auto &item: instance.getMesh().fillers) {
//        render(*item, drawp);
//    }
//
//    for (auto &item: instance.getMesh().tiles) {
//        render(*item, drawp);
//    }
}

void InstanceRenderer::renderVisible(Instance &instance, const DrawParameters &drawp) {
    switch (instance.getInstanceType()) {
        case InstanceType::Model: render(static_cast<ModelInstance&>(instance), drawp); break; //NOLINT
        case InstanceType::Skeletal: render(static_cast<SkeletalInstance&>(instance), drawp); break; //NOLINT
        case InstanceType::Instanced:
            renderVisibleInstancedInstance(static_cast<InstancedInstance&>(instance), drawp); // NOLINT
            break;
        case InstanceType::SkeletalInstanced:
            renderVisibleSkeletalInstancedInstance(static_cast<SkeletalInstancedInstance&>(instance), drawp); // NOLINT
            break;
        case InstanceType::Effect: break; //NOLINT
        case InstanceType::Decal: break; //NOLINT
        case InstanceType::Terrain: renderVisibleTerrain(static_cast<TerrainInstance&>(instance), drawp); break; //NOLINT
        case InstanceType::BatchedModel: break;
        // case InstanceType::Terrain: render(static_cast<TerrainInstance&>(instance), drawp); break; //NOLINT
//        case InstanceType::Terrain: render(static_cast<TerrainInstance&>(instance), drawp); break; //NOLINT
    }
}

void InstanceRenderer::update(Scene& scene, Camera& camera) {
    frustum_culling.update(scene, camera);
    effect_renderer.update(frustum_culling.getVisibleInstances());
}

void InstanceRenderer::prepareSortedRendering(const Camera& camera, const RendererSettings& settings) {
    render_queue.setRenderSettings(settings);
    render_queue.clear();
    render_queue.collect(frustum_culling.getVisibleInstances(), frustum_culling, camera);
    render_queue.sort();
}

void InstanceRenderer::renderSceneSorted(const DrawParameters& drawp) {
    // Render using sorted queue (minimizes state changes)
    render_queue.render(drawp, drawp.blending);

    // Effects are still rendered through effect_renderer
    effect_renderer.draw(drawp.ctx, drawp.assets, drawp.type, drawp.blending, drawp.setter);
}

void InstanceRenderer::renderVisibleTerrainOnlySorted(const DrawParameters& drawp) {
    render_queue.renderTerrainOnly(drawp);
}

void InstanceRenderer::renderVisibleNonTerrainSorted(const DrawParameters& drawp) {
    render_queue.renderNonTerrain(drawp);

    // Effects are rendered with non-terrain
    effect_renderer.draw(drawp.ctx, drawp.assets, drawp.type, drawp.blending, drawp.setter);
}

void InstanceRenderer::renderVisibleNonBatchedNonTerrain(const DrawParameters& drawp) {
    for (const auto& instance : frustum_culling.getVisibleInstances()) {
        if (instance->getInstanceType() == InstanceType::Terrain ||
            instance->getInstanceType() == InstanceType::Decal) {
            continue;
        }
        // Skip instances fully handled by indirect draw
        if (isFullyBatchedForIndirect(*instance)) {
            continue;
        }
        renderVisible(*instance, drawp);
    }

    // Effects are not handled by indirect draw - always render them
    effect_renderer.draw(drawp.ctx, drawp.assets, drawp.type, drawp.blending, drawp.setter);
}

void InstanceRenderer::renderVisibleNonBatchedNonTerrainSorted(const DrawParameters& drawp) {
    render_queue.renderNonBatchedNonTerrain(drawp);

    // Effects are not handled by indirect draw - always render them
    effect_renderer.draw(drawp.ctx, drawp.assets, drawp.type, drawp.blending, drawp.setter);
}
