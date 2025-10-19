ENGINE::COMMON

in vec2 uv;

#include "./scene.glsl"
#include "../functions/linearize_depth.glsl"
#include "../functions/reconstruct_position.glsl"

layout (location = 0) out vec3 color;

uniform sampler2D lightened;
uniform sampler2D depth_texture;

uniform vec3 fog_color;
uniform float fog_density; // for exponential fog
uniform float fog_start;   // for linear fog start distance
uniform float fog_end;     // for linear fog end distance
uniform float fog_height_start; // altitude where attenuation begins
uniform float fog_height_end;   // altitude where fog becomes 0
uniform int fog_enabled;

float computeLinearFogFactor(float linearDepth) {
	float f = clamp((linearDepth - fog_start) / max(0.0001, (fog_end - fog_start)), 0.0, 1.0);
	return f;
}

float computeExponentialFogFactor(float linearDepth) {
	float f = 1.0 - exp(-linearDepth * fog_density);
	return clamp(f, 0.0, 1.0);
}

void main() {
	vec3 scene_color = texture(lightened, uv).rgb;
	if (fog_enabled == 0) {
		color = scene_color;
		return;
	}

    float depth = texture(depth_texture, uv).r;
    float linearDepth = linearize_depth(depth, getCameraNearPlane(), getCameraFarPlane());

    // Reconstruct world position
    vec3 worldPos = reconstructPosition(uv, depth);
    float altitude = worldPos.y; // assuming Y-up

    // Height attenuation: at fog_height_start => 1.0, at fog_height_end => 0.0
    float heightAtten = 1.0 - clamp((altitude - fog_height_start) / max(0.0001, (fog_height_end - fog_height_start)), 0.0, 1.0);

	// Mix linear and exponential for flexibility; here use linear by default
    float fogFactor = computeLinearFogFactor(linearDepth) * heightAtten;

	color = mix(scene_color, fog_color, fogFactor);
}


