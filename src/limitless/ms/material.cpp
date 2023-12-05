#include <limitless/ms/material.hpp>

#include <limitless/core/context_initializer.hpp>

#include <limitless/core/texture/bindless_texture.hpp>
#include <limitless/core/uniform/uniform_sampler.hpp>
#include <limitless/core/uniform/uniform_time.hpp>
#include <limitless/core/buffer/buffer_builder.hpp>
#include <limitless/ms/material_builder.hpp>

#include <cstring>

using namespace Limitless::ms;
using namespace Limitless;

void Material::Buffer::initialize(const Material& material) {
    // check the order in material.glsl
    // std140
    // https://www.khronos.org/registry/OpenGL/specs/gl/glspec45.core.pdf#page=159

    size_t offset = 0;
    const auto offset_setter = [&] (const auto& container, const std::function<bool(const Uniform&)>& condition = {}) {
        for (const auto& [key, uniform] : container) {
            if (!ContextInitializer::isBindlessTextureSupported() && uniform->getType() == UniformType::Sampler) {
                continue;
            }

            if (condition && !condition(*uniform)) {
                continue;
            }

            const auto size = getUniformSize(*uniform);
            const auto alignment = getUniformAlignment(*uniform);

            offset += offset % alignment ? alignment - offset % alignment : 0;

            uniform_offsets.emplace(uniform->getName(), offset);
            offset += size;
        }
    };

    offset_setter(material.getProperties());
    offset_setter(material.getUniforms(), [] (const Uniform& uniform) { return uniform.getType() == UniformType::Sampler; });
    offset_setter(material.getUniforms(), [] (const Uniform& uniform) { return uniform.getType() != UniformType::Sampler; });

    // ShadingModel uint32_t
    offset += sizeof(uint32_t);

    material_buffer = ::Buffer::builder()
            .target(::Buffer::Type::Uniform)
            .usage(::Buffer::Usage::DynamicDraw)
            .access(::Buffer::MutableAccess::WriteOrphaning)
            .size(sizeof(std::byte) * offset)
            .build();
}

template<typename V>
void Material::Buffer::map(std::vector<std::byte>& block, const Uniform& uniform) const {
    const auto& uni = static_cast<const UniformValue<V>&>(uniform);
    const auto offset = uniform_offsets.at(uniform.getName());
    std::memcpy(block.data() + offset, &uni.getValue(), sizeof(V));
}

void Material::Buffer::map(std::vector<std::byte>& block, Uniform& uniform) {
    switch (uniform.getType()) {
        case UniformType::Value:
            switch (uniform.getValueType()) {
                case UniformValueType::Uint:
                    map<unsigned int>(block, uniform);
                    break;
                case UniformValueType::Int:
                    map<int>(block, uniform);
                    break;
                case UniformValueType::Float:
                    map<float>(block, uniform);
                    break;
                case UniformValueType::Vec2:
                    map<glm::vec2>(block, uniform);
                    break;
                case UniformValueType::Vec3:
                    map<glm::vec3>(block, uniform);
                    break;
                case UniformValueType::Vec4:
                    map<glm::vec4>(block, uniform);
                    break;
                case UniformValueType::Mat4:
                    map<glm::mat4>(block, uniform);
                    break;
                case UniformValueType::Mat3:
                    map<glm::mat3>(block, uniform);
                    break;
            }
            break;
        case UniformType::Sampler:
            if (ContextInitializer::isBindlessTextureSupported()) {
                const auto& uni = static_cast<const UniformSampler&>(uniform); //NOLINT
                const auto offset = uniform_offsets.at(uniform.getName());
                auto& bindless_texture = static_cast<BindlessTexture&>(uni.getSampler()->getExtensionTexture()); //NOLINT
                bindless_texture.makeResident();
                std::memcpy(block.data() + offset, &bindless_texture.getHandle(), sizeof(uint64_t));
            }
            break;
        case UniformType::Time: {
            auto& time = static_cast<UniformTime&>(uniform); //NOLINT
            time.update();
            map<float>(block, uniform);
            break;
        }
    }
}

Material::Buffer::Buffer(const Material &material) {
    initialize(material);
}

Material::Buffer::Buffer(const Buffer& buffer)
    : material_buffer {buffer.material_buffer->clone()}
    , uniform_offsets {buffer.uniform_offsets} {
}

Material::Buffer& Material::Buffer::operator=(const Buffer& buf) {
    auto copy = Buffer {buf};
    std::swap(material_buffer, copy.material_buffer);
    std::swap(uniform_offsets, copy.uniform_offsets);
    return *this;
}

const std::shared_ptr<Limitless::Buffer>& Material::Buffer::getBuffer() const noexcept {
    return material_buffer;
}

void Material::Buffer::map(const Material& material) {
    std::vector<std::byte> block(material_buffer->getSize());

    for (const auto& [_, uniform] : material.getProperties()) {
        map(block, *uniform);
    }

    for (const auto& [_, uniform] : material.getUniforms()) {
        map(block, *uniform);
    }

    // adding shading in the end
    auto s = material.getShading();
    std::memcpy(block.data() + block.size() - sizeof(uint32_t), &s, sizeof(uint32_t));

    material_buffer->mapData(block.data(), block.size());
}

void Limitless::ms::swap(Material& lhs, Material& rhs) noexcept {
    using std::swap;

    swap(lhs.properties, rhs.properties);
    swap(lhs.blending, rhs.blending);
    swap(lhs.shading, rhs.shading);
    swap(lhs.two_sided, rhs.two_sided);
    swap(lhs.refraction, rhs.refraction);
    swap(lhs.name, rhs.name);
    swap(lhs.shader_index, rhs.shader_index);
    swap(lhs.model_shaders, rhs.model_shaders);
    swap(lhs.uniforms, rhs.uniforms);
    swap(lhs.vertex_snippet, rhs.vertex_snippet);
    swap(lhs.fragment_snippet, rhs.fragment_snippet);
    swap(lhs.global_snippet, rhs.global_snippet);
    swap(lhs.shading_snippet, rhs.shading_snippet);
    swap(lhs.buffer, rhs.buffer);
}

Material::Material(const Material& material)
    : blending {material.blending}
    , shading {material.shading}
    , two_sided {material.two_sided}
    , refraction {material.refraction}
    , name {material.name}
    , shader_index {material.shader_index}
    , model_shaders {material.model_shaders}
    , vertex_snippet {material.vertex_snippet}
    , fragment_snippet {material.fragment_snippet}
    , global_snippet {material.global_snippet}
    , shading_snippet {material.shading_snippet}
    , buffer {material.buffer} {

    // deep copy of properties
    for (const auto& [type, property] : material.properties) {
        properties.emplace(type, property->clone());
    }

    // deep copy of custom properties
    for (const auto& [uniform_name, uniform] : material.uniforms) {
        uniforms.emplace(uniform_name, uniform->clone());
    }

    update();
}

Material& Material::operator=(Material material) {
    swap(*this, material);
    return *this;
}

bool Limitless::ms::operator==(const Material& lhs, const Material& rhs) noexcept {
    return !(lhs < rhs) && !(rhs < lhs);
}

bool Limitless::ms::operator<(const Material& lhs, const Material& rhs) noexcept {
    {
        if (lhs.properties.size() != rhs.properties.size()) {
            return lhs.properties.size() < rhs.properties.size();
        }

        auto lhs_it = lhs.properties.begin();
        auto rhs_it = rhs.properties.begin();

        while (lhs_it != lhs.properties.end()) {
            if (lhs_it->first != rhs_it->first) {
                return lhs_it->first < rhs_it->first;
            }

            if (*lhs_it->second != *rhs_it->second) {
                return *lhs_it->second < *rhs_it->second;
            }

            lhs_it++;
            rhs_it++;
        }
    }

    {
        if (lhs.uniforms.size() != rhs.uniforms.size()) {
            return lhs.uniforms.size() < rhs.uniforms.size();
        }

        auto lhs_it = lhs.uniforms.begin();
        auto rhs_it = rhs.uniforms.begin();

        while (lhs_it != lhs.uniforms.end()) {
            if (lhs_it->first != rhs_it->first) {
                return lhs_it->first < rhs_it->first;
            }

            if (*lhs_it->second != *rhs_it->second) {
                return *lhs_it->second < *rhs_it->second;
            }

            lhs_it++;
            rhs_it++;
        }
    }

    return std::tie(lhs.blending, lhs.two_sided, lhs.shader_index) < std::tie(rhs.blending, rhs.two_sided, rhs.shader_index);
}

void Material::update() {
    const auto properties_changed = std::any_of(properties.begin(), properties.end(), [] (auto& property) {
        return property.second->isChanged();
    });
    const auto uniforms_changed = std::any_of(uniforms.begin(), uniforms.end(), [] (auto& uniform) {
        return uniform.second->isChanged() || uniform.second->getType() == UniformType::Time;
    });

    if (!properties_changed && !uniforms_changed) {
        return;
    }

    buffer.map(*this);

    for (const auto& [_, uniform] : properties) {
        uniform->resetChanged();
    }

    for (const auto& [_, uniform] : uniforms) {
        uniform->resetChanged();
    }
}

const glm::vec4& Material::getColor() const {
    try {
        return static_cast<UniformValue<glm::vec4>&>(*properties.at(Property::Color)).getValue(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - Color");
    }
}

const glm::vec3& Material::getEmissiveColor() const {
    try {
        return static_cast<UniformValue<glm::vec3>&>(*properties.at(Property::EmissiveColor)).getValue(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - Emissive Color");
    }
}

float Material::getMetallic() const {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(Property::Metallic)).getValue(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - Metallic");
    }
}

float Material::getRoughness() const {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(Property::Roughness)).getValue(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - Roughness");
    }
}

float Material::getIoR() const {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(Property::IoR)).getValue(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - IoR");
    }
}

float Material::getAbsorption() const {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(Property::Absorption)).getValue(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - Absorption");
    }
}

float Material::getMicroThickness() const {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(Property::MicroThickness)).getValue(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - MicroThickness");
    }
}

float Material::getThickness() const {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(Property::Thickness)).getValue(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - Thickness");
    }
}

float Material::getReflectance() const {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(Property::Reflectance)).getValue(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - Reflectance");
    }
}

float Material::getTransmission() const {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(Property::Transmission)).getValue(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - Transmission");
    }
}

const std::shared_ptr<Texture>& Material::getDiffuseTexture() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::Diffuse)).getSampler(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - Diffuse");
    }
}

const std::shared_ptr<Texture>& Material::getNormalTexture() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::Normal)).getSampler(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - Normal");
    }
}

const std::shared_ptr<Texture>& Material::getEmissiveMaskTexture() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::EmissiveMask)).getSampler(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - EmissiveMask");
    }
}

const std::shared_ptr<Texture>& Material::getBlendMaskTexture() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::BlendMask)).getSampler(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - BlendMask");
    }
}

const std::shared_ptr<Texture>& Material::getMetallicTexture() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::MetallicTexture)).getSampler(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - MetallicTexture");
    }
}

const std::shared_ptr<Texture>& Material::getRoughnessTexture() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::RoughnessTexture)).getSampler(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - RoughnessTexture");
    }
}

const std::shared_ptr<Texture>& Material::getORMTexture() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::ORM)).getSampler(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - ORM");
    }
}

const std::shared_ptr<Texture>& Material::getAmbientOcclusionTexture() const {
    try {
        return static_cast<UniformSampler&>(*properties.at(Property::AmbientOcclusionTexture)).getSampler(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - AmbientOcclusionTexture");
    }
}

glm::vec4& Material::getColor() {
    try {
        return static_cast<UniformValue<glm::vec4>&>(*properties.at(Property::Color)).getValue(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - Color");
    }
}

glm::vec3& Material::getEmissiveColor() {
    try {
        return static_cast<UniformValue<glm::vec3>&>(*properties.at(Property::EmissiveColor)).getValue(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - EmissiveColor");
    }
}

float& Material::getMetallic() {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(Property::Metallic)).getValue(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - Metallic");
    }
}

float& Material::getRoughness() {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(Property::Roughness)).getValue(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - Roughness");
    }
}

float& Material::getIoR() {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(Property::IoR)).getValue(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - IoR");
    }
}

float& Material::getAbsorption() {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(Property::Absorption)).getValue(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - Absorption");
    }
}

float& Material::getMicroThickness() {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(Property::MicroThickness)).getValue(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - MicroThickness");
    }
}

float& Material::getThickness() {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(Property::Thickness)).getValue(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - Thickness");
    }
}

float& Material::getReflectance() {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(Property::Reflectance)).getValue(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - Reflectance");
    }
}

float& Material::getTransmission() {
    try {
        return static_cast<UniformValue<float>&>(*properties.at(Property::Transmission)).getValue(); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - Transmission");
    }
}

Blending& Material::getBlending() {
    return blending;
}

void Material::setColor(const glm::vec4& value) {
    try {
        static_cast<UniformValue<glm::vec4>&>(*properties.at(Property::Color)).setValue(value); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - Color");
    }
}

void Material::setEmissiveColor(const glm::vec3& value) {
    try {
        static_cast<UniformValue<glm::vec3>&>(*properties.at(Property::EmissiveColor)).setValue(value); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - EmissiveColor");
    }
}

void Material::setMetallic(float value) {
    try {
        static_cast<UniformValue<float>&>(*properties.at(Property::Metallic)).setValue(value); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - Metallic");
    }
}

void Material::setRoughness(float value) {
    try {
        static_cast<UniformValue<float>&>(*properties.at(Property::Roughness)).setValue(value); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - Roughness");
    }
}

void Material::setIoR(float value) {
    try {
        static_cast<UniformValue<float>&>(*properties.at(Property::IoR)).setValue(value); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - IoR");
    }
}

void Material::setAbsorption(float value) {
    try {
        static_cast<UniformValue<float>&>(*properties.at(Property::Absorption)).setValue(value); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - Absorption");
    }
}

void Material::setMicroThickness(float value) {
    try {
        static_cast<UniformValue<float>&>(*properties.at(Property::MicroThickness)).setValue(value); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - MicroThickness");
    }
}

void Material::setThickness(float value) {
    try {
        static_cast<UniformValue<float>&>(*properties.at(Property::Thickness)).setValue(value); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - Thickness");
    }
}

void Material::setReflectance(float value) {
    try {
        static_cast<UniformValue<float>&>(*properties.at(Property::Reflectance)).setValue(value); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - Reflectance");
    }
}

void Material::setTransmission(float value) {
    try {
        static_cast<UniformValue<float>&>(*properties.at(Property::Transmission)).setValue(value); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - Transmission");
    }
}

void Material::setDiffuseTexture(const std::shared_ptr<Texture>& texture) {
    try {
        static_cast<UniformSampler&>(*properties.at(Property::Diffuse)).setSampler(texture); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - Diffuse");
    }
}

void Material::setNormalTexture(const std::shared_ptr<Texture>& texture) {
    try {
        static_cast<UniformSampler&>(*properties.at(Property::Normal)).setSampler(texture); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - Normal");
    }
}

void Material::setEmissiveMaskTexture(const std::shared_ptr<Texture>& texture) {
    try {
        static_cast<UniformSampler&>(*properties.at(Property::EmissiveMask)).setSampler(texture); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - EmissiveMask");
    }
}

void Material::setBlendMaskTexture(const std::shared_ptr<Texture>& texture) {
    try {
        static_cast<UniformSampler&>(*properties.at(Property::BlendMask)).setSampler(texture); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - BlendMask");
    }
}

void Material::setMetallicTexture(const std::shared_ptr<Texture>& texture) {
    try {
        static_cast<UniformSampler&>(*properties.at(Property::MetallicTexture)).setSampler(texture); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - MetallicTexture");
    }
}

void Material::setRoughnessTexture(const std::shared_ptr<Texture>& texture) {
    try {
        static_cast<UniformSampler&>(*properties.at(Property::RoughnessTexture)).setSampler(texture); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - RoughnessTexture");
    }
}

void Material::setAmbientOcclusionTexture(const std::shared_ptr<Texture>& texture) {
    try {
        static_cast<UniformSampler&>(*properties.at(Property::AmbientOcclusionTexture)).setSampler(texture); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - AmbientOcclusionTexture");
    }
}

void Material::setORMTexture(const std::shared_ptr<Texture>& texture) {
    try {
        static_cast<UniformSampler&>(*properties.at(Property::ORM)).setSampler(texture); //NOLINT
    } catch (const std::out_of_range& e) {
        throw material_exception("Material property not found - ORM");
    }
}

void Material::setBlending(Blending _blending) {
    blending = _blending;
}

Blending Material::getBlending() const noexcept {
    return blending;
}

Shading Material::getShading() const noexcept {
    return shading;
}

bool Material::getTwoSided() const noexcept {
    return two_sided;
}

bool Material::getRefraction() const noexcept {
    return refraction;
}

const std::string& Material::getName() const noexcept {
    return name;
}

uint64_t Material::getShaderIndex() const noexcept {
    return shader_index;
}

const std::string& Material::getVertexSnippet() const noexcept {
    return vertex_snippet;
}

const std::string& Material::getFragmentSnippet() const noexcept {
    return fragment_snippet;
}

const std::string& Material::getGlobalSnippet() const noexcept {
    return global_snippet;
}

const std::string& Material::getShadingSnippet() const noexcept {
    return shading_snippet;
}

const InstanceTypes& Material::getModelShaders() const noexcept {
    return model_shaders;
}

const Material::Buffer &Material::getBuffer() const noexcept {
    return buffer;
}

const std::map<Property, std::unique_ptr<Uniform>>& Material::getProperties() const noexcept {
    return properties;
}

const std::map<std::string, std::unique_ptr<Uniform>>& Material::getUniforms() const noexcept {
    return uniforms;
}

Material::Builder Material::builder() {
    return {};
}

Material::Material(Material::Builder& builder)
    : properties {std::move(builder.properties)}
    , blending {builder._blending}
    , shading {builder._shading}
    , two_sided {builder._two_sided}
    , refraction {builder._refraction}
    , name {builder._name}
    , shader_index {builder.shader_index}
    , model_shaders {builder._model_shaders}
    , uniforms {std::move(builder.uniforms)}
    , vertex_snippet {builder.vertex_snippet}
    , fragment_snippet {builder.fragment_snippet}
    , global_snippet {builder.global_snippet}
    , shading_snippet {builder.shading_snippet}
    , buffer {*this} {
}