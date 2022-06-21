#include <limitless/instances/material_instance.hpp>

#include <limitless/core/context.hpp>
#include <limitless/ms/material.hpp>
#include <stdexcept>
#include <algorithm>

using namespace Limitless;
using namespace Limitless::ms;

MaterialInstance::MaterialInstance(const std::shared_ptr<Material>& material)
    : base {std::make_shared<Material>(*material)} {
    materials.emplace(next_id++, base);
}

MaterialInstance::MaterialInstance(const MaterialInstance& instance)
    : next_id {instance.next_id}
    , base {std::make_shared<Material>(*instance.base)} {
    // deep map copy
    for (const auto& [id, material] : instance.materials) {
        materials.emplace(id, std::make_shared<Material>(*material));
    }
}

void MaterialInstance::changeBaseMaterial(const std::shared_ptr<ms::Material>& material) noexcept {
    base = std::make_shared<Material>(*material);
    changeMaterial(base);
}

void MaterialInstance::changeMaterial(const std::shared_ptr<Material>& material) noexcept {
    materials[0] = std::make_shared<Material>(*material);
}

void MaterialInstance::reset() noexcept {
    materials[0] = base;
}

void MaterialInstance::clear() noexcept {
    materials.erase(++materials.begin(), materials.end());
}

uint64_t MaterialInstance::apply(const std::shared_ptr<Material>& material) noexcept {
    materials.emplace(next_id, std::make_shared<Material>(*material));
    return next_id++;
}

void MaterialInstance::remove(uint64_t id) {
    if (id == 0) {
        throw std::runtime_error("Attempt to remove base material layer.");
    }

    materials.erase(id);
}

void MaterialInstance::setMaterialState(Context& ctx, uint64_t id, ShaderPass pass) {
    const auto& material = materials.at(id);

    // checks for multiple opaque materials
    // that should be blended as sum color/N
    if (material->getBlending() == Blending::Opaque) {
        const auto opaque_count = std::count_if(begin(), end(), [] (const auto& pair) {
            return pair.second->getBlending() == Blending::Opaque;
        });

        if (opaque_count == 1) {
            setBlendingMode(ctx, Blending::Opaque);
        } else {
            setBlendingMode(ctx, Blending::MultipleOpaque, opaque_count);
        }
    } else {

        if (material->getRefraction()) {
            setBlendingMode(ctx, Blending::Opaque);
        } else {
            setBlendingMode(ctx, material->getBlending());
        }
    }

    // sets culling based on two-sideness
    if (material->getTwoSided()) {
        ctx.disable(Capabilities::CullFace);
    } else {
        ctx.enable(Capabilities::CullFace);

        // front cullfacing for shadows helps prevent peter panning
        if (pass == ShaderPass::DirectionalShadow) {
            ctx.setCullFace(CullFace::Front);
        } else {
            ctx.setCullFace(CullFace::Back);
        }
    }
}

void MaterialInstance::makeLayered() noexcept {
	layered = true;
}

void MaterialInstance::makeNonLayered() noexcept {
	layered = false;
}

bool MaterialInstance::isLayered() const noexcept {
	return layered;
}
