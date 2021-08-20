#pragma once

#include "../lodepng/lodepng.h"
#include <array>
#include <memory>


using namespace std;

using id = int;

namespace nbtp {

    constexpr int max_failed_time_count = 3;

    struct chunk {
        id cid;
        size_t size;
        unique_ptr<char> data;

        chunk(id chunk_id, size_t chunk_size, unique_ptr<char> &&input_data);

        ~chunk() noexcept;

        chunk(const chunk &) = delete; // uncopyable object.

        chunk(chunk &&other) noexcept;
    };

    struct upload_result {
        id cid = -1;
        string link; // link to get the uploaded image.
    };
}
