/*
 *  Screen space ray tracing
 */

void swap(inout float a, inout float b) {
    float temp = a;
    a = b;
    b = temp;
}

float distance_squared(vec2 a, vec2 b) {
    a -= b;
    return dot(a, a);
}

bool traceScreenSpaceRay(
    vec3        vs_origin,              // view-space ray origin with negative z-value
    vec3        vs_direction,           // view-space ray direction
    mat4        view_to_screen,   // matrix that maps to screen space coordinates (NOT ndc)
    sampler2D   depth_texture,          // depth texture with normalized [0, 1] values
    vec2        texture_resolution,     // depth texture resolution
    float       vs_thickness,           // view-space pixel thickness
    float       near_plane,             // camera near plane; positive
    float       stride,                 // step in horizontal or vertical pixels between samples >= 1.0
    float       jitter_fraction,        // to conceal banding artifacts [0, 1]
    float       max_steps,              // maximum number of iterations
    float       vs_max_ray_distance,    // maximum view-space distance before miss
    float       self_count_skip,        // to avoid self-collisions, [0, 3]
    out vec2    hit_pixel,
    out vec3    vs_hit_point,
    out float   iteration_count
) {
    // Clip ray to a near plane in 3D (doesn't have to be *the* near plane, although that would be a good idea)
    float ray_length = (vs_origin.z + vs_direction.z * vs_max_ray_distance) > -near_plane ? (-near_plane - vs_origin.z) / vs_direction.z : vs_max_ray_distance;

    vec3 vs_end_point = vs_origin + vs_direction * ray_length;

    // Initialize to off screen
    hit_pixel = vec2(-1.0, -1.0);

    // Project into screen space
    vec4 H0 = view_to_screen * vec4(vs_origin, 1.0);
    vec4 H1 = view_to_screen * vec4(vs_end_point, 1.0);

    // There are a lot of divisions by w that can be turned into multiplications
    // at some minor precision loss...and we need to interpolate these 1/w values
    // anyway.
    //
    // Because the caller was required to clip to the near plane,
    // this homogeneous division (projecting from 4D to 2D) is guaranteed
    // to succeed.
    float k0 = 1.0 / H0.w;
    float k1 = 1.0 / H1.w;

    // Switch the original points to values that interpolate linearly in 2D
    vec3 Q0 = vs_origin * k0;
    vec3 Q1 = vs_end_point * k1;

    // Screen-space endpoints
    vec2 P0 = H0.xy * k0;
    vec2 P1 = H1.xy * k1;

#ifdef SCREEN_SPACE_REFLECTIONS_CLIP_TO_FRUSTRUM
    float xMax = texture_resolution.x - 0.5, xMin = 0.5, yMax = texture_resolution.y - 0.5, yMin = 0.5;
    float alpha = 0.0;

    // Assume P0 is in the viewport (P1 - P0 is never zero when clipping)
    if ((P1.y > yMax) || (P1.y < yMin)) {
        alpha = (P1.y - ((P1.y > yMax) ? yMax : yMin)) / (P1.y - P0.y);
    }

    if ((P1.x > xMax) || (P1.x < xMin)) {
        alpha = max(alpha, (P1.x - ((P1.x > xMax) ? xMax : xMin)) / (P1.x - P0.x));
    }

    P1 = mix(P1, P0, alpha); k1 = mix(k1, k0, alpha); Q1 = mix(Q1, Q0, alpha);
#endif

    // If the line is degenerate, make it cover at least one pixel
    // to avoid handling zero-pixel extent as a special case later
    P1 += vec2((distance_squared(P0, P1) < 0.0001) ? 0.01 : 0.0);

    vec2 delta = P1 - P0;

    // Permute so that the primary iteration is in x to reduce
    // large branches later
    bool permute = false;
    if (abs(delta.x) < abs(delta.y)) {
		// More-vertical line. Create a permutation that swaps x and y in the output
        permute = true;

        // Directly swizzle the inputs
        delta = delta.yx;
        P0 = P0.yx;
        P1 = P1.yx;
    }

	// From now on, "x" is the primary iteration direction and "y" is the secondary one
    float step_direction = sign(delta.x);
    float invdx = step_direction / delta.x;
    vec2 dP = vec2(step_direction, delta.y * invdx);

    // Track the derivatives of Q and k
    vec3  dQ = (Q1 - Q0) * invdx;
    float dk = (k1 - k0) * invdx;

    // Because we test 1/2 a texel forward along the ray, on the very last iteration
    // the interpolation can go past the end of the ray. Use these bounds to clamp it.
    float zMin = min(vs_end_point.z, vs_origin.z);
    float zMax = max(vs_end_point.z, vs_origin.z);

    // Scale derivatives by the desired pixel stride
    dP *= stride; dQ *= stride; dk *= stride;

    // Offset the starting values by the jitter_fraction fraction
    P0 += dP * jitter_fraction; Q0 += dQ * jitter_fraction; k0 += dk * jitter_fraction;

    // Track ray step and derivatives in a vec4 to parallelize
    vec4 pqk = vec4(P0, Q0.z, k0);
    vec4 dPQK = vec4(dP, dQ.z, dk);

	// We track the ray depth at +/- 1/2 pixel to treat pixels as clip-space solid
	// voxels. Because the depth at -1/2 for a given pixel will be the same as at
	// +1/2 for the previous iteration, we actually only have to compute one value
	// per iteration.
    float prevZMaxEstimate = vs_origin.z;
    float rayZMin = prevZMaxEstimate, rayZMax = prevZMaxEstimate;
    float scene_z_max = rayZMax + 1e4;

    // P1.x is never modified after this point, so pre-scale it by
    // the step direction for a signed comparison
    float end = P1.x * step_direction;

    bool hit = false;
    float step_count;
    for (step_count = 0.0;
         step_count <= self_count_skip ||
         ((pqk.x * step_direction) <= end &&
         step_count < max_steps &&
         !hit &&
         scene_z_max != 0.0);
         pqk += dPQK, ++step_count)
    {
        hit_pixel = permute ? pqk.yx : pqk.xy;

        // The depth range that the ray covers within this loop
        // iteration.  Assume that the ray is moving in increasing z
        // and swap if backwards.  Because one end of the interval is
        // shared between adjacent iterations, we track the previous
        // value and then swap as needed to ensure correct ordering
        rayZMin = prevZMaxEstimate;

        // Compute the value at 1/2 pixel into the future
		rayZMax = (dPQK.z * 0.5 + pqk.z) / (dPQK.w * 0.5 + pqk.w);
        rayZMax = clamp(rayZMax, zMin, zMax);
        prevZMaxEstimate = rayZMax;
        if (rayZMin > rayZMax) {
            swap(rayZMin, rayZMax);
        }

        // normalized depth [0, 1]
        scene_z_max = texelFetch(depth_texture, ivec2(hit_pixel), 0).r;
        // linear negative depth
        scene_z_max = -linearize_depth(scene_z_max, getCameraNearPlane(), getCameraFarPlane());

        hit = (rayZMax >= scene_z_max - vs_thickness) && (rayZMin <= scene_z_max);
    }

    // Undo the last increment, which ran after the test variables
    // were set up.
    pqk -= dPQK;
    step_count -= 1.0;

    if (((pqk.x + dPQK.x) * step_direction) > end || (step_count + 1.0) >= max_steps || scene_z_max == 0.0) {
        hit = false;
    }

#ifdef SCREEN_SPACE_REFLECTIONS_REFINEMENT
    if (stride > 1.0 && hit) {
        // Refine the hit point within the last large-stride step

        // Retreat one whole stride step from the previous loop so that
        // we can re-run that iteration at finer scale
        pqk -= dPQK;
        step_count -= 1.0;

        // Take the derivatives back to single-pixel stride
        float invStride = 1.0 / stride;
        dPQK *= invStride;

        // For this test, we don't bother checking vs_thickness or passing the end, since we KNOW there will
        // be a hit point. As soon as
        // the ray passes behind an object, call it a hit. Advance (stride + 1) steps to fully check this
        // interval (we could skip the very first iteration, but then we'd need identical code to prime the loop)
        float refinement_step_count = 0.0;

        // This is the current sample point's z-value, taken back to camera space
        prevZMaxEstimate = pqk.z / pqk.w;
        rayZMax = prevZMaxEstimate;

        // Ensure that the FOR-loop test passes on the first iteration since we
        // won't have a valid value of scene_z_max to test.
        scene_z_max = rayZMax + 1e7;

        for (;
            refinement_step_count <= 1.0 ||
            (refinement_step_count <= stride * 1.4) &&
            (rayZMax < scene_z_max) && (scene_z_max != 0.0);
            pqk += dPQK, refinement_step_count += 1.0) 
        {
            rayZMin = prevZMaxEstimate;

            // Compute the ray camera-space Z value at 1/2 fine step (pixel) into the future
		    rayZMax = (dPQK.z * 0.5 + pqk.z) / (dPQK.w * 0.5 + pqk.w);
            rayZMax = clamp(rayZMax, zMin, zMax);

            prevZMaxEstimate = rayZMax;
            rayZMax = max(rayZMax, rayZMin);

            hit_pixel = permute ? pqk.yx : pqk.xy;

            // normalized depth [0, 1]
            scene_z_max = texelFetch(depth_texture, ivec2(hit_pixel), 0).r;
            // linear negative depth
            scene_z_max = -linearize_depth(scene_z_max, getCameraNearPlane(), getCameraFarPlane());
        }

        // Undo the last increment, which happened after the test variables were set up
        pqk -= dPQK;
        refinement_step_count -= 1.0;

        // Count the refinement steps as fractions of the original stride. Save a register
        // by not retaining invStride until here
        step_count += refinement_step_count / stride;
    }
#endif

    Q0.xy += dQ.xy * step_count;
    Q0.z = pqk.z;

    vs_hit_point = Q0 / pqk.w;

    iteration_count = step_count + 1.0;

    return hit;
}
