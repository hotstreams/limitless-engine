#pragma once

#include <limitless/instances/model_instance.hpp>
#include <limitless/core/buffer/buffer_builder.hpp>
#include <limitless/core/context.hpp>

namespace Limitless {
    class Scene;

    class InstancedInstance : public Instance {
    protected:
        // contains all instanced models
        std::vector<std::shared_ptr<ModelInstance>> instances;

        // contains instances to be drawn in current frame
        std::vector<std::shared_ptr<ModelInstance>> visible_instances;

        // contains model matrices for each ModelInstance
        std::shared_ptr<Buffer> buffer;

        std::vector<glm::mat4> current_data;

        void updateBoundingBox() noexcept override;
        void updateInstanceBuffer();
    public:
        InstancedInstance();
        ~InstancedInstance() override = default;

        InstancedInstance(const InstancedInstance& rhs);
        InstancedInstance(InstancedInstance&&) noexcept = default;

        std::unique_ptr<Instance> clone() noexcept override;

        void add(const std::shared_ptr<ModelInstance>& instance);
        void remove(uint64_t id);

        void update(Context& context, const Camera& camera) override;

        auto& getInstances() noexcept { return instances; }
        auto& getBuffer() noexcept { return buffer ; }

        /**
         *  Sets visible instances to specified subset
         */
        void setVisible(const std::vector<std::shared_ptr<ModelInstance>>& visible);

        void draw(Context& ctx, const Assets& assets, ShaderType pass, ms::Blending blending, const UniformSetter& uniform_set) override;
    };
}