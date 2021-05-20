#if defined(DIRECTIONAL_CSM)
    layout(std140) buffer directional_shadows {
        mat4 dir_light_space[];
    };

    uniform sampler2DArray dir_shadows;
    uniform vec4 far_bounds;

    float computeDirectionalShadow(DirLight light, vec3 normal) {
        // determining in which layer we will look into based on far bounds of frustums
        int index = DIRECTIONAL_SPLIT_COUNT;
        for (int i = 0; i < DIRECTIONAL_SPLIT_COUNT; ++i) {
            if (gl_FragCoord.z < far_bounds[i]) {
                index = i;
                break;
            }
        }

        mat4 light_space = dir_light_space[index];

        // transforming from global space to ndc light space
        vec4 light_pos_space = light_space * vec4(in_data.world_position, 1.0);
        vec3 ndc = light_pos_space.xyz / light_pos_space.w;
        ndc = ndc * 0.5 + 0.5;

        float closestDepth = texture(dir_shadows, vec3(ndc.xy, index)).r;
        float currentDepth = ndc.z;

        float shadow = 0.0;
        const float MIN_BIAS = 0.005;
        float bias = max(0.05 * (1.0 - dot(normal, vec3(-light.direction.xyz))), MIN_BIAS);
        #if defined(DIRECTIONAL_PFC)
            vec2 texelSize = 1.0 / textureSize(dir_shadows, 0).xy;
            for (int x = -1; x <= 1; ++x) {
                for (int y = -1; y <= 1; ++y) {
                    float pcfDepth = texture(dir_shadows, vec3(ndc.xy + vec2(x, y) * texelSize, index)).r;
                    shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
                }
            }
            shadow /= 9.0;
        #else
            shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
        #endif

        if (ndc.z > 1.0) {
            shadow = 0.0;
        }

        return shadow;
    }
#endif

vec3 computeDirLight(DirLight light, vec3 normal, vec3 surface_color, vec3 view_dir, float specular, float shininess) {
    vec3 light_dir = normalize(-light.direction.xyz);

    // diffuse lighting
    float diffuse_factor = max(dot(normal, light_dir), 0.0);
    vec3 diffuse_light = light.color.rgb * light.color.a * diffuse_factor * surface_color;

    // specular lighting
    #if defined(PHONG_MODEL)
        vec3 reflect_dir = reflect(-light_dir, normal);
        float specular_factor = pow(max(dot(view_dir, reflect_dir), 0.0), shininess) * specular;
        vec3 specular_light = light.color.xyz * specular_factor;
    #endif

    #if defined(BLINN_PHONG_MODEL)
        vec3 halfway_dir = normalize(light_dir + view_dir);
        float specular_factor = pow(max(dot(normal, halfway_dir), 0.0), shininess) * specular;
        vec3 specular_light = light.color.xyz * specular_factor;
    #endif

    return diffuse_light + specular_light;
}
