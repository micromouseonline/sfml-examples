
// define the level_map with an array of tile indices
// clang-format off
    const int level_map[32*32] =
    {
        0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
        0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,  0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,  0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,  0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
        0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,  0x03, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
        0x00, 0x01, 0x00, 0x00, 0x02, 0x00, 0x03, 0x03,  0x01, 0x03, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00,  0x00, 0x00, 0x01, 0x00, 0x00, 0x02, 0x00, 0x03,  0x03, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,
        0x00, 0x01, 0x01, 0x00, 0x03, 0x03, 0x03, 0x00,  0x01, 0x00, 0x00, 0x01, 0x01, 0x01, 0x02, 0x00,  0x00, 0x00, 0x01, 0x01, 0x00, 0x03, 0x03, 0x03,  0x00, 0x00, 0x01, 0x01, 0x01, 0x02, 0x00, 0x00,
        0x00, 0x00, 0x01, 0x00, 0x03, 0x00, 0x02, 0x02,  0x01, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x02,  0x00, 0x00, 0x00, 0x01, 0x00, 0x03, 0x00, 0x02,  0x02, 0x00, 0x01, 0x01, 0x01, 0x01, 0x02, 0x00,
        0x02, 0x00, 0x01, 0x00, 0x03, 0x00, 0x02, 0x02,  0x01, 0x02, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01,  0x01, 0x02, 0x00, 0x01, 0x00, 0x03, 0x00, 0x02,  0x02, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
        0x00, 0x00, 0x01, 0x00, 0x03, 0x02, 0x02, 0x02,  0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01,  0x01, 0x00, 0x00, 0x01, 0x00, 0x03, 0x02, 0x02,  0x02, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01,

        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
        0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,  0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,  0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,  0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
        0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,  0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
        0x00, 0x01, 0x00, 0x00, 0x02, 0x00, 0x03, 0x03,  0x03, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,  0x00, 0x01, 0x00, 0x00, 0x02, 0x00, 0x03, 0x03,  0x03, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,
        0x00, 0x01, 0x01, 0x00, 0x03, 0x03, 0x03, 0x00,  0x00, 0x00, 0x01, 0x01, 0x01, 0x02, 0x00, 0x00,  0x00, 0x01, 0x01, 0x00, 0x03, 0x03, 0x03, 0x00,  0x00, 0x00, 0x01, 0x01, 0x01, 0x02, 0x00, 0x00,
        0x00, 0x00, 0x01, 0x00, 0x03, 0x00, 0x02, 0x02,  0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x02, 0x00,  0x00, 0x00, 0x01, 0x00, 0x03, 0x00, 0x02, 0x02,  0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x02, 0x00,
        0x02, 0x00, 0x01, 0x00, 0x03, 0x00, 0x02, 0x02,  0x02, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,  0x02, 0x00, 0x01, 0x00, 0x03, 0x00, 0x02, 0x02,  0x02, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
        0x00, 0x00, 0x01, 0x00, 0x03, 0x02, 0x02, 0x02,  0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01,  0x00, 0x00, 0x01, 0x00, 0x03, 0x02, 0x02, 0x02,  0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01,
        
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
        0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,  0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,  0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,  0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
        0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,  0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
        0x00, 0x01, 0x00, 0x00, 0x02, 0x00, 0x03, 0x03,  0x03, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,  0x00, 0x01, 0x00, 0x00, 0x02, 0x00, 0x03, 0x03,  0x03, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,
        0x00, 0x01, 0x01, 0x00, 0x03, 0x03, 0x03, 0x00,  0x00, 0x00, 0x01, 0x01, 0x01, 0x02, 0x00, 0x00,  0x00, 0x01, 0x01, 0x00, 0x03, 0x03, 0x03, 0x00,  0x00, 0x00, 0x01, 0x01, 0x01, 0x02, 0x00, 0x00,
        0x00, 0x00, 0x01, 0x00, 0x03, 0x00, 0x02, 0x02,  0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x02, 0x00,  0x00, 0x00, 0x01, 0x00, 0x03, 0x00, 0x02, 0x02,  0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x02, 0x00,
        0x02, 0x00, 0x01, 0x00, 0x03, 0x00, 0x02, 0x02,  0x02, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,  0x02, 0x00, 0x01, 0x00, 0x03, 0x00, 0x02, 0x02,  0x02, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
        0x00, 0x00, 0x01, 0x00, 0x03, 0x02, 0x02, 0x02,  0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01,  0x00, 0x00, 0x01, 0x00, 0x03, 0x02, 0x02, 0x02,  0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01,
        
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
        0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,  0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,  0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,  0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
        0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,  0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
        0x00, 0x01, 0x00, 0x00, 0x02, 0x00, 0x03, 0x03,  0x03, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,  0x00, 0x01, 0x00, 0x00, 0x02, 0x00, 0x03, 0x03,  0x03, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,
        0x00, 0x01, 0x01, 0x00, 0x03, 0x03, 0x03, 0x00,  0x00, 0x00, 0x01, 0x01, 0x01, 0x02, 0x00, 0x00,  0x00, 0x01, 0x01, 0x00, 0x03, 0x03, 0x03, 0x00,  0x00, 0x00, 0x01, 0x01, 0x01, 0x02, 0x00, 0x00,
        0x00, 0x00, 0x01, 0x00, 0x03, 0x00, 0x02, 0x02,  0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x02, 0x00,  0x00, 0x00, 0x01, 0x00, 0x03, 0x00, 0x02, 0x02,  0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x02, 0x00,
        0x02, 0x00, 0x01, 0x00, 0x03, 0x00, 0x02, 0x02,  0x02, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,  0x02, 0x00, 0x01, 0x00, 0x03, 0x00, 0x02, 0x02,  0x02, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
        0x00, 0x00, 0x01, 0x00, 0x03, 0x02, 0x02, 0x02,  0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01,  0x00, 0x00, 0x01, 0x00, 0x03, 0x02, 0x02, 0x02,  0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01,

    };
// clang-format on