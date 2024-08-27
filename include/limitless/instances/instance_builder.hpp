#pragma once

#include <limitless/instances/instance.hpp>
#include <limitless/instances/skeletal_instance.hpp>
#include <limitless/instances/effect_instance.hpp>
#include <limitless/models/model.hpp>
#include "decal_instance.hpp"

namespace Limitless {
    class instance_builder_exception : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    class Instance::Builder {
    private:
        std::shared_ptr<AbstractModel> model_;
        std::shared_ptr<EffectInstance> effect_;

        glm::quat rotation_ {1.0f, 0.0f, 0.0f, 0.0f};
        glm::vec3 position_ {0.0f};
        glm::vec3 scale_ {1.0f};
        bool cast_shadow_ {true};
        std::optional<Box> bounding_box_ {};
        uint8_t decal_mask {0xFF};
        uint8_t decal_proj_mask {0xFF};

        class MaterialChange {
        public:
            uint32_t index;
            std::string name;
            std::shared_ptr<ms::Material> material;
        };
        std::vector<MaterialChange> changed_materials;

        std::shared_ptr<ms::Material> global_material;

        std::vector<std::shared_ptr<Instance>> attachments;

        class SocketAttachment {
        public:
            std::string bone_name;
            std::shared_ptr<Instance> attachment;
        };
        std::vector<SocketAttachment> bone_attachments;

        void initialize(Instance& instance);
        void initialize(const std::shared_ptr<ModelInstance>& instance);
    public:
        Builder() noexcept = default;

        /**
         *  Sets Model to building instance
         */
        Builder& model(const std::shared_ptr<AbstractModel>& model);

        /**
         *  Sets Model to effect instance
         */
        Builder& effect(const std::shared_ptr<EffectInstance>& effect);

        /**
         *  Sets position to building instance
         */
        Builder& position(const glm::vec3& position);

        /**
         *  Sets rotation to building instance
         */
        Builder& rotation(const glm::quat& rotation);

        /**
         *  Sets scale to building instance
         */
        Builder& scale(const glm::vec3& scale);

        /**
         *  Sets whether instance should cast shadow
         */
        Builder& cast_shadow(bool cast_shadow);

        /**
         *  Sets custom bounding box
         *
         *  note: MUST BE SET for EffectInstance
         *  note: overrides calculated one from underlying model (but takes into consideration instance transformation)
         *  note: does not work for InstancedInstance (should be set on particular instance of InstancedInstance)
         */
        Builder& bounding_box(const Box& box);

        /**
         *  Sets decal receipt mask
         *
         *  8 unique bits for each 'type collection'
         *
         *  0xFF - for receiving all decals
         *  0x00 - for not receiving
         */
        Builder& decal_receipt_mask(uint8_t mask);

        /**
         *  Sets decal projection mask for DecalInstance
         *
         *  Projects on Instances when [receipt mask & projection mask != 0]
         */
        Builder& decal_projection_mask(uint8_t mask);

        /**
         *  Replaces default mesh material with specified one
         */
        Builder& material(uint32_t mesh_index, const std::shared_ptr<ms::Material>& material);

        /**
         *  Replaces default mesh material with specified one
         */
        Builder& material(const std::string& mesh_name, const std::shared_ptr<ms::Material>& material);

        /**
         *  Replaces all model materials with specified one
         */
        Builder& material(const std::shared_ptr<ms::Material>& material);

        /**
         *  Attaches specified instance to building one
         */
        Builder& attach(const std::shared_ptr<Instance>& instance);

        /**
         *  Attaches specified instance to bone
         */
        Builder& attach(const std::string& bone_name, const std::shared_ptr<Instance>& instance);

        /**
         *
         */
        std::shared_ptr<Instance> build();

        /**
         *
         */
        std::shared_ptr<ModelInstance> asModel();

        /**
         *
         */
        std::shared_ptr<SkeletalInstance> asSkeletal();

        /**
         *
         */
        std::shared_ptr<EffectInstance> asEffect();

        /**
         *
         */
        std::shared_ptr<DecalInstance> asDecal();
    };
}