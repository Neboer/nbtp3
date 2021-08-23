#include "converter.h"

// input_binary_data length must be default.
packed_storage encode_to_png(packed_storage input_binary_data) {
    octet *output_PNG;
    size_t output_size;
    // normal data
    lodepng_encode_memory(&output_PNG, &output_size, input_binary_data.data.get(), d_shape_width, d_shape_height,
                          LCT_RGBA, 8);
    return {input_binary_data.cid, output_size, unique_ptr<octet>(output_PNG),
            input_binary_data.label};
}

