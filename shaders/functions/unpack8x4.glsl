void unpack8x4(uint value, out uint unpacked[8]) {
    unpacked[0] = (value      ) & 0x0Fu;
    unpacked[1] = (value >>  4u) & 0x0Fu;
    unpacked[2] = (value >>  8u) & 0x0Fu;
    unpacked[3] = (value >> 12u) & 0x0Fu;
    unpacked[4] = (value >> 16u) & 0x0Fu;
    unpacked[5] = (value >> 20u) & 0x0Fu;
    unpacked[6] = (value >> 24u) & 0x0Fu;
    unpacked[7] = (value >> 28u) & 0x0Fu;
}
