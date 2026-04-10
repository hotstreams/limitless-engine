#if defined (ENGINE_SETTINGS_CSM)
    #include "./scene_lighting.glsl"

    layout (std140) buffer directional_shadows {
        mat4 _dir_light_space[];
    };

    // Filament-style shadow sampling types
    #define SHADOW_SAMPLING_PCF_HARD    0
    #define SHADOW_SAMPLING_PCF_LOW     1
    #define SHADOW_SAMPLING_PCF_MEDIUM  2

    // Filament-style: use hardware depth compare (requires GL_TEXTURE_COMPARE_MODE=COMPARE_REF_TO_TEXTURE).
    uniform sampler2DArrayShadow _dir_shadows;
    // Legacy/debug: NDC depth split boundaries in [0..1] (used by overlay/debug).
    uniform vec4 _far_bounds;
    // Filament-style: view-space Z split planes (linear) for cascade selection.
    uniform vec4 _cascade_splits_viewz;
    // Per-cascade normal bias in world units (scaled from world-space texel size).
    uniform vec4 _normal_bias;
    // Filament-style clamp-to-edge coords in normalized UV space (xy=min, zw=max).
    uniform vec4 _dir_scissor[ENGINE_SETTINGS_CSM_SPLIT_COUNT];
    // Receiver plane bias settings.
    uniform int _csm_receiver_plane_bias_enabled;
    uniform float _csm_receiver_plane_bias_scale;
    // Constant bias in world units applied along light direction.
    uniform float _csm_constant_bias;
    // Filament-style cascade info: lower 4 bits = cascade count, bits 8..11 = per-cascade visibility flags.
    uniform int _csm_cascade_info;
    // Shadow far attenuation parameters: x = scale, y = 1/(far^2).
    uniform vec2 _csm_far_attenuation;
    // Shadow sampling type: 0 = PCF_HARD, 1 = PCF_LOW, 2 = PCF_MEDIUM (3x3 with RPB)
    uniform int _csm_sampling_type;

    //------------------------------------------------------------------------------
    // Helper: sample depth with UV clamping
    //------------------------------------------------------------------------------
    float sampleShadowDepth(const highp vec4 scissor, const uint layer, 
                            highp vec2 uv, highp float depth) {
        // Clamp UV to scissor region (handles border texels)
        uv = clamp(uv, scissor.xy, scissor.zw);
        // Depth must be clamped to [0,1] for floating-point depth formats
        return texture(_dir_shadows, vec4(uv, float(layer), saturate(depth)));
    }

    //------------------------------------------------------------------------------
    // PCF_HARD: Single sample with hardware comparison (fastest)
    //------------------------------------------------------------------------------
    float shadowSample_PCF_Hard(const highp vec4 scissor, const uint layer, 
                                const highp vec3 position) {
        return sampleShadowDepth(scissor, layer, position.xy, position.z);
    }

    //------------------------------------------------------------------------------
    // PCF_LOW: 4-sample bilinear-weighted PCF (Castaño 2013)
    // Optimized 3x3 gaussian approximation using hardware bilinear filtering
    //------------------------------------------------------------------------------
    float shadowSample_PCF_Low(const highp vec4 scissor, const uint layer, 
                               const highp vec3 position) {
        highp vec2 size = vec2(textureSize(_dir_shadows, 0));
        highp vec2 texelSize = vec2(1.0) / size;
        highp float depth = position.z;

        // Clamp position to avoid GPU overflow issues
        highp vec2 pos = clamp(position.xy, vec2(-1.0), vec2(2.0));

        // Castaño, 2013, "Shadow Mapping Summary Part 1"
        vec2 offset = vec2(0.5);
        highp vec2 uv = (pos * size) + offset;
        highp vec2 base = (floor(uv) - offset) * texelSize;
        highp vec2 st = fract(uv);

        // Compute bilinear weights for 3x3 gaussian kernel
        vec2 uw = vec2(3.0 - 2.0 * st.x, 1.0 + 2.0 * st.x);
        vec2 vw = vec2(3.0 - 2.0 * st.y, 1.0 + 2.0 * st.y);

        // Compute sample positions
        highp vec2 u = vec2((2.0 - st.x) / uw.x - 1.0, st.x / uw.y + 1.0);
        highp vec2 v = vec2((2.0 - st.y) / vw.x - 1.0, st.y / vw.y + 1.0);

        u *= texelSize.x;
        v *= texelSize.y;

        // 4 samples with bilinear weights (equivalent to 9-sample gaussian)
        float sum = 0.0;
        sum += uw.x * vw.x * sampleShadowDepth(scissor, layer, base + vec2(u.x, v.x), depth);
        sum += uw.y * vw.x * sampleShadowDepth(scissor, layer, base + vec2(u.y, v.x), depth);
        sum += uw.x * vw.y * sampleShadowDepth(scissor, layer, base + vec2(u.x, v.y), depth);
        sum += uw.y * vw.y * sampleShadowDepth(scissor, layer, base + vec2(u.y, v.y), depth);

        return sum * (1.0 / 16.0);
    }

    //------------------------------------------------------------------------------
    // PCF_MEDIUM: 9-sample PCF with receiver-plane depth bias
    //------------------------------------------------------------------------------
    float shadowSample_PCF_Medium(const highp vec4 scissor, const uint layer,
                                  const highp vec3 position) {
        // Receiver-plane depth bias using screen-space derivatives
        highp vec3 duvz_dx = dFdx(position);
        highp vec3 duvz_dy = dFdy(position);
        highp vec2 dz_duv = inverse(transpose(mat2(duvz_dx.xy, duvz_dy.xy))) 
                          * vec2(duvz_dx.z, duvz_dy.z);

        vec2 texelSize = 1.0 / vec2(textureSize(_dir_shadows, 0));
        float sum = 0.0;

        for (int x = -1; x <= 1; ++x) {
            for (int y = -1; y <= 1; ++y) {
                vec2 duv = vec2(x, y) * texelSize;
                vec2 tc = clamp(position.xy + duv, scissor.xy, scissor.zw);
                float depth = position.z;
                if (_csm_receiver_plane_bias_enabled != 0) {
                    depth = position.z - dot(dz_duv, duv) * _csm_receiver_plane_bias_scale;
                }
                sum += texture(_dir_shadows, vec4(tc, float(layer), saturate(depth)));
            }
        }

        return sum / 9.0;
    }

    //------------------------------------------------------------------------------
    // Cascade selection (Filament-style vectorized)
    //------------------------------------------------------------------------------
    int getShadowCascade(vec3 world_pos) {
        highp float z = (getView() * vec4(world_pos, 1.0)).z;
        ivec4 greaterZ = ivec4(greaterThan(_cascade_splits_viewz, vec4(z)));
        int cascadeCount = _csm_cascade_info & 0xF;
        return clamp(greaterZ.x + greaterZ.y + greaterZ.z + greaterZ.w, 0, cascadeCount - 1);
    }

    //------------------------------------------------------------------------------
    // Main directional shadow function
    //------------------------------------------------------------------------------
    float getDirectionalShadow(vec3 normal, vec3 world_pos) {
        Light light = getDirectionalLight();

        int cascade = getShadowCascade(world_pos);

        // Check if this cascade has valid shadow data
        bool cascadeHasVisibleShadows = bool(_csm_cascade_info & (1 << (cascade + 8)));
        if (!cascadeHasVisibleShadows) {
            return 0.0; // No shadow data, fully lit
        }

        mat4 light_space = _dir_light_space[cascade];

        // Normal bias: offset along normal based on angle to light
        vec3 geoN = normalize(normal);
        highp float cosTheta = saturate(dot(geoN, -light.direction.xyz));
        highp float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
        float b = _normal_bias[cascade];
        world_pos += geoN * (sinTheta * b);

        // Constant bias along light direction
        world_pos += (-light.direction.xyz) * _csm_constant_bias;

        // Transform to shadow texture coordinates
        highp vec4 shadowPosition = light_space * vec4(world_pos, 1.0);
        highp vec3 position = shadowPosition.xyz * (1.0 / shadowPosition.w);

        // Clamp UV to scissor region
        vec4 sc = _dir_scissor[cascade];
        position.xy = clamp(position.xy, sc.xy, sc.zw);

        // Sample shadow based on selected algorithm
        float visibility = 1.0;

        #if defined(ENGINE_SETTINGS_PCF)
            // Runtime selection of PCF quality
            if (_csm_sampling_type == SHADOW_SAMPLING_PCF_HARD) {
                visibility = shadowSample_PCF_Hard(sc, uint(cascade), position);
            } else if (_csm_sampling_type == SHADOW_SAMPLING_PCF_LOW) {
                visibility = shadowSample_PCF_Low(sc, uint(cascade), position);
            } else {
                visibility = shadowSample_PCF_Medium(sc, uint(cascade), position);
            }
        #else
            // No PCF - use hard shadows
            visibility = shadowSample_PCF_Hard(sc, uint(cascade), position);
        #endif

        // Shadow far attenuation
        if (_csm_far_attenuation.y > 0.0) {
            highp vec3 v = world_pos - getCameraPosition();
            highp float z = dot(transpose(getView())[2].xyz, v);
            highp vec2 p = _csm_far_attenuation;
            visibility = 1.0 - ((1.0 - visibility) * saturate(p.x - z * z * p.y));
        }

        return 1.0 - visibility;
    }
#endif
