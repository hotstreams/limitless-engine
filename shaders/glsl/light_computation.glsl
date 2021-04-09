#include "../glsl/point_light.glsl"
#include "../glsl/directional_light.glsl"

#if defined(PBR)
    #include "../glsl/pbr.glsl"

    vec3 getPBRShadedColor(vec3 N, vec3 fragment_color, float metallic, float roughness) {
        vec3 ambient = ambient_color.xyz * ambient_color.w * fragment_color;

        vec3 V = normalize(camera_position.xyz - in_data.world_position);

        vec3 F0 = mix(vec3(0.04), fragment_color, metallic);

        vec3 Lo = vec3(0.0);
        for (uint i = uint(0); i < point_lights_count; ++i) {
            vec3 L = normalize(point_lights[i].position.xyz - in_data.world_position);
            vec3 H = normalize(V + L);

            float distance = length(point_lights[i].position.xyz - in_data.world_position);
            float attenuation = 1.0 / (point_lights[i].constant + point_lights[i].linear * distance + point_lights[i].quadratic * distance * distance);
            vec3 radiance = point_lights[i].color.xyz * attenuation;

            float NDF = DistributionGGX(N, H, roughness);
            float G   = GeometrySmith(N, V, L, roughness);
            vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

            vec3 nominator    = NDF * G * F;
            float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
            vec3 specular = nominator / max(denominator, 0.001);

            vec3 kS = F;
            vec3 kD = vec3(1.0) - kS;
            kD *= 1.0 - metallic;

            float NdotL = max(dot(N, L), 0.0);

            Lo += (kD * fragment_color / PI + specular) * radiance * NdotL;
        }

        // TODO: PBR directional lights

        return ambient + Lo;
    }
#else
    vec3 getShadedColor(vec3 fragment_color, vec3 normal, float specular, float shininess) {
        vec3 ambient = ambient_color.xyz * ambient_color.w * fragment_color;
        vec3 view_dir = normalize(camera_position.xyz - in_data.world_position);

        vec3 light = vec3(0.0);

        // computing point lights
        for (uint i = uint(0); i < point_lights_count; ++i) {
            PointLight point_light = point_lights[i];
            float distance = length(point_light.position.xyz - in_data.world_position);

            if (distance <= point_light.radius) {
                light += computePointLight(point_light, normal, fragment_color, view_dir, specular, shininess);
            }
        }

        if (dir_lights_count != uint(0)) {
            #if defined(DIRECTIONAL_CSM)
                float shadow = computeDirectionalShadow(dir_light, normal);
                light += (1.0 - shadow) * computeDirLight(dir_light, normal, fragment_color, view_dir, specular, shininess);
            #else
                light += computeDirLight(dir_light, normal, fragment_color, view_dir, specular, shininess);
            #endif
        }

        return ambient + light;
    }
#endif
