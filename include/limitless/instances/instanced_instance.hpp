#pragma once

#include <limitless/instances/model_instance.hpp>
#include <limitless/core/buffer/buffer_builder.hpp>
#include <limitless/core/context.hpp>
#include <limitless/renderer/shader_type.hpp>
#include <map>

namespace Limitless {
    class InstancedInstance : public Instance {
    protected:
        // contains all instanced models
        std::vector<std::shared_ptr<ModelInstance>> instances;

        // contains instances to be drawn in current frame (flat list for backwards compatibility)
        std::vector<std::shared_ptr<ModelInstance>> visible_instances;

        // contains visible instances grouped by LOD level
        std::map<uint32_t, std::vector<std::shared_ptr<ModelInstance>>> lod_visible_instances;

        // contains per-LOD buffers with model matrices
        std::map<uint32_t, std::shared_ptr<Buffer>> lod_buffers;

        // tracks current instance data per LOD to detect changes
        std::map<uint32_t, std::vector<Data>> lod_current_data;

        virtual void updateInstanceBuffer();

        // Helper to ensure buffer exists for a given LOD level
        std::shared_ptr<Buffer>& ensureLodBuffer(uint32_t lod);
    public:
        explicit InstancedInstance(InstanceType container_type = InstanceType::Instanced);
        ~InstancedInstance() override = default;

        InstancedInstance(const InstancedInstance& rhs);
        InstancedInstance(InstancedInstance&&) noexcept = default;

        std::unique_ptr<Instance> clone() noexcept override;

        void add(const std::shared_ptr<ModelInstance>& instance);
        virtual void remove(uint64_t id);

        void update(const Camera &camera) override;

        auto& getInstances() noexcept { return instances; }
        [[nodiscard]] const auto& getInstances() const noexcept { return instances; }
        auto& getVisibleInstances() noexcept { return visible_instances; }

        /**
         * Gets visible instances grouped by LOD level
         */
        [[nodiscard]] const auto& getLodVisibleInstances() const noexcept { return lod_visible_instances; }

        /**
         * Gets buffer for a specific LOD level (nullptr if no instances at that LOD)
         */
        [[nodiscard]] std::shared_ptr<Buffer> getLodBuffer(uint32_t lod) const noexcept;

        /**
         *  Sets visible instances to specified subset
         */
        void setVisible(const std::vector<std::shared_ptr<ModelInstance>>& visible);
    };
}