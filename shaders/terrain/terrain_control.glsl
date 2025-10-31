// Efficient inline decode macros - avoid repeated decoding
// Our bit layout: base_id(0-5), extra_id(6-11), blend(12-19), scale(20-22), rotation(23-26), reserved(27-31)

#define DIV_255 0.003921568627450

#define DECODE_BASE_ID(control)   int(uint(control) & 0x3Fu)
#define DECODE_EXTRA_ID(control)  int(uint(control) >> 6u & 0x3Fu)
#define DECODE_BLEND(control)     (float(uint(control) >> 12u & 0xFFu) * DIV_255)
#define DECODE_SCALE(control)     (0.9 - float(((uint(control) >> 20u & 0x7u) + 3u) % 8u + 1u) * 0.1)
#define DECODE_ROTATION(control)  (float(uint(control) >> 23u & 0xFu) * 0.392699081698724)
#define DECODE_RESERVED(control)  (uint(control) >> 27u & 0x1Fu)

// Legacy struct for backward compatibility (if needed)
struct terrain_control {
    uint base_id;
    uint extra_id;
    float blend;
    float scale;
    float rotation;
    uint reserved;
};