#include <limitless/pipeline/forward/color_pass.hpp>

#include <limitless/core/context.hpp>
#include <limitless/pipeline/shader_type.hpp>
#include <limitless/ms/blending.hpp>
#include <limitless/util/sorter.hpp>
#include <limitless/instances/abstract_instance.hpp>
#include <stdexcept>

using namespace Limitless;

ColorPass::ColorPass(Pipeline& pipeline, ms::Blending _blending)
        : PipelinePass(pipeline)
        , blending {_blending} {
}

void ColorPass::draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) {
    switch (blending) {
        case ms::Blending::Opaque:
            std::sort(instances.begin(), instances.end(), FrontToBackSorter{camera});
            break;
        case ms::Blending::Translucent:
        case ms::Blending::Additive:
        case ms::Blending::Modulate:
            std::sort(instances.begin(), instances.end(), BackToFrontSorter{camera});
            break;
        case ms::Blending::MultipleOpaque:
        case ms::Blending::Text:
            throw std::logic_error("This type of blending cannot be used as ColorPass value");
    }

    for (auto& instance : instances) {
//        ctx.setPolygonMode(CullFace::FrontBack, PolygonMode::Line);

        instance.get().draw(ctx, assets, ShaderType::Forward, blending, setter);

//        ctx.setPolygonMode(CullFace::FrontBack, PolygonMode::Fill);
    }
}
