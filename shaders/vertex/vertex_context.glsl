#include "../functions/calculateTBN.glsl"
#include "../functions/billboard.glsl"
#include "../functions/wind.glsl"

#if defined(ENGINE_PASS_DIRECTIONAL_SHADOW)
// For DirectionalShadow pass, gl_Position is computed in light clip space.
// Must be declared at global scope (GLSL requirement).
uniform mat4 light_space;
#endif

struct EvalContext {
    mat4 model_transform;
    vec4 world_position;
#if defined (ENGINE_MATERIAL_NORMAL_TEXTURE) && defined (ENGINE_SETTINGS_NORMAL_MAPPING)
    mat3 TBN;
#endif
#if defined (ENGINE_MATERIAL_WIND_MODE)
    mat3 wind_rot;
#endif
};

EvalContext computeEvalContext(const VertexContext vctx, const InstanceContext ictx) {
    EvalContext ectx;

    ectx.model_transform = getModelTransform(ictx, vctx);
#if defined (ENGINE_MATERIAL_WIND_MODE)
    ectx.wind_rot = mat3(1.0);
#endif

#if defined (ENGINE_MATERIAL_BILLBOARD_MODE) && !defined (ENGINE_MATERIAL_SKELETAL_MODEL) && !defined (ENGINE_MATERIAL_SKELETAL_INSTANCED_MODEL)
    uint _bb_mode = getMaterialBillboardMode();
    if (_bb_mode != 0u) {
        vec3 pivot_local = vec3(0.0);
#if defined (ENGINE_MATERIAL_BILLBOARD_PIVOT)
        pivot_local = getMaterialBillboardPivot();
#endif

        // Keep billboard pivot stable and mesh-independent (do not use bone transform here)
        vec3 pivot_world = (ictx.model_matrix * vec4(pivot_local, 1.0)).xyz;

        vec3 axis_world = vec3(0.0, 1.0, 0.0);
#if defined (ENGINE_MATERIAL_BILLBOARD_AXIS)
        axis_world = getMaterialBillboardAxis();
#endif

        mat3 basis = compute_billboard_basis(_bb_mode, axis_world, pivot_world);

        vec3 s = extract_scale(ictx.model_matrix);
        mat3 S = mat3(
            vec3(s.x, 0.0, 0.0),
            vec3(0.0, s.y, 0.0),
            vec3(0.0, 0.0, s.z)
        );
        mat3 RS = basis * S;

        // Translation so that pivot_local maps to pivot_world
        vec3 t = pivot_world - RS * pivot_local;

        ectx.model_transform = mat4(
            vec4(RS[0], 0.0),
            vec4(RS[1], 0.0),
            vec4(RS[2], 0.0),
            vec4(t, 1.0)
        );
    }
#endif
    vec3 local_pos = vctx.position;

#if defined (ENGINE_MATERIAL_WIND_MODE) && !defined (ENGINE_MATERIAL_SKELETAL_MODEL) && !defined (ENGINE_MATERIAL_SKELETAL_INSTANCED_MODEL)
    if (getMaterialWindMode() != 0u) {
        // World instance position from object matrix translation
        vec3 world_instance_pos = vec3(ictx.model_matrix[3].x, ictx.model_matrix[3].y, ictx.model_matrix[3].z);

        uint wind_mode = getMaterialWindMode();
        if (wind_mode == 1u) {
            float intensity = 1.0;
        #if defined (ENGINE_MATERIAL_WIND_INTENSITY)
            intensity = getMaterialWindIntensity();
        #endif
            float frequency = 1.0;
        #if defined (ENGINE_MATERIAL_WIND_FREQUENCY)
            frequency = getMaterialWindFrequency();
        #endif

            // Wind direction in object space
            vec3 wind_dir_world = getWindDirection();
            vec3 wind_dir_obj = normalize(inverse(mat3(ictx.model_matrix)) * wind_dir_world);

            local_pos = apply_simple_wind(local_pos, world_instance_pos, wind_dir_obj, intensity, frequency, ectx.wind_rot);
        } else if (wind_mode == 2u) {
            // SpeedTree8-style wind: uses extra UV sets as payload.
            mat3 object_to_world = mat3(ictx.model_matrix);
            vec2 uv1 = getVertexUv1();
            vec2 uv2 = getVertexUv2();
            vec2 uv3 = getVertexUv3();
            vec2 uv4 = getVertexUv4();
            vec2 uv5 = getVertexUv5();

            // Leaf wind updates normals via rotation; we model that as a per-vertex rotation matrix.
            vec3 n = vctx.normal;
            vec3 t = vctx.tangent.xyz;
            local_pos = apply_speedtree8_wind(
                local_pos,
                world_instance_pos,
                object_to_world,
                uv1, uv2, uv3, uv4, uv5,
                n, t,
                ectx.wind_rot
            );
        }
    }
#endif

    ectx.world_position = ectx.model_transform * vec4(local_pos, 1.0);

    return ectx;
}

void ProcessEvalContext(const VertexContext vctx, const InstanceContext ictx, inout EvalContext ectx) {
#if defined (ENGINE_MATERIAL_NORMAL_TEXTURE) && defined (ENGINE_SETTINGS_NORMAL_MAPPING)
    //TODO: pass through uniform instance buffer ? bone transform ?
    //TODO: research on transpose inverse bone scaling
    mat3 normal_matrix = transpose(inverse(mat3(ectx.model_transform)));

#if defined (ENGINE_MATERIAL_WIND_MODE) && !defined (ENGINE_MATERIAL_SKELETAL_MODEL) && !defined (ENGINE_MATERIAL_SKELETAL_INSTANCED_MODEL)
    ectx.TBN = calculateTBN(ectx.wind_rot * vctx.normal, vec4(ectx.wind_rot * vctx.tangent.xyz, vctx.tangent.w), normal_matrix);
#else
    ectx.TBN = calculateTBN(vctx.normal, vec4(vctx.tangent.xyz, vctx.tangent.w), normal_matrix);
#endif
#endif

    // Most passes render from the camera VP stored in `scene_data`.
    // Directional shadow maps must render from the light, using the per-cascade `light_space` matrix.
#if defined(ENGINE_PASS_DIRECTIONAL_SHADOW)
    gl_Position = light_space * ectx.world_position;
#else
    gl_Position = getViewProjection() * ectx.world_position;
#endif
}

VertexContext computeVertexContext() {
    VertexContext vctx;

    ENGINE_ASSIGN_STREAM_ATTRIBUTES

    return vctx;
}

ENGINE_MATERIAL_GLOBAL_VERTEX_DEFINITIONS

void CustomVertexContext(inout VertexContext vctx, inout InstanceContext ictx, inout EvalContext ectx) {
    ENGINE_MATERIAL_VERTEX_SNIPPET
}

void VertexPassThrough(const VertexContext vctx, const InstanceContext ictx, const EvalContext ectx) {
    ENGINE_VERTEX_PASS_THROUGH
}
