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
    class Instance::Builder
    {
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
        std::shared_ptr<Model> model_;

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

        float terrain_size_ {1024.0f};
        float vertex_spacing_ {1.0f};
        float height_scale_ {1.0f};

        bool enable_tile_bilerp_ {true};
        float normal_bilerp_multiplier_ {1.0f};
        float tile_bilerp_multiplier_ {1.0f};

        uint32_t mesh_size_ {64};
        uint32_t mesh_lod_count_ {6};
        bool auto_mesh_size_ {false};

        std::shared_ptr<Texture> height_map_;
        std::shared_ptr<Texture> control_map_;
        std::shared_ptr<Texture> albedo_map_;
        std::shared_ptr<Texture> normal_map_;
        std::shared_ptr<Texture> color_map_;

        std::vector<float> texture_uv_scale_;
        std::vector<float> texture_normal_depth_;
        std::vector<glm::vec2> texture_detile_;

        // Terrain material feature toggles (compile-time shader defines)
        bool terrain_detiling_ {true};
        bool terrain_blending_ {true};
        bool terrain_layering_ {true};
        bool terrain_high_blending_ {true};

        void initialize(Instance& instance);
        void initialize(const std::shared_ptr<ModelInstance>& instance);
    public:
        Builder() noexcept;

        /**
         *  Sets Model to building instance
         */
        Builder& model(const std::shared_ptr<Model>& model);

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
         * Terrain size in vertices (width == height).
         *
         * This value becomes `terrain_size` in terrain shaders and is used for:
         * - bounds checks / discard outside [0, terrain_size)
         * - texel wrapping and UV conversion
         *
         * Must match the dimensions of `height_map` and `control_map` when those are created by the builder
         * (`height()` / `control()`), and should match any externally provided textures too.
         *
         * Perf notes:
         * - large sizes increase memory bandwidth for sampling and increase CPU cost when updating height/control.
         */
        Builder& terrain_size(float terrain_size);

        /**
         * World-space spacing between terrain vertices (meters per vertex).
         *
         * This controls the mapping between world position and height/control texels:
         * `terrain_texel_uv = world_xz * (1 / vertex_spacing)`.
         *
         * Perf/quality:
         * - larger spacing = fewer vertices / less geometric detail
         * - smaller spacing = more detail but more aliasing pressure and can increase clipmap update work
         */
        Builder& vertex_spacing(float vertex_spacing);

        /**
         * Vertical displacement scale for the height map.
         *
         * Height is sampled from `terrain_height_texture` and multiplied by `terrain_height_scale`.
         *
         * Note: bounds/culling expand in Y using this value.
         */
        Builder& height_scale(float height_scale);

        /**
         * Enables per-vertex tile bilerp for control & color maps (used by terrain layering).
         *
         * Shader: uniform `enable_tile_bilerp`.
         * Effect:
         * - when enabled, the shader blends control/color from up to 4 neighboring texels (smoother transitions)
         * - when disabled, uses the single base texel (faster, sharper)
         *
         * Perf: can increase texture fetches significantly when `terrain_layering(true)`.
         */
        Builder& enable_tile_bilerp(bool bilerp = true);

        /**
         * Threshold on derived mip level that decides when normals are bilerp'd.
         *
         * Shader: `normal_bilerp = region_mip < normal_bilerp_multiplier`.
         * Higher values keep expensive normal sampling for longer distances.
         */
        Builder& normal_bilerp_multiplier(float multiplier);

        /**
         * Threshold on derived mip level that decides when tile bilerp is considered.
         *
         * Shader: `tile_bilerp_candidate = region_mip < tile_bilerp_multiplier`.
         */
        Builder& tile_bilerp_multiplier(float multiplier);

        // Terrain material feature toggles (compile-time shader defines)
        /**
         * Enables `ENGINE_MATERIAL_TERRAIN_DETILING` (rotation/randomization to break tiling).
         *
         * Driven by material uniform `_detiling`.
         * Perf: increases ALU and may use textureGrad paths.
         */
        Builder& terrain_detiling(bool enabled = true);

        /**
         * Enables `ENGINE_MATERIAL_TERRAIN_BLENDING` (base/extra texture blending based on control map).
         *
         * Driven by material uniform `_blending`.
         * Perf: increases texture fetches depending on your control map.
         */
        Builder& terrain_blending(bool enabled = true);

        /**
         * Enables `ENGINE_MATERIAL_TERRAIN_LAYERING` (tile bilerp path and multi-texel control sampling).
         *
         * Driven by material uniform `_layering`.
         * Perf: this is one of the biggest multipliers for texture fetches, especially with tile bilerp.
         */
        Builder& terrain_layering(bool enabled = true);

        /**
         * Enables `ENGINE_MATERIAL_TERRAIN_HIGH_BLENDING` (sharper / height-aware weight computation).
         *
         * Driven by material uniform `_height_blending`.
         * Perf: extra ALU; quality improvement for transitions.
         */
        Builder& terrain_high_blending(bool enabled = true);

        /**
         * GeoClipMap base patch resolution (in vertices per tile edge).
         *
         * Constraints:
         * - must be >= 4 (generation requires some minimum)
         * - best as a power-of-two-ish size for clean rings (common: 32, 64)
         */
        Builder& mesh_size(float mesh_size);

        /**
         * GeoClipMap LOD ring count.
         *
         * More rings increases visible coverage but increases draw calls / instances and update work.
         */
        Builder& mesh_lod_count(float mesh_lod_count);
        
        /**
         * Auto-calculates mesh_size based on terrain_size and mesh_lod_count
         * to ensure full terrain coverage. Call this instead of mesh_size().
         */
        Builder& auto_mesh_size();

        /**
         * Sets externally-created height map texture (Tex2D, size terrain_size x terrain_size).
         *
         * Sampled in shaders:
         * - `fetchTerrainHeight` (texelFetch)
         * - `getTerrainHeight` (texture)
         *
         * Format: should be a single-channel height (e.g., R16/R32F); builder doesn't enforce the exact format.
         */
        Builder& height_map(const std::shared_ptr<Texture>& height_map);

        /**
         * Sets externally-created control map texture (Tex2D, size terrain_size x terrain_size).
         *
         * Format expectation: R32UI (see `TerrainInstance::control_value` encoding).
         */
        Builder& control_map(const std::shared_ptr<Texture>& control_map);

        /**
         * Sets terrain albedo texture array (Tex2DArray). Layer count must match normal_map's layer count.
         *
         * Layer indices come from control map base/extra ids (6 bits => up to 64).
         */
        Builder& albedo_map(const std::shared_ptr<Texture>& albedo_map);

        /**
         * Sets terrain normal texture array (Tex2DArray). Layer count must match albedo_map's layer count.
         */
        Builder& normal_map(const std::shared_ptr<Texture>& normal_map);

        /**
         * Optional color map (Tex2D, RGBA8 in sample). Used as tint/roughness modulation in shader.
         */
        Builder& color_map(const std::shared_ptr<Texture>& color_map);

        /**
         * Per-layer UV scale for terrain textures.
         * Size should be >= layer count, and typically exactly TerrainInstance::MAX_TEXTURES (64).
         */
        Builder& texture_uv_scale(const std::vector<float>& texture_uv_scale);

        /**
         * Per-layer normal depth/intensity.
         * Size should be >= layer count, and typically exactly TerrainInstance::MAX_TEXTURES (64).
         */
        Builder& texture_normal_depth(const std::vector<float>& texture_normal_depth);

        /**
         * Per-layer detiling parameters used when `terrain_detiling(true)`.
         * Size should be >= layer count, and typically exactly TerrainInstance::MAX_TEXTURES (64).
         */
        Builder& texture_detile(const std::vector<glm::vec2>& texture_detile);

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