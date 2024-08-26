#pragma once

#include <limitless/pipeline/shader_type.hpp>

#include <limitless/instances/instanced_instance.hpp>
#include <limitless/scene.hpp>
#include <limitless/assets.hpp>
#include <limitless/core/shader/shader_program.hpp>
#include <limitless/instances/terrain_instance.hpp>
#include <limitless/fx/effect_renderer.hpp>
#include <limitless/util/frustum_culling.hpp>

namespace Limitless {
    class DrawParameters {
    public:
        Context& ctx;
        const Assets& assets;
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
        FrustumCulling frustum_culling;
        fx::EffectRenderer effect_renderer;

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
        void update(Scene& scene, Camera& camera) {
            frustum_culling.update(scene, camera);
            effect_renderer.update(frustum_culling.getVisibleInstances());
        }

        /**
         * Renders scene instances
         *
         * DecalInstances are not rendered
         */
        void renderScene(const DrawParameters& drawp) {
            // renders common instances except decals
            for (const auto& instance: frustum_culling.getVisibleInstances()) {
                render(*instance, drawp);
            }

            // renders batched effect instances
            effect_renderer.draw(drawp.ctx, drawp.assets, drawp.type, drawp.blending, drawp.setter);
        }

        void renderDecals(const DrawParameters& drawp) {
            for (const auto& instance: frustum_culling.getVisibleInstances()) {
                if (instance->getInstanceType() == InstanceType::Decal) {
                    render(static_cast<DecalInstance&>(*instance), drawp);
                }
            }
        }

        static void render(ModelInstance& instance, const DrawParameters& drawp) {
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

        static void render(SkeletalInstance& instance, const DrawParameters& drawp) {
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

        void render(InstancedInstance& instance, const DrawParameters& drawp) {
            if (instance.isHidden()) {
                return;
            }

            // we should take shadow influencers from shadowmap too
            // if drawp.type != Shadows
            // set instanced subset (visible for current frame path)
            instance.setVisible(frustum_culling.getVisibleModelInstanced(instance));

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

        static void render(DecalInstance& instance, const DrawParameters& drawp) {
            if (instance.isHidden() || instance.getMaterial()->getBlending() != drawp.blending) {
                return;
            }

            setBlendingSettings(instance.getMaterial()->getBlending());

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
//                    render(static_cast<EffectInstance&>(instance), drawp);
                    break;
                case InstanceType::Decal:
//                    render(static_cast<DecalInstance&>(instance), drawp);
                    break;
                case InstanceType::Terrain:
                    render(static_cast<TerrainInstance&>(instance), drawp);
                    break;
            }
        }
    };
}