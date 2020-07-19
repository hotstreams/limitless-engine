#include "glsl/point_light.glsl"

#ifdef PBR
    #include "glsl/pbr.glsl"

    vec3 getPBRShadedColor(vec3 N, vec3 fragment_color, float metallic, float roughness) {
        vec3 ambient = ambient_color.xyz * ambient_color.w * fragment_color;

        vec3 V = normalize(camera_position.xyz, fs_data.world_position);

        vec3 F0 = mix(vec3(0.04), fragment_color, metallic);

        vec3 Lo = vec3(0.0);
        for (int i = 0; i < point_lights_count; ++i) {
            vec3 L = normalize(point_lights[i].position - fs_data.world_position);
            vec3 H = normalize(V + L);

            float distance = length(point_lights[i].position.xyz - fs_data.world_position);
            float attenuation = 1.0 / (point_lights[i].constant + point_lights[i].linear * distance + point_lights[i].quadratic * distance * distance);
            vec3 radiance = point_lights[i].color * attenuation;

            float NDF = DistributionGGX(N, H, roughness.y);
            float G   = GeometrySmith(N, V, L, roughness.y);
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

        return ambient + Lo;
    }
#else
    vec3 getShadedColor(vec3 fragment_color, vec3 normal, float specular, float shininess) {
        vec3 ambient = ambient_color.xyz * ambient_color.w * fragment_color;

        vec3 view_dir = normalize(camera_position.xyz, fs_data.world_position);

        vec3 light = vec3(0.0);
        for (int i = 0; i < point_lights_count; ++i) {
            PointLight light = point_lights[i];
            float distance = length(light.position.xyz - fs_data.world_position);

            if (distance <= light.radius) {
                light += computePointLight(light, normal, fragment_color, view_dir, specular, shininess);
            }
        }

        return ambient + light;
    }
#endif
