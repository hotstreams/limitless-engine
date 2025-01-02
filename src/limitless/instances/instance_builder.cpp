#include <limitless/instances/instance_builder.hpp>
#include <limitless/models/skeletal_model.hpp>
#include <limitless/instances/skeletal_instance.hpp>
#include <limitless/models/elementary_model.hpp>
#include <limitless/instances/decal_instance.hpp>
#include <limitless/models/mesh.hpp>
#include <limitless/core/indexed_stream.hpp>

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

Instance::Builder& Instance::Builder::model(const std::shared_ptr<AbstractModel>& model) {
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
    if (dynamic_cast<Model*>(model_.get())) {
        auto instance = std::make_shared<ModelInstance>(model_, position_);
        initialize(*instance);
        initialize(instance);
        return instance;
    }

    if (dynamic_cast<ElementaryModel*>(model_.get())) {
        if (global_material) {
            auto instance = std::make_shared<ModelInstance>(model_, global_material, position_);
            initialize(*instance);
            initialize(instance);
            return instance;
        } else {
            throw instance_builder_exception {"Material for Elementary model is not set!"};
        }
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

        if (dynamic_cast<Model*>(model_.get())) {
            return asModel();
        }

        if (dynamic_cast<ElementaryModel *>(model_.get())) {
            if (global_material) {
                auto instance = std::make_shared<ModelInstance>(model_, global_material, position_);
                initialize(*instance);
                initialize(instance);
                return instance;
            } else {
                throw instance_builder_exception {"Material for Elementary model is not set!"};
            }
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

    if (!dynamic_cast<ElementaryModel*>(model_.get())) {
        throw instance_builder_exception {"Model for decal is not elementary!"};
    }

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

    if (!orm_map_) {
        throw instance_builder_exception {"Orm for terrain is not set!"};
    }

    if (!noise_) {
        throw instance_builder_exception {"Noise for terrain is not set!"};
    }

    if (albedo_map_ && normal_map_ && orm_map_) {
        if (albedo_map_->getSize().z != normal_map_->getSize().z || albedo_map_->getSize().z != orm_map_->getSize().z) {
            throw instance_builder_exception {"Missing textures in albedo/normal/orm maps!"};
        }
    }

    auto instance = std::make_shared<TerrainInstance>(
        height_map_,
        control_map_,
        albedo_map_,
        normal_map_,
        orm_map_,
        noise_
    );
    initialize(*instance);

    instance->setChunkSize(chunk_size_);
    instance->setVertexSpacing(vertex_spacing_);
    instance->setHeightScale(height_scale_);
    instance->setNoise1Scale(noise1_scale_);
    instance->setNoise2Scale(noise2_scale_);
    instance->setNoise2Angle(noise2_angle_);
    instance->setNoise2Offset(noise2_offset_);
    instance->setNoise3Scale(noise3_scale_);
    instance->setMacroVariation1(macro_variation1_);
    instance->setMacroVariation2(macro_variation2_);
    instance->setMeshSize(mesh_size_);
    instance->setMeshLodCount(mesh_lod_count_);

    instance->initializeMesh(assets);

    return instance;
}

Instance::Builder& Instance::Builder::chunk_size(float chunkSize) {
    chunk_size_ = chunkSize;
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

Instance::Builder& Instance::Builder::noise1_scale(float noise1Scale) {
    noise1_scale_ = noise1Scale;
    return *this;
}

Instance::Builder& Instance::Builder::noise2_scale(float noise2Scale) {
    noise2_scale_ = noise2Scale;
    return *this;
}

Instance::Builder& Instance::Builder::noise2_angle(float noise2Angle) {
    noise2_angle_ = noise2Angle;
    return *this;
}

Instance::Builder& Instance::Builder::noise2_offset(float noise2Offset) {
    noise2_offset_ = noise2Offset;
    return *this;
}

Instance::Builder& Instance::Builder::noise3_scale(float noise3Scale) {
    noise3_scale_ = noise3Scale;
    return *this;
}

Instance::Builder& Instance::Builder::macro_variation1(const glm::vec3 &macroVariation1) {
    macro_variation1_ = macroVariation1;
    return *this;
}

Instance::Builder& Instance::Builder::macro_variation2(const glm::vec3 &macroVariation2) {
    macro_variation2_ = macroVariation2;
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

Instance::Builder& Instance::Builder::orm_map(const std::shared_ptr<Texture>& orm_map) {
    orm_map_ = orm_map;
    return *this;
}

Instance::Builder& Instance::Builder::noise(const std::shared_ptr<Texture>& noise) {
    noise_ = noise;
    return *this;
}

Instance::Builder &Instance::Builder::height(const float* data) {
    std::vector<uint16_t> el;
    el.resize(chunk_size_ * chunk_size_);

    for (auto i = 0; i < chunk_size_ * chunk_size_; ++i) {
        el[i] = data[i] * 65535.0f;
    }

    height_map_ =
        Texture::builder()
            .target(Texture::Type::Tex2D)
            .mipmap(false)
            .internal_format(Texture::InternalFormat::R16)
            .size(glm::uvec2{chunk_size_, chunk_size_})
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
            .size(glm::uvec2{chunk_size_, chunk_size_})
            .data_type(Texture::DataType::UnsignedInt)
            .wrap_r(Texture::Wrap::Repeat)
            .wrap_s(Texture::Wrap::Repeat)
            .wrap_t(Texture::Wrap::Repeat)
            .min_filter(Texture::Filter::Linear)
            .mag_filter(Texture::Filter::Linear)
            .data(data)
            .build();
    return *this;
}
