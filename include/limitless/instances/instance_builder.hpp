#pragma once

#include <limitless/instances/instance.hpp>
#include <limitless/instances/skeletal_instance.hpp>
#include <limitless/instances/effect_instance.hpp>
#include <limitless/instances/decal_instance.hpp>
#include <limitless/instances/terrain_instance.hpp>
#include <limitless/models/model.hpp>

namespace Limitless {
    class instance_builder_exception : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    /**
     *
     */
    class Instance::Builder {
    private:
        /**
         * Instance data
         */
        glm::quat rotation_ {1.0f, 0.0f, 0.0f, 0.0f};
        glm::vec3 position_ {0.0f};
        glm::vec3 scale_ {1.0f};
        bool cast_shadow_ {true};
        std::optional<Box> bounding_box_ {};
        uint8_t decal_mask {0xFF};

        /**
         * Model data
         */
        std::shared_ptr<AbstractModel> model_;

        class MaterialChange {
        public:
            uint32_t index;
            std::string name;
            std::shared_ptr<ms::Material> material;
        };
        std::vector<MaterialChange> changed_materials;

        std::shared_ptr<ms::Material> global_material;

        std::vector<std::shared_ptr<Instance>> attachments;

        /**
         * Skeletal data
         */
        class SocketAttachment {
        public:
            std::string bone_name;
            std::shared_ptr<Instance> attachment;
        };
        std::vector<SocketAttachment> bone_attachments;

        /**
         * Effect data
         */
        std::shared_ptr<EffectInstance> effect_;

        /**
         * Decal data
         */
        uint8_t decal_proj_mask {0xFF};

        /**
         * Terrain data
         */
        float chunk_size_ {1024.0f};
        float vertex_spacing_ {0.1f};
        float height_scale_ {10.0f};
        float noise1_scale_ {0.225f};
        float noise2_scale_ {0.04f};
        float noise2_angle_ {0.0f};
        float noise2_offset_ {0.5f};
        float noise3_scale_ {0.076f};
        glm::vec3 macro_variation1_ = glm::vec3(0.5f);
        glm::vec3 macro_variation2_ = glm::vec3(0.33f);
        uint32_t mesh_size {64};
        uint32_t mesh_lod_count {6};
        std::shared_ptr<Texture> height_map_;
        std::shared_ptr<Texture> control_map_;
        std::shared_ptr<Texture> albedo_map_;
        std::shared_ptr<Texture> normal_map_;
        std::shared_ptr<Texture> orm_map_;
        std::shared_ptr<Texture> noise_;

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

        Builder& chunk_size(float chunk_size);

        Builder& vertex_spacing(float vertex_spacing);

        Builder& height_scale(float height_scale);

        Builder& noise1_scale(float noise1_scale);

        Builder& noise2_scale(float noise2_scale);

        Builder& noise2_angle(float noise2_angle);

        Builder& noise2_offset(float noise2_offset);

        Builder& noise3_scale(float noise3_scale);

        Builder& macro_variation1(const glm::vec3& macro_variation1);

        Builder& macro_variation2(const glm::vec3& macro_variation2);

        Builder& height_map(const std::shared_ptr<Texture>& height_map);
        Builder& control_map(const std::shared_ptr<Texture>& control_map);
        Builder& albedo_map(const std::shared_ptr<Texture>& albedo_map);
        Builder& normal_map(const std::shared_ptr<Texture>& normal_map);
        Builder& orm_map(const std::shared_ptr<Texture>& orm_map);
        Builder& noise(const std::shared_ptr<Texture>& noise);

        Builder& height(const float* data);
        Builder& control(const TerrainInstance::control_value* data);

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

        /**
         *
         */
        std::shared_ptr<TerrainInstance> asTerrain(Assets& assets);
    };
}