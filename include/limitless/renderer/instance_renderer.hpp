#pragma once

#include <limitless/pipeline/shader_type.hpp>
#include <limitless/instances/instance.hpp>
#include <limitless/instances/model_instance.hpp>
#include <limitless/instances/instanced_instance.hpp>
#include <limitless/scene.hpp>
#include <limitless/assets.hpp>
#include <limitless/core/shader/shader_program.hpp>
#include <limitless/instances/terrain_instance.hpp>

namespace Limitless {
    class DrawParameters {
    public:
        Context& ctx;
        Assets& assets;
        ShaderType type;
        ms::Blending blending;
        UniformSetter setter;
    };

    class InstanceParameters {
    public:
        InstanceType type;
        glm::mat4 model_matrix;
    };

    class InstanceRenderer {
    private:
        std::vector<std::shared_ptr<Instance>> visible;
        std::map<uint64_t, std::vector<std::shared_ptr<ModelInstance>>> visible_instanced;

        static bool isInFrustum(const Frustum& frustum, Instance& instance) {
            return frustum.intersects(instance.getBoundingBox());
        }

        void frustumCulling(Scene& scene, Camera& camera) {
            const auto frustum = Frustum::fromCamera(camera);

            for (auto& instance : scene.getInstances()) {
                if (instance->getInstanceType() == InstanceType::Instanced) {
                    visible.emplace_back(instance);

                    auto& instanced = static_cast<InstancedInstance&>(*instance); //NOLINT

                    for (auto& i: instanced.getInstances()) {
                        if (isInFrustum(frustum, *i)) {
                            visible_instanced[instance->getId()].emplace_back(i);
                        }
                    }
                } else {
                    if (isInFrustum(frustum, *instance)) {
                        visible.emplace_back(instance);
                    }
                }
            }
        }

        static void setShaderTypeSettings(const DrawParameters& drawp) {
            // front cullfacing for shadows helps prevent peter panning
            if (drawp.type == ShaderType::DirectionalShadow) {
                drawp.ctx.setCullFace(CullFace::Front);
            } else {
                drawp.ctx.setCullFace(CullFace::Back);
            }
        }

        static void setBlendingSettings(ms::Blending blending) {
            // set context blending mode
            setBlendingMode(blending);
        }
    public:
        void prepare(Scene& scene, Camera& camera) {
            visible.clear();
            visible_instanced.clear();
            frustumCulling(scene, camera);
        }

        void render(Scene& scene, const DrawParameters& drawp) {
            for (const auto& instance: visible) {
                render(*instance, drawp);
            }
        }

        void render(Instance& instance, DrawParameters drawp) {
            switch (instance.getInstanceType()) {
                case InstanceType::Model:
                    render(static_cast<ModelInstance&>(instance), drawp);
                    break;
                case InstanceType::Skeletal:
                    render(static_cast<SkeletalInstance&>(instance), drawp);
                    break;
                case InstanceType::Instanced:
                    render(static_cast<InstancedInstance&>(instance), drawp);
                    break;
                case InstanceType::SkeletalInstanced:
//                    render(static_cast<SkeletalInstancedInstance&>(*instance), drawp);
                    break;
                case InstanceType::Effect:
                    render(static_cast<EffectInstance&>(instance), drawp);
                    break;
                case InstanceType::Decal:
                    render(static_cast<DecalInstance&>(instance), drawp);
                    break;
                case InstanceType::Terrain:
                    render(static_cast<TerrainInstance&>(instance), drawp);
                    break;
            }
        }

        void render(ModelInstance& instance, const DrawParameters& drawp) {
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

        void render(InstancedInstance& instance, const DrawParameters& drawp) {
            if (instance.isHidden() || instance.getInstances().empty()) {
                return;
            }

            // set instanced subset (visible for current frame)
            instance.setVisible(visible_instanced.at(instance.getId()));

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
                mesh.getMesh()->draw_instanced(instance.getInstances().size());
            }
        }

        void render(EffectInstance& instance, const DrawParameters& parameters) {

        }

        void render(DecalInstance& instance, const DrawParameters& drawp) {
            if (instance.isHidden() || instance.getMaterial()->getBlending() != drawp.blending) {
                return;
            }

            setBlendingSettings(instance.getMaterial()->getBlending());

            drawp.ctx.disable(Capabilities::DepthTest);
            drawp.ctx.setDepthFunc(DepthFunc::Lequal);
            drawp.ctx.setDepthMask(DepthMask::False);

            auto& shader = drawp.assets.shaders.get(drawp.type, InstanceType::Decal, instance.getMaterial()->getShaderIndex());

            // updates model/material uniforms
            shader.setUniform("_model_transform", instance.getFinalMatrix())
                    .setMaterial(*instance.getMaterial());

            // sets custom pass-dependent uniforms
            drawp.setter(shader);

            shader.use();

            instance.getModel()->getMeshes()[0]->draw();
        }

        static void setRenderState(const MeshInstance& mesh, const DrawParameters& drawp, const InstanceParameters& instp) {
            // sets culling based on two-sideness
            if (mesh.getMaterial()->getTwoSided()) {
                drawp.ctx.disable(Capabilities::CullFace);
            } else {
                drawp.ctx.enable(Capabilities::CullFace);
            }

            setShaderTypeSettings(drawp);

            setBlendingSettings(mesh.getMaterial()->getBlending());

            // gets required shader from storage
            auto& shader = drawp.assets.shaders.get(drawp.type, instp.type, mesh.getMaterial()->getShaderIndex());

            // updates model/material uniforms
            shader  .setUniform("_model_transform", instp.model_matrix)
                    .setMaterial(*mesh.getMaterial());

            // sets custom pass-dependent uniforms
            drawp.setter(shader);

            shader.use();
        }
    };
}