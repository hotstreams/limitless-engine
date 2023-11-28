#if defined (ENGINE_SETTINGS_CSM)
    #include "./scene_lighting.glsl"

    layout (std140) buffer directional_shadows {
        mat4 _dir_light_space[];
    };

    uniform sampler2DArray _dir_shadows;
    uniform vec4 _far_bounds;

    int getShadowFrustumIndex(vec3 position) {
        vec4 p = getViewProjection() * vec4(position, 1.0);
        p.xyz /= p.w;

        float z = p.z * 0.5 + 0.5;

        int index = ENGINE_SETTINGS_CSM_SPLIT_COUNT;
        for (int i = 0; i < ENGINE_SETTINGS_CSM_SPLIT_COUNT; ++i) {
            if (z < _far_bounds[i]) {
                index = i;
                break;
            }
        }

        return index;
    }

    float getDirectionalShadow(vec3 normal, vec3 world_pos) {
        Light light = getDirectionalLight();

        int index = getShadowFrustumIndex(world_pos);

        mat4 light_space = _dir_light_space[index];

        // transforming from global space to ndc light space
        vec4 light_pos_space = light_space * vec4(world_pos, 1.0);
//        vec3 ndc = light_pos_space.xyz / light_pos_space.w; ?????????/
        vec3 ndc = light_pos_space.xyz;
        ndc = ndc * 0.5 + 0.5;

        if (ndc.z > 1.0) {
            return 0.0;
        }

        float closestDepth = texture(_dir_shadows, vec3(ndc.xy, index)).r;
        float currentDepth = ndc.z;

        float shadow = 0.0;
        const float MIN_BIAS = 0.0005;
        float bias = max(0.05 * (1.0 - dot(normal, vec3(-light.direction.xyz))), MIN_BIAS);

        #if defined (ENGINE_SETTINGS_PCF)
            vec2 texelSize = 1.0 / textureSize(_dir_shadows, 0).xy;
            for (int x = -1; x <= 1; ++x) {
                for (int y = -1; y <= 1; ++y) {
                    float pcfDepth = texture(_dir_shadows, vec3(ndc.xy + vec2(x, y) * texelSize, index)).r;
                    shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
                }
            }
            shadow /= 9.0;
        #else
            shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
        #endif

        return shadow;
    }
#endif
