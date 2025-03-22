#include "../functions/calculateTBN.glsl"

struct EvalContext {
    mat4 model_transform;
    vec4 world_position;
#if defined (ENGINE_MATERIAL_NORMAL_TEXTURE) && defined (ENGINE_SETTINGS_NORMAL_MAPPING)
    mat3 TBN;
#endif
};

EvalContext computeEvalContext(const VertexContext vctx, const InstanceContext ictx) {
    EvalContext ectx;

    ectx.model_transform = getModelTransform(ictx, vctx);
    ectx.world_position = ectx.model_transform * vec4(vctx.position, 1.0);

    return ectx;
}

void ProcessEvalContext(const VertexContext vctx, const InstanceContext ictx, inout EvalContext ectx) {
#if defined (ENGINE_MATERIAL_NORMAL_TEXTURE) && defined (ENGINE_SETTINGS_NORMAL_MAPPING)
    //TODO: pass through uniform instance buffer ? bone transform ?
    //TODO: research on transpose inverse bone scaling
    mat3 normal_matrix = transpose(inverse(mat3(ectx.model_transform)));
    ectx.TBN = calculateTBN(vctx.normal, vctx.tangent, normal_matrix);
#endif

    gl_Position = getViewProjection() * ectx.world_position;
}

VertexContext computeVertexContext() {
    VertexContext vctx;

    ENGINE_ASSIGN_STREAM_ATTRIBUTES

    return vctx;
}

void CustomVertexContext(inout VertexContext vctx, inout InstanceContext ictx, inout EvalContext ectx) {
    ENGINE_MATERIAL_VERTEX_SNIPPET
}

void VertexPassThrough(const VertexContext vctx, const InstanceContext ictx, const EvalContext ectx) {
    ENGINE_VERTEX_PASS_THROUGH
}
