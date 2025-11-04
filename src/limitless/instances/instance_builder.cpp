#include <limitless/instances/instance_builder.hpp>
#include <limitless/models/skeletal_model.hpp>
#include <limitless/instances/skeletal_instance.hpp>
#include <limitless/models/model.hpp>
#include <limitless/instances/decal_instance.hpp>
#include <limitless/models/mesh.hpp>

#include "limitless/models/cube.hpp"
#include "limitless/models/cylinder.hpp"
#include "limitless/models/line.hpp"
#include "limitless/models/plane.hpp"
#include "limitless/models/quad.hpp"
#include "limitless/models/sphere.hpp"

using namespace Limitless;

void Instance::Builder::initialize(Instance& instance) {
    // apply properties

    instance.setPosition(position_);
    instance.setRotation(rotation_);
    instance.setScale(scale_);

    if (cast_shadow_) {
        instance.castShadow();
    }

    if (bounding_box_) {
        instance.setBoundingBox(*bounding_box_);
    }

    instance.setDecalMask(decal_mask);

    // add instance attachments
    for (const auto &attachment: attachments) {
        instance.attach(attachment);
    }

    // add bone attachments
    if (instance.getInstanceType() == InstanceType::Skeletal) {
        auto& skeletal = static_cast<SkeletalInstance&>(instance);

        for (const auto &[bone, attachment]: bone_attachments) {
            skeletal.attachToBone(bone, attachment);
        }
    }
}

void Instance::Builder::initialize(const std::shared_ptr<ModelInstance>& instance) {
    // change materials
    if (global_material) {
        instance->changeMaterials(global_material);
    }

    for (const auto& [index, name, material] : changed_materials) {
        if (!name.empty()) {
            instance->changeMaterial(name, material);
        } else {
            instance->changeMaterial(index, material);
        }
    }
}

Instance::Builder::Builder() noexcept
    : texture_uv_scale_(TerrainInstance::MAX_TEXTURES, 1.0f)
    , texture_normal_depth_(TerrainInstance::MAX_TEXTURES, 2.0f)
    , texture_detile_(TerrainInstance::MAX_TEXTURES, glm::vec2{1.0f})
{

}

Instance::Builder& Instance::Builder::model(const std::shared_ptr<Model>& model) {
    model_ = model;
    return *this;
}

Instance::Builder &Instance::Builder::position(const glm::vec3& position) {
    position_ = position;
    return *this;
}

Instance::Builder &Instance::Builder::rotation(const glm::quat& rotation) {
    rotation_ = rotation;
    return *this;
}

Instance::Builder &Instance::Builder::scale(const glm::vec3& scale) {
    scale_ = scale;
    return *this;
}

Instance::Builder &Instance::Builder::cast_shadow(bool cast_shadow) {
    cast_shadow_ = cast_shadow;
    return *this;
}

Instance::Builder& Instance::Builder::bounding_box(const Box& box) {
    bounding_box_ = box;
    return *this;
}

std::shared_ptr<ModelInstance> Instance::Builder::asModel() {
    if (dynamic_cast<Cube*>(model_.get()) ||
        dynamic_cast<Cylinder*>(model_.get()) ||
        dynamic_cast<Sphere*>(model_.get()) ||
        dynamic_cast<Line*>(model_.get()) ||
        dynamic_cast<Plane*>(model_.get()) ||
        dynamic_cast<PlaneQuad*>(model_.get()) ||
        dynamic_cast<Quad*>(model_.get())
    ) {
        if (global_material) {
            auto& lods = model_->getLods();
            for (auto& [meshes, materials] : lods)
            {
                std::vector<std::shared_ptr<ms::Material>> replaced_materials;
                replaced_materials.resize(materials.size());
                for (auto& material: replaced_materials) {
                    material = global_material;
                }
                materials = std::move(replaced_materials);
            }

            auto instance = std::make_shared<ModelInstance>(model_, position_);
            initialize(*instance);
            initialize(instance);
            return instance;
        }

        throw instance_builder_exception {"Material for Elementary model is not set!"};
    }

    if (model_.get()) {
        auto instance = std::make_shared<ModelInstance>(model_, position_);
        initialize(*instance);
        initialize(instance);
        return instance;
    }

    throw instance_builder_exception {"Invalid parameters for instance builder!"};
}

std::shared_ptr<SkeletalInstance> Instance::Builder::asSkeletal() {
    if (dynamic_cast<SkeletalModel*>(model_.get())) {
        auto instance = std::make_shared<SkeletalInstance>(model_, position_);
        initialize(*instance);
        initialize(instance);
        return instance;
    } else {
        throw instance_builder_exception {model_->getName() + " is not SkeletalModel!"};
    }
}

std::shared_ptr<Instance> Instance::Builder::build() {
    if (model_) {
        if (dynamic_cast<SkeletalModel*>(model_.get())) {
            return asSkeletal();
        }

        if (model_.get()) {
            return asModel();
        }

        throw instance_builder_exception {"Invalid parameters for instance builder!"};
    }

    if (effect_) {
        auto instance = std::make_shared<EffectInstance>(effect_, position_);
        initialize(*instance);
        return instance;
    }

    throw instance_builder_exception {"Invalid parameters for instance builder!"};
}

Instance::Builder& Instance::Builder::material(uint32_t mesh_index, const std::shared_ptr<ms::Material>& material) {
    changed_materials.emplace_back(MaterialChange{mesh_index, {}, material});
    return *this;
}

Instance::Builder& Instance::Builder::material(const std::string& mesh_name, const std::shared_ptr<ms::Material>& material) {
    changed_materials.emplace_back(MaterialChange{0, mesh_name, material});
    return *this;
}


Instance::Builder& Instance::Builder::material(const std::shared_ptr<ms::Material>& material) {
    global_material = material;
    return *this;
}

Instance::Builder& Instance::Builder::attach(const std::shared_ptr<Instance>& instance) {
    attachments.emplace_back(instance);
    return *this;
}

Instance::Builder& Instance::Builder::attach(const std::string& bone_name, const std::shared_ptr<Instance>& instance) {
    bone_attachments.emplace_back(SocketAttachment{bone_name, instance});
    return *this;
}

Instance::Builder& Instance::Builder::effect(const std::shared_ptr<EffectInstance>& effect) {
    effect_ = effect;
    return *this;
}

std::shared_ptr<EffectInstance> Instance::Builder::asEffect() {
    if (effect_) {
        auto instance = std::make_shared<EffectInstance>(effect_, position_);
        initialize(*instance);
        return instance;
    } else {
        throw instance_builder_exception {"Instance builder does not have effect to build!"};
    }
}

std::shared_ptr<DecalInstance> Instance::Builder::asDecal() {
    if (!global_material) {
        throw instance_builder_exception {"Material for decal is not set!"};
    }

    // if (!dynamic_cast<ElementaryModel*>(model_.get())) {
    //     throw instance_builder_exception {"Model for decal is not elementary!"};
    // }

    auto instance = std::make_shared<DecalInstance>(model_, global_material, position_);
    initialize(*instance);
    instance->getProjectionMask() = decal_proj_mask;
    return instance;
}

Instance::Builder &Instance::Builder::decal_receipt_mask(uint8_t mask) {
    decal_mask = mask;
    return *this;
}

Instance::Builder &Instance::Builder::decal_projection_mask(uint8_t mask) {
    decal_proj_mask = mask;
    return *this;
}

std::shared_ptr<TerrainInstance> Instance::Builder::asTerrain(Assets& assets) {
    if (!height_map_) {
        throw instance_builder_exception {"Height map for terrain is not set!"};
    }

    if (!control_map_) {
        throw instance_builder_exception {"Control map for terrain is not set!"};
    }

    if (!albedo_map_) {
        throw instance_builder_exception {"Albedo for terrain is not set!"};
    }

    if (!normal_map_) {
        throw instance_builder_exception {"Normals for terrain is not set!"};
    }

    if (albedo_map_ && normal_map_) {
        if (albedo_map_->getSize().z != normal_map_->getSize().z) {
            throw instance_builder_exception {"Missing textures in albedo/normal!"};
        }
    }

    auto instance = std::make_shared<TerrainInstance>(
        height_map_,
        control_map_,
        albedo_map_,
        normal_map_,
        color_map_
    );

    initialize(*instance);

    instance->setVertexSpacing(vertex_spacing_);
    instance->setTerrainSize(terrain_size_);
    instance->setMeshLodCount(mesh_lod_count_);
    
    // Auto-calculate mesh_size if requested, otherwise use manual value
    if (auto_mesh_size_) {
        instance->autoCalculateMeshSize();
    } else {
        instance->setMeshSize(mesh_size_);
    }
    
    instance->setHeightScale(height_scale_);
    instance->setTileBilerp(enable_tile_bilerp_);
    instance->setNormalBilerpMultiplier(normal_bilerp_multiplier_);
    instance->setTileBilerpMultiplier(tile_bilerp_multiplier_);
    instance->setTextureUVScales(texture_uv_scale_);
    instance->setTextureNormalDepths(texture_normal_depth_);
    instance->setTextureDetiles(texture_detile_);

    instance->initializeMesh(assets);

    return instance;
}

Instance::Builder& Instance::Builder::terrain_size(float terrain_size) {
    terrain_size_ = terrain_size;
    return *this;
}

Instance::Builder& Instance::Builder::vertex_spacing(float vertexSpacing) {
    vertex_spacing_ = vertexSpacing;
    return *this;
}

Instance::Builder& Instance::Builder::height_scale(float heightScale) {
    height_scale_ = heightScale;
    return *this;
}

Instance::Builder& Instance::Builder::enable_tile_bilerp(bool bilerp)
{
    enable_tile_bilerp_ = bilerp;
    return *this;
}

Instance::Builder& Instance::Builder::normal_bilerp_multiplier(float multiplier)
{
    normal_bilerp_multiplier_ = multiplier;
    return *this;
}

Instance::Builder& Instance::Builder::tile_bilerp_multiplier(float multiplier)
{
    tile_bilerp_multiplier_ = multiplier;
    return *this;
}

Instance::Builder& Instance::Builder::mesh_size(float mesh_size) {
    mesh_size_ = mesh_size;
    return *this;
}

Instance::Builder& Instance::Builder::mesh_lod_count(float mesh_lod_count) {
    mesh_lod_count_ = mesh_lod_count;
    return *this;
}

Instance::Builder& Instance::Builder::auto_mesh_size() {
    auto_mesh_size_ = true;
    return *this;
}

Instance::Builder& Instance::Builder::height_map(const std::shared_ptr<Texture>& height_map) {
    height_map_ = height_map;
    return *this;
}

Instance::Builder& Instance::Builder::control_map(const std::shared_ptr<Texture>& control_map) {
    control_map_ = control_map;
    return *this;
}

Instance::Builder& Instance::Builder::albedo_map(const std::shared_ptr<Texture>& albedo_map) {
    albedo_map_ = albedo_map;
    return *this;
}

Instance::Builder& Instance::Builder::normal_map(const std::shared_ptr<Texture>& normal_map) {
    normal_map_ = normal_map;
    return *this;
}

Instance::Builder& Instance::Builder::color_map(const std::shared_ptr<Texture>& color_map) {
    color_map_ = color_map;
    return *this;
}

Instance::Builder& Instance::Builder::texture_uv_scale(const std::vector<float>& texture_uv_scale)
{
    texture_uv_scale_ = texture_uv_scale;
    return *this;
}

Instance::Builder& Instance::Builder::texture_normal_depth(const std::vector<float>& texture_normal_depth)
{
    texture_normal_depth_ = texture_normal_depth;
    return *this;
}

Instance::Builder& Instance::Builder::texture_detile(const std::vector<glm::vec2>& texture_detile)
{
    texture_detile_ = texture_detile;
    return *this;
}

Instance::Builder &Instance::Builder::height(const float* data) {
    std::vector<uint16_t> el;
    el.resize(terrain_size_ * terrain_size_);

    for (auto i = 0; i < terrain_size_ * terrain_size_; ++i) {
        el[i] = data[i] * 65535.0f;
    }

    height_map_ =
        Texture::builder()
            .target(Texture::Type::Tex2D)
            .mipmap(false)
            .internal_format(Texture::InternalFormat::R16)
            .size(glm::uvec2{terrain_size_, terrain_size_})
            .format(Texture::Format::Red)
            .data_type(Texture::DataType::UnsignedShort)
            .wrap_r(Texture::Wrap::Repeat)
            .wrap_s(Texture::Wrap::Repeat)
            .wrap_t(Texture::Wrap::Repeat)
            .min_filter(Texture::Filter::Linear)
            .mag_filter(Texture::Filter::Linear)
            .data(el.data())
            .build();

    return *this;
}

Instance::Builder &Instance::Builder::control(const TerrainInstance::control_value* data) {
    control_map_ =
        Texture::builder()
            .mipmap(false)
            .target(Texture::Type::Tex2D)
            .format(Texture::Format::RedInt)
            .internal_format(Texture::InternalFormat::R32UI)
            .size(glm::uvec2{terrain_size_, terrain_size_})
            .data_type(Texture::DataType::UnsignedInt)
            .wrap_r(Texture::Wrap::Repeat)
            .wrap_s(Texture::Wrap::Repeat)
            .wrap_t(Texture::Wrap::Repeat)
            .min_filter(Texture::Filter::Nearest)
            .mag_filter(Texture::Filter::Nearest)
            .data(data)
            .build();
    return *this;
}
