#include <limitless/models/model.hpp>
#include <limitless/models/model_builder.h>

using namespace Limitless;

Model::Model(
    const std::string& name,
    const std::vector<std::shared_ptr<Mesh>>& meshes,
    const std::vector<std::shared_ptr<ms::Material>>& materials,
    LodTransition transition,
    LodSelection selection,
    const std::vector<float>& distances,
    float lod_fade_transition_width,
    std::shared_ptr<const ModelMaterialVariantSet> material_variants,
    std::shared_ptr<const BillboardLodBundle> billboard_bundle
)
    : name {name}
    , lods {{meshes, materials}}
    , transition {transition}
    , selection {selection}
    , distances {distances}
    , lod_fade_transition_width {lod_fade_transition_width}
    , material_variant_set_ {std::move(material_variants)}
    , billboard_lod_bundle_ {std::move(billboard_bundle)} {
    calculateBoundingBox();
}

Model::Model(
    const std::string& name,
    const std::vector<Lod>& lods,
    LodTransition transition,
    LodSelection selection,
    const std::vector<float>& distances,
    float lod_fade_transition_width,
    std::shared_ptr<const ModelMaterialVariantSet> material_variants,
    std::shared_ptr<const BillboardLodBundle> billboard_bundle
)
    : name {name}
    , lods {lods}
    , transition {transition}
    , selection {selection}
    , distances {distances}
    , lod_fade_transition_width {lod_fade_transition_width}
    , material_variant_set_ {std::move(material_variants)}
    , billboard_lod_bundle_ {std::move(billboard_bundle)} {
    calculateBoundingBox();
}

void Model::calculateBoundingBox() {
    bounding_box = lods[0].meshes[0]->getBoundingBox();
    for (uint32_t i = 1; i < lods[0].meshes.size(); ++i) {
        bounding_box = mergeBoundingBox(bounding_box, lods[0].meshes[i]->getBoundingBox());
    }
}

Model::Builder Model::builder()
{
    return {};
}
