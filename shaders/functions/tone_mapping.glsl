mat3 ACESInputMat = mat3(
	0.59719, 0.07600, 0.02840,
	0.35458, 0.90834, 0.13383,
	0.04823, 0.01566, 0.83777);

mat3 ACESOutputMat = mat3(
	 1.60475, -0.10208, -0.00327,
	-0.53108,  1.10813, -0.07276,
	-0.07367, -0.00605,  1.07602);

vec3 RRTAndODTFit(vec3 v) {
	vec3 a = v * (v + 0.0245786) - 0.000090537;
	vec3 b = v * (0.983729 * v + 0.4329510) + 0.238081;
	return a / b;
}

vec3 acesFitted(vec3 color) {
	color = ACESInputMat * color;
	color = RRTAndODTFit(color);
	color = ACESOutputMat * color;
	return clamp(color, 0.0, 1.0);
}

vec3 acesMapping(vec3 hdrColor, float exposure) {
    vec3 mapped = acesFitted(hdrColor * exposure);
    return pow(mapped, vec3(1.0 / 2.2));
}

vec3 toneMapping(vec3 color, float exposure) {
    return vec3(1.0) - exp(-color * exposure);
}
