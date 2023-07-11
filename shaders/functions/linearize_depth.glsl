/*
 *  Depth value in depth texture stored in [0, 1] range
 *
 *  Normalized Device Coordinates for OpenGL in range [-1, 1]
 *
 */
float linearize_depth(float depth, float near, float far) {
   float z_n = 2.0 * depth - 1.0;
   return 2.0 * near * far / (far + near - z_n * (far - near));
}
