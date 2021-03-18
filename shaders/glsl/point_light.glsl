struct PointLight {
    vec4 position;
    vec4 color;
    float constant;
    float linear;
    float quadratic;
    float radius;
};

layout(std140) buffer point_light_buffer {
    PointLight point_lights[];
};

vec3 computePointLight(PointLight light, vec3 normal, vec3 surface_color, vec3 view_dir, float specular, float shininess) {
    vec3 light_dir = normalize(light.position.xyz - fs_data.world_position);
    float distance = length(light.position.xyz - fs_data.world_position);

    float diffuse_factor = max(dot(normal, light_dir), 0.0);
    vec3 diffuse_light = light.color.rgb * light.color.a * diffuse_factor * surface_color;

    #ifdef PHONG_MODEL
        vec3 reflect_dir = reflect(-light_dir, normal);
        float specular_factor = pow(max(dot(view_dir, reflect_dir), 0.0), shininess) * specular;
        vec3 specular_light = light.color.xyz * specular_factor;
    #endif

    #ifdef BLINN_PHONG_MODEL
        vec3 halfway_dir = normalize(light_dir + view_dir);
        float specular_factor = pow(max(dot(normal, halfway_dir), 0.0), shininess) * specular;
        vec3 specular_light = light.color.xyz * specular_factor;
    #endif

    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    return diffuse_light * attenuation + specular_light * attenuation;
}