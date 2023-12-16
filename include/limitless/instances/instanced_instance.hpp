#pragma once

#include <limitless/instances/model_instance.hpp>
#include <limitless/core/buffer/buffer_builder.hpp>
#include <limitless/core/context.hpp>

namespace Limitless {
    class InstancedInstance : public Instance {
    protected:
        // contains instanced models
        std::vector<std::shared_ptr<ModelInstance>> instances;

        // contains model matrices for each ModelInstance
        std::shared_ptr<Buffer> buffer;

        std::vector<glm::mat4> current_data;

        void updateBoundingBox() noexcept override;
        void updateBuffer();
    public:
        InstancedInstance();
        ~InstancedInstance() override = default;

        InstancedInstance(const InstancedInstance& rhs);
        InstancedInstance(InstancedInstance&&) noexcept = default;

        std::unique_ptr<Instance> clone() noexcept override;

        void add(const std::shared_ptr<ModelInstance>& instance);
        void remove(uint64_t id);

        void update(Context& context, const Camera& camera) override;
        void draw(Context& ctx, const Assets& assets, ShaderType pass, ms::Blending blending, const UniformSetter& uniform_set) override;
    };
}