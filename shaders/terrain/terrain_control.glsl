struct terrain_control {
    uint base_id;
    uint extra_id;
    float blend;
    uint reserved;
};

terrain_control decode(uint value) {
    terrain_control tctrl;

    tctrl.base_id = value & 0x3Fu;
    tctrl.extra_id = (value >> 6u) & 0x3Fu;
    tctrl.blend = ((value >> 12u) & 0xFFu) / 255.0;
    tctrl.reserved = (value >> 20u) & 0xFFFu;

    return tctrl;
}